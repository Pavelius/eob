#pragma once

struct looti {
	int			gold, experience, reputation, blessing;
	constexpr operator bool() { return gold || experience || reputation || blessing; }
	void		clear();
};
extern looti last_loot;
