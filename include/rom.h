#ifndef ROM_H
#define ROM_H

#include <array>

#include "binary_file.h"
#include "libstr.h"
#include "address.h"
#include "mapper.h"

namespace binary_file {
	class Rom : public BinaryFile {
	public:
		using BinaryFile::BinaryFile;

		Rom(const fs::path& path);
		Rom(const fs::path& path, Mapper mapper);

		Rom(std::vector<byte>&& bytes);
		Rom(std::vector<byte>&& bytes, Mapper mapper);

		void ensureMapper();

		Address pc(size_t pc_address);
		Address snes(size_t snes_address);

		std::vector<byte> getBytes();

		std::optional<Mapper> getMapper() const;
		void setMapper(Mapper mapper);
		void deriveMapper();

		// note reads are always on SNES addresses, relevant for bank breaks and whatever
		byte read1(Address&& address) const;
		_2bytes read2(Address&& address) const;
		_4bytes read3(Address&& address) const;
		_4bytes read4(Address&& address) const;

		void write1(Address&& address, byte byte_to_write);
		void write2(Address&& address, _2bytes bytes_to_write);
		void write3(Address&& address, _4bytes bytes_to_write);
		void write4(Address&& address, _4bytes bytes_to_write);

	private:
		std::optional<Mapper> mapper;
	};
}

#endif // ROM_H
