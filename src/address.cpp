#include "../include/address.h"

namespace binary_file {
	int sa1banks[8] = { 0 << 20, 1 << 20, -1, -1, 2 << 20, 3 << 20, -1, -1 };

	Address::Address(size_t address, std::optional<Mapper> mapper, bool is_pc) :
		mapper(mapper),
		pc_address(is_pc ? std::make_optional(address) : std::nullopt),
		snes_address(is_pc ? std::nullopt : std::make_optional(address)) {}

	Address Address::PC(size_t pc_address) {
		return Address(pc_address, std::nullopt, true);
	}	
	
	Address Address::PC(size_t pc_address, Mapper mapper) {
		return Address(pc_address, mapper, true);
	}

	Address Address::SNES(size_t snes_address, Mapper mapper) {
		return Address(snes_address, mapper, false);
	}

	size_t Address::pc() {
		ensurePcAddress();

		return pc_address.value();
	}

	size_t Address::snes() {
		ensureSnesAddress();

		return snes_address.value();
	}

	Address& Address::operator+=(const size_t rhs) {
		ensureSnesAddress();
		snes_address = snes_address.value() + rhs;
		updatePcFromSnes();

		return *this;
	}

	Address& Address::operator-=(const size_t rhs) {
		ensureSnesAddress();
		snes_address = snes_address.value() - rhs;
		updatePcFromSnes();

		return *this;
	}

	Address Address::operator+(const size_t rhs) {
		ensureSnesAddress();

		return Address(snes_address.value() + rhs, mapper, false);
	}

	Address Address::operator-(const size_t rhs) {
		ensureSnesAddress();

		return Address(snes_address.value() - rhs, mapper, false);
	}

	Address& Address::operator++() {
		ensureSnesAddress();
		snes_address = snes_address.value() + 1;
		updatePcFromSnes();

		return *this;
	}

	Address Address::operator++(int) {
		Address old{ *this };
		operator++();
		return old;
	}

	Address& Address::operator--() {
		ensureSnesAddress();
		snes_address = snes_address.value() - 1;
		updatePcFromSnes();

		return *this;
	}

	Address Address::operator--(int) {
		Address old{ *this };
		operator--();
		return old;
	}

	std::string Address::string() {
		try {
			ensurePcAddress();
		}
		catch (...) {
			// pass
		}

		try {
			ensureSnesAddress();
		}
		catch (...) {
			// pass
		}

		if (snes_address.has_value() && pc_address.has_value()) {
			return fmt::format(
				"Address[SNES: ${:06X}, PC: 0x{:06X}]",
				snes_address.value(), pc_address.value()
			);
		}
		else if (snes_address.has_value()) {
			return fmt::format(
				"Address[SNES: ${:06X}]",
				snes_address.value()
			);
		}
		else if (pc_address.has_value()) {
			return fmt::format(
				"Address[PC: 0x{:06X}]",
				pc_address.value()
			);
		}
		else {
			return "Address[Invalid]";
		}
	}

	size_t Address::pcToSnes(size_t pc_address) {
		if (!mapper.has_value()) {
			throw MissingMapperException(fmt::format(
				"No mapper specified trying to convert 0x{:06X} (PC) to SNES",
				pc_address
			));
		}

		switch (mapper.value()) {
		case Mapper::LO_ROM:
			return pcToLoRom(pc_address);

		case Mapper::HI_ROM:
			return pcToHiRom(pc_address);

		case Mapper::EX_LO_ROM:
			return pcToExLoRom(pc_address);

		case Mapper::EX_HI_ROM:
			return pcToExHiRom(pc_address);
		
		case Mapper::SA1_ROM:
			return pcToSa1Rom(pc_address);

		case Mapper::BIG_SA1_ROM:
			return pcToBigSa1Rom(pc_address);

		case Mapper::SFX_ROM:
			return pcToSfxRom(pc_address);

		case Mapper::NO_ROM:
		default:
			return pcToNoRom(pc_address);
		}
	}

