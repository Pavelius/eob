#include "stringbuilder.h"

#pragma once

struct dice {
	char				c, d, b, m;
	constexpr explicit operator bool() const { return c != 0; }
	static dice			create(int c, int d, int b = 0, int m = 0) { return{(char)c, (char)d, (char)b, (char)m}; }
	void				clear() { c = d = m = b = 0; }
	constexpr int		maximal() const { return c * d + b; }
	void				print(stringbuilder& sb) const;
	const char*			range(stringbuilder& sb) const;
	int					roll() const { return roll(c, d) + b; }
	static int			roll(int c, int d);
};