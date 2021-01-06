#include "main.h"

BSDATA(buildingi) = {{"Arena"},
{"Armory", "#BUILDNGS 0\nThis place is full of weapon for sale. You can buy new weapon or sell.", {Buy, Sell}, {Armors, Weapons}},
{"Bank"},
{"Brothel"},
{"Inn", "#BUILDNGS 19\nOld fashioned inn with small, but warm rooms.", {Rest, Talk}},
{"Library"},
{"Harbor", "#BUILDNGS 2\nIn harbor present many ships at now. You can travel from here to any settlement where harbor present.", {Travel}},
{"Prison"},
{"Stable", "#BUILDNGS 4\nStable has caravans, that leave city in any directions. You can reach at distant location for some gold coins.", {Travel}},
{"Stock"},
{"Tavern", "#BUILDNGS 5\nNoisy tavern were full of folks. It drinks and play games. Smells good food.", {Drink, Rest, Gambling}},
{"Temple", "#BUILDNGS 8\nSilent temple of good god. You can heal here and keep you soul in safe.", {}},
{"Tower", "#BUILDNGS 3\nIn this shop you can buy or sell weird magical devices and can identify weird items.", {Buy, Sell}, {Devices}},
};
INSTELEM(buildingi)
assert_enum(building, WizardTower)