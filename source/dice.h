#include "stringbuilder.h"

#pragma once

struct dice {
	unsigned char		c, d;
	char				b, m;
	constexpr explicit operator bool() const { return c != 0; }
	static dice			create(int c, int d, int b = 0, int m = 0) { return{(unsigned char)c, (unsigned char)d, (char)b, (char)m}; }
	void				clear() { c = 0; d = 0; m = 0; b = 0; }
	constexpr int		maximal() const { return c * d + b; }
	void				print(stringbuilder& sb) const;
	const char*			range(char* result, const char* result_maximum) const;
	int					roll() const { return roll(c, d) + b; }
	static int			roll(int c, int d);
};