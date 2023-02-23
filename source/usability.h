#include "cflags.h"

#pragma once

enum usability_s : unsigned char {
	UseLeatherArmor, UseMetalArmor, UseShield,
	UseTheifWeapon, UseMartialWeapon, UseLargeWeapon,
	UseScrolls, UseDivine, UseArcane, UseTheif
};
typedef cflags<usability_s> usabilitya;