	size_t Address::snesToPc(size_t snes_address) {
		if (!mapper.has_value()) {
			throw MissingMapperException(fmt::format(
				"No mapper specified trying to convert ${:06X} (SNES) to PC",
				snes_address
			));
		}

		if (snes_address > 0xFFFFFF) {
			throwInvalidSnesAddress(snes_address);
		}

		switch (mapper.value()) {
		case Mapper::LO_ROM:
			return loRomToPc(snes_address);

		case Mapper::HI_ROM:
			return hiRomToPc(snes_address);

		case Mapper::EX_LO_ROM:
			return exLoRomToPc(snes_address);

		case Mapper::EX_HI_ROM:
			return exHiRomToPc(snes_address);

		case Mapper::SA1_ROM:
			return sa1RomToPc(snes_address);

		case Mapper::BIG_SA1_ROM:
			return bigSa1RomToPc(snes_address);

		case Mapper::SFX_ROM:
			return sfxRomToPc(snes_address);

		case Mapper::NO_ROM:
		default:
			return noRomToPc(snes_address);
		}
	}

	size_t Address::pcToLoRom(size_t pc_address) {
		if (pc_address >= 0x400000) {
			throwInvalidPcAddress(pc_address);
		}
		pc_address = ((pc_address << 1) & 0x7F0000) | (pc_address & 0x7FFF) | 0x8000;
		return pc_address | 0x800000;
	}

	size_t Address::pcToHiRom(size_t pc_address) {
		if (pc_address >= 0x400000) {
			throwInvalidPcAddress(pc_address);
		}
		return pc_address | 0xC00000;
	}

	size_t Address::pcToExLoRom(size_t pc_address) {
		if (pc_address >= 0x800000) {
			throwInvalidPcAddress(pc_address);
		}

		if (pc_address & 0x400000) {
			pc_address -= 0x400000;
			pc_address = ((pc_address << 1) & 0x7F0000 | (pc_address & 0x7FFF) | 0x8000);
			return pc_address;
		}
		else {
			pc_address = ((pc_address << 1) & 0x7F0000) | (pc_address & 0x7FFF) | 0x8000;
			return pc_address | 0x800000;
		}
	}

	size_t Address::pcToExHiRom(size_t pc_address) {
		if (pc_address >= 0x800000) {
			throwInvalidPcAddress(pc_address);
		}

		if (pc_address & 0x400000) {
			return pc_address;
		}

		return pc_address | 0xC00000;
	}

	size_t Address::pcToSa1Rom(size_t pc_address) {
		for (size_t i{ 0 }; i != 8; ++i) {
			if (sa1banks[i] == (pc_address & 0x700000)) {
				return 0x008000 | (i << 21) | ((pc_address & 0x0F8000) << 1) | (pc_address & 0x7FFF);
			}
		}

		throwInvalidPcAddress(pc_address);

		return pc_address; // unreachable, just making the compiler be quiet
	}

	size_t Address::pcToBigSa1Rom(size_t pc_address) {
		if (pc_address >= 0x800000) {
			throwInvalidPcAddress(pc_address);
		}

		if ((pc_address & 0x400000) == 0x400000) {
			return pc_address | 0xC00000;
		}

		if ((pc_address & 0x600000) == 0x000000) {
			return ((pc_address << 1) & 0x3F0000) | 0x8000 | (pc_address & 0x7FFF);
		}

		if ((pc_address & 0x600000) == 0x200000) {
			return 0x800000 | ((pc_address << 1) & 0x3F0000) | 0x8000 | (pc_address & 0x7FFF);
		}

		throwInvalidPcAddress(pc_address);

		return pc_address; // unreachable, just making the compiler be quiet
	}

	size_t Address::pcToSfxRom(size_t pc_address) {
		if (pc_address >= 0x200000) {
			throwInvalidPcAddress(pc_address);
		}

		return ((pc_address << 1) & 0x7F0000) | (pc_address & 0x7FFF) | 0x8000;
	}

	size_t Address::pcToNoRom(size_t pc_address) {
		return pc_address;
	}

	size_t Address::loRomToPc(size_t snes_address) {
		if ((snes_address & 0xFE0000) == 0x7E0000 ||
			(snes_address & 0x408000) == 0x000000 ||
			(snes_address & 0x708000) == 0x700000) {
			throwInvalidSnesAddress(snes_address);
		}

		return ((snes_address & 0x7F0000) >> 1 | (snes_address & 0x7FFF));
	}

	size_t Address::hiRomToPc(size_t snes_address) {
		if ((snes_address & 0xFE0000) == 0x7E0000 ||
			(snes_address & 0x408000) == 0x000000) {
			throwInvalidSnesAddress(snes_address);
		}

		return snes_address & 0x3FFFFF;
	}

	size_t Address::exLoRomToPc(size_t snes_address) {
		if ((snes_address & 0xF00000) == 0x700000 ||
			(snes_address & 0x408000) == 0x000000) {
			throwInvalidSnesAddress(snes_address);
		}

		if (snes_address & 0x800000) {
			snes_address = ((snes_address & 0x7F0000) >> 1 | (snes_address & 0x7FFF));
		}
		else {
			snes_address = ((snes_address & 0x7F0000) >> 1 | (snes_address & 0x7FFF)) + 0x400000;
		}

		return snes_address;
	}

	size_t Address::exHiRomToPc(size_t snes_address) {
		if ((snes_address & 0xFE0000) == 0x7E0000 ||
			(snes_address & 0x408000) == 0x000000) {
			throwInvalidSnesAddress(snes_address);
		}

		if ((snes_address & 0x800000) == 0x000000) {
			return (snes_address & 0x3FFFFF) | 0x400000;
		}

		return snes_address & 0x3FFFFF;
	}

	size_t Address::sa1RomToPc(size_t snes_address) {
		if ((snes_address & 0x408000) == 0x008000) {
			return sa1banks[(snes_address & 0xE00000) >> 21] | ((snes_address & 0x1F0000) >> 1) | (snes_address & 0x007FFF);
		}

		if ((snes_address & 0xC00000) == 0xC00000) {
			return sa1banks[((snes_address & 0x100000) >> 20) | ((snes_address & 0x200000) >> 19)] | (snes_address & 0x0FFFFF);
		}

		throwInvalidSnesAddress(snes_address);

		return snes_address; // unreachable, just making the compiler be quiet
	}

	size_t Address::bigSa1RomToPc(size_t snes_address) {
		if ((snes_address & 0xC00000) == 0xC00000) {
			return (snes_address & 0x3FFFFF) | 0x400000;
		}

		if ((snes_address & 0xC00000) == 0x000000 || (snes_address & 0xC00000) == 0x800000) {
			if ((snes_address & 0x008000) == 0x000000) {
				throwInvalidSnesAddress(snes_address);
			}
			else {
				return (snes_address & 0x800000) >> 2 | (snes_address & 0x3F0000) >> 1 | (snes_address & 0x7FFF);
			}
		}

		throwInvalidSnesAddress(snes_address);

		return snes_address; // unreachable, just making the compiler be quiet
	}

	size_t Address::sfxRomToPc(size_t snes_address) {
		if ((snes_address & 0x600000) == 0x600000 ||
			(snes_address & 0x408000) == 0x000000 ||
			(snes_address & 0x800000) == 0x800000) {
			throwInvalidSnesAddress(snes_address);
		}

		if (snes_address & 0x400000) {
			return snes_address & 0x3FFFFF;
		}

		return (snes_address & 0x7F0000) >> 1 | (snes_address & 0x7FFF);
	}

	size_t Address::noRomToPc(size_t snes_address) {
		return snes_address;
	}
	
	void Address::ensureSnesAddress() {
		if (!snes_address.has_value()) {
			snes_address = pcToSnes(pc_address.value());
		}
	}
	
	void Address::ensurePcAddress() {
		if (!pc_address.has_value()) {
			pc_address = snesToPc(snes_address.value());
		}
	}
	
	void Address::updatePcFromSnes() {
		if (pc_address.has_value()) {
			pc_address = snesToPc(snes_address.value());
		}
	}

	void Address::throwInvalidPcAddress(size_t pc_address) {
		throw InvalidAddressException(fmt::format(
			"PC address 0x{:06X} is not valid and cannot be converted to a SNES address",
			pc_address
		));
	}

	void Address::throwInvalidSnesAddress(size_t snes_address) {
		throw InvalidAddressException(fmt::format(
			"SNES address ${:06X} is not valid and cannot be converted to a PC address",
			snes_address
		));
	}

	std::ostream& operator<<(std::ostream& target, Address& source) {
		target << source.string();
		return target;
	}
}
