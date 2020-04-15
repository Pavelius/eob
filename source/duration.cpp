#include "main.h"

durationi bsdata<durationi>::elements[] = {{"Instant"},
{"round/level", 1, 1},
{"5 rounds/level", 5, 1},
{"turn", 0, 0, 10},
{"turn/level", 10, 1},
{"hour", 0, 0, 60},
{"2 hours", 0, 0, 60 * 2},
{"4 hours", 0, 0, 60 * 4},
{"8 hours", 0, 0, 60 * 8},
};
assert_enum(duration, Duration8Hours);

int durationi::get(int v) const {
	if(!divider)
		return addiction;
	return multiplier * v / divider + addiction;
}