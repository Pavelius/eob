#include "main.h"

BSDATA(buildingi) = {{"Arena", {}},
{"Armory", {"forge"}, "This place is full of weapon for sale. You can buy new weapon or sell.", {Buy, Sell}, {Armors, Weapons}},
{"Bank"},
{"Brothel"},
{"Library", {""}, ""},
{"Harbor", {"harbor2"}, "In harbor present many ships at now. You can travel from here to any settlement where harbor present."},
{"Prison"},
{"Stable", {"stable"}, "Stable has caravans, that leave city in any directions. You can reach at distant location for some gold coins."},
{"Stock"},
{"Tavern", {"tavern16"}, "Noisy tavern were full of folks. It drinks and play games. Smells good food.", {Drink, Rest, Gambling}},
{"Temple", {"temple"}, "Silent temple of good god. You can heal here and keep you soul in safe.", {}},
{"Wizard Tower"},
};
INSTELEM(buildingi)
assert_enum(building, WizardTower)