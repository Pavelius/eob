#include "dice.h"

#pragma once

enum duration_s : unsigned char {
	Instant,
	Duration1PerLevel, Duration1d4P1PerLevel,
	Duration5PerLevel,
	DurationTurn, DurationTurnPerLevel,
	DurationHour, Duration1HourPerLevel, Duration2Hours, Duration4Hours, Duration8Hours,
};
struct durationi {
	const char*	name;
	unsigned	multiplier, divider, addiction;
	dice		base;
	int			get(int v) const;
};
