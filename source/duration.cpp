#include "main.h"

durationi bsdata<durationi>::elements[] = {
	{"Instant"},
	{"round/level", 1, 1},
	{"5 rounds/level", 5, 1},
	{"1d4 rounds/level + round/level", 1, 1, 0, {1, 4}},
	{"turn", 0, 0, 10},
	{"turn/level", 10, 1},
	{"hour", 0, 0, 60},
	{"hour/level", 60, 1},
	{"2 hours", 0, 0, 60 * 2},
	{"4 hours", 0, 0, 60 * 4},
	{"8 hours", 0, 0, 60 * 8},
};
assert_enum(duration, Duration8Hours);

int durationi::get(int v) const {
	auto result = 0;
	if(base)
		result += base.roll();
	if(divider)
		result += multiplier * v / divider;
	result += addiction;
	return result;
}