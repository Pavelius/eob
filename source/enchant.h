#pragma once

enum enchant_s : unsigned char {
	OfAccuracy, OfCold,
	OfEnergyDrain,
	OfFear, OfFire, OfHolyness,
	OfParalize, OfPoison,
	OfSharpness, OfSmashing, OfSpeed, OfStrenghtDrain,
	OfVampirism, OfWizardy,
};
struct enchanti {
	const char*	name;
};
