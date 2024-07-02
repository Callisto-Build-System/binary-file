#ifndef LIBSTR_H
#define LIBSTR_H

namespace binary_file {
	extern const unsigned char char_props[256];

	inline bool is_space(unsigned char c) { return char_props[c] & 0x80; } // C standard says \f \v are space, but this one disagrees
	inline bool is_digit(unsigned char c) { return char_props[c] & 0x40; }
	inline bool is_alpha(unsigned char c) { return char_props[c] & 0x20; }
	inline bool is_lower(unsigned char c) { return char_props[c] & 0x04; }
	inline bool is_upper(unsigned char c) { return char_props[c] & 0x02; }
	inline bool is_alnum(unsigned char c) { return char_props[c] & 0x60; }
	inline bool is_ualpha(unsigned char c) { return char_props[c] & 0x28; }
	inline bool is_ualnum(unsigned char c) { return char_props[c] & 0x68; }
	inline bool is_xdigit(unsigned char c) { return char_props[c] & 0x01; }
}

#endif // LIBSTR_H
