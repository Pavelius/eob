#include "main.h"

BSDATA(buildingi) = {{"Arena", {}},
{"Armory", {}},
{"Bank"},
{"Brothel"},
{"Library"},
{"Harbor", "", "In harbor present many ships. You can travel to any land where harbor present."},
{"Prison"},
{"Shop"},
{"Stable"},
{"Stock"},
{"Tavern", "tavern16"},
{"Temple"},
{"Wizard Tower"},
};
INSTELEM(buildingi)
assert_enum(building, WizardTower)