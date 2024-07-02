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

		Rom(const fs::path& path, std::optional<Mapper> mapper = std::nullopt);
		Rom(std::vector<byte>&& bytes, std::optional<Mapper> mapper = std::nullopt);

		Address pc(size_t pc_address) const;
		Address snes(size_t snes_address) const;

		Mapper getMapper() const;
		void setMapper(Mapper mapper);
		void deriveMapper();

		byte read1(Address&& address);

	private:
		std::optional<Mapper> mapper;
	};
}

#endif // ROM_H
