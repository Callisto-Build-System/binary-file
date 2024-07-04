#ifndef ADDRESS_H
#define ADDRESS_H

#include <optional>
#include <ostream>

#include "mapper.h"
#include "exception.h"
#include "fmt/format.h"

namespace binary_file {
	extern int sa1banks[8];//only 0, 1, 4, 5 are used

	class MissingMapperException : public BinaryFileException {
	public:
		using BinaryFileException::BinaryFileException;
	};

	class InvalidAddressException : public BinaryFileException {
	public:
		using BinaryFileException::BinaryFileException;
	};

	class Address {
	private:
		std::optional<Mapper> mapper;

		std::optional<size_t> pc_address;
		std::optional<size_t> snes_address;

		Address(size_t address, std::optional<Mapper> mapper, bool is_pc);

		size_t pcToSnes(size_t pc_address);
		size_t snesToPc(size_t snes_address);

		size_t pcToLoRom(size_t pc_address);
		size_t pcToHiRom(size_t pc_address);
		size_t pcToExLoRom(size_t pc_address);
		size_t pcToExHiRom(size_t pc_address);
		size_t pcToSa1Rom(size_t pc_address);
		size_t pcToBigSa1Rom(size_t pc_address);
		size_t pcToSfxRom(size_t pc_address);
		size_t pcToNoRom(size_t pc_address);

		size_t loRomToPc(size_t snes_address);
		size_t hiRomToPc(size_t snes_address);
		size_t exLoRomToPc(size_t snes_address);
		size_t exHiRomToPc(size_t snes_address);
		size_t sa1RomToPc(size_t snes_address);
		size_t bigSa1RomToPc(size_t snes_address);
		size_t sfxRomToPc(size_t snes_address);
		size_t noRomToPc(size_t snes_address);

		void ensureSnesAddress();
		void ensurePcAddress();

		void updatePcFromSnes();

		static void throwInvalidPcAddress(size_t pc_address);
		static void throwInvalidSnesAddress(size_t snes_address);

	public:
		static Address PC(size_t pc_address);
		static Address PC(size_t pc_address, Mapper mapper);
		static Address SNES(size_t snes_address, Mapper mapper);

		size_t pc();
		size_t snes();

		Address& operator+=(const size_t rhs);
		Address& operator-=(const size_t rhs);		
		
		Address operator+(const size_t rhs);
		Address operator-(const size_t rhs);

		Address& operator++();
		Address operator++(int);

		Address& operator--();
		Address operator--(int);

		std::string string();

		friend std::ostream& operator<<(std::ostream& target, Address& source);
	};

	std::ostream& operator<<(std::ostream& target, Address& source);
}

#endif // ADDRESS_H
