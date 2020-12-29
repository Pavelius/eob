#include "main.h"

BSDATA(buildingi) = {{"Arena", {}},
{"Armory", {}},
{"Bank"},
{"Brothel"},
{"Library", {""}, ""},
{"Harbor", {""}, "In harbor present many ships at now. You can travel from here to any settlement where harbor present."},
{"Prison"},
{"Shop"},
{"Stable"},
{"Stock"},
{"Tavern", {"tavern16"}, "Noisy tavern were full of folks. It drinks and play games. Smells good food.", {Drink, Rest, Gambling}},
{"Temple"},
{"Wizard Tower"},
};
INSTELEM(buildingi)
assert_enum(building, WizardTower)