#include "../include/rom.h"

namespace binary_file {
	Rom::Rom(const fs::path& path) : BinaryFile(path) {}
	Rom::Rom(const fs::path& path, Mapper mapper) : BinaryFile(path), mapper(mapper) {}

	Rom::Rom(std::vector<byte>&& bytes) : BinaryFile(std::move(bytes)) {}
	Rom::Rom(std::vector<byte>&& bytes, Mapper mapper) : BinaryFile(std::move(bytes)), mapper(mapper) {}

	void Rom::ensureMapper() {
		if (!mapper.has_value()) {
			deriveMapper();
		}
	}

	Address Rom::pc(size_t pc_address) {
		ensureMapper();

		return Address::PC(pc_address, mapper.value());
	}

	Address Rom::snes(size_t snes_address) {
		ensureMapper();

		return Address::SNES(snes_address, mapper.value());
	}

	std::vector<byte> Rom::getBytes() {
		return bytes;
	}

	std::optional<Mapper> Rom::getMapper() const {
		return mapper;
	}

	void Rom::setMapper(Mapper mapper) {
		this->mapper = mapper;
	}

	void Rom::deriveMapper() {
		int max_score{ -99999 };

		Mapper best_map{ Mapper::LO_ROM };

		std::array<Mapper, 4> maps{
			Mapper::LO_ROM,
			Mapper::HI_ROM,
			Mapper::EX_LO_ROM,
			Mapper::EX_HI_ROM
		};

		for (auto map : maps) {
			int score{ 0 };
			int high_bits{ 0 };
			bool found_null{ false };

			for (size_t i{ 0 }; i != 21; ++i) {
				uint8_t c{ read1(Address::SNES(0x00FFC0 + i, map)) };
				
				if (found_null && c) {
					score -= 4;
				}

				if (c >= 128) {
					++high_bits;
				}
				else if (is_upper(c)) {
					score += 3;
				}
				else if (c == ' ') {
					score += 2;
				}
				else if (is_digit(c)) {
					score += 1;
				}
				else if (is_lower(c)) {
					score += 1;
				}
				else if (c == '-') {
					score += 1;
				}
				else if (!c) {
					found_null = true;
				}
				else {
					score -= 3;
				}
			}

			if (high_bits > 0 && high_bits <= 14) {
				score -= 21;
			}

			if ((read1(Address::SNES(0x00FFDE, map)) ^ read1(Address::SNES(0x00FFDC, map))) != 0xFF ||
				((read1(Address::SNES(0x00FFDF, map)) ^ read1(Address::SNES(0x00FFDD, map))) != 0xFF)) {
				score -= 99999;
			}

			if (score > max_score) {
				max_score = score;
				best_map = map;
			}
		}

		mapper = best_map;

		const auto mapper_byte{ read1(Address::SNES(0x00FFD5, mapper.value())) };
		const auto rom_type_byte{ read1(Address::SNES(0x00FFD6, mapper.value())) };
		if (mapper == Mapper::LO_ROM) {
			if (mapper_byte == 0x23 && (rom_type_byte == 0x32 || rom_type_byte == 0x34 || rom_type_byte == 0x35)) {
				mapper = Mapper::SA1_ROM;
			}
		}
	}

	byte Rom::read1(Address&& address) const {
		try {
			return BinaryFile::read1(address.pc());
		} catch (const BinaryFileException&) {
			throw BinaryFileException(fmt::format(
				"Invalid read of one byte at {}",
				address.string()
			));
		}
	}

	// these reads might be slow, but they should make sure bank crossing is handled right
	_2bytes Rom::read2(Address&& address) const {
		auto second{ address + 1 };

		return BinaryFile::join({
			read1(std::forward<Address>(address)),
			read1(std::move(second))
		});
	}

	_4bytes Rom::read3(Address&& address) const {
		auto second{ address + 1 };
		auto third{ address + 2 };

		return BinaryFile::join({
			read1(std::forward<Address>(address)),
			read1(std::move(second)),
			read1(std::move(third))
		});
	}

	_4bytes Rom::read4(Address&& address) const {
		auto second{ address + 1 };
		auto third{ address + 2 };
		auto fourth{ address + 3 };

		return BinaryFile::join({
			read1(std::forward<Address>(address)),
			read1(std::move(second)),
			read1(std::move(third)),
			read1(std::move(fourth))
		});
	}

	void Rom::write1(Address&& address, byte byte_to_write) {
		try {
			return BinaryFile::write1(address.pc(), byte_to_write);
		}
		catch (const BinaryFileException&) {
			throw BinaryFileException(fmt::format(
				"Invalid write of one byte 0x{:02X} at {}",
				byte_to_write, address.string()
			));
		}
	}

	void Rom::write2(Address&& address, _2bytes bytes_to_write) {
		const auto split{ BinaryFile::split(bytes_to_write, 2) };

		auto second{ address + 1 };

		write1(std::forward<Address>(address), split[0]);
		write1(std::move(second), split[1]);
	}

	void Rom::write3(Address&& address, _4bytes bytes_to_write) {
		const auto split{ BinaryFile::split(bytes_to_write, 3) };

		auto second{ address + 1 };
		auto third{ address + 2 };

		write1(std::forward<Address>(address), split[0]);
		write1(std::move(second), split[1]);
		write1(std::move(third), split[2]);
	}

	void Rom::write4(Address&& address, _4bytes bytes_to_write) {
		const auto split{ BinaryFile::split(bytes_to_write, 4) };

		auto second{ address + 1 };
		auto third{ address + 2 };
		auto fourth{ address + 3 };

		write1(std::forward<Address>(address), split[0]);
		write1(std::move(second), split[1]);
		write1(std::move(third), split[2]);
		write1(std::move(fourth), split[3]);
	}
}
