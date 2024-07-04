#ifndef MAPPER_H
#define MAPPER_H

namespace binary_file {
	enum class Mapper {
		LO_ROM,
		HI_ROM,
		SA1_ROM,
		BIG_SA1_ROM,
		SFX_ROM,
		EX_LO_ROM,
		EX_HI_ROM,
		NO_ROM
	};
}

#endif // MAPPER_H
