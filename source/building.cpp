#include "main.h"

BSDATA(buildingi) = {{"Outside", {BUILDNGS, 1}, "", {Travel}},
{"Arena", {}},
{"Armory", {BUILDNGS, 1}, "This place is full of weapon for sale. You can buy new weapon or sell.", {Buy, Sell}, {Armors, Weapons}},
{"Bank"},
{"Brothel"},
{"Library", {}, ""},
{"Harbor", {BUILDNGS, 1}, "In harbor present many ships at now. You can travel from here to any settlement where harbor present.", {Travel}},
{"Prison"},
{"Stable", {BUILDNGS, 1}, "Stable has caravans, that leave city in any directions. You can reach at distant location for some gold coins.", {Travel}},
{"Stock"},
{"Tavern", {BUILDNGS, 1}, "Noisy tavern were full of folks. It drinks and play games. Smells good food.", {Drink, Rest, Gambling}},
{"Temple", {BUILDNGS, 1}, "Silent temple of good god. You can heal here and keep you soul in safe.", {}},
{"Tower", {BUILDNGS, 1}, "In this shop you can buy or sell weird magical devices and can identify weird items.", {Buy, Sell}, {Devices}},
};
INSTELEM(buildingi)
assert_enum(building, WizardTower)