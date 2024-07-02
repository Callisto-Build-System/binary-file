#include "../include/rom.h"

namespace binary_file {
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

	byte Rom::read1(Address&& address) {
		return BinaryFile::read1(address.pc());
	}
}
