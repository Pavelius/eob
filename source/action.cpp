#include "main.h"

BSDATA(actioni) = {{"Creeting"},
{"Drink"},
{"Gamble"},
{"Leave"},
{"Lie"},
{"Rest"},
{"Trade"},
{"Bribe"},
{"Talk"},
{"Repair"},
{"Attack"},
{"Pet"},
{"Fail lie roll"},
{"Talk about artifact", true},
{"Talk about cursed item", true},
{"Talk about magic item", true},
{"Talk about magic loot on the ground", true},
{"Talk about magic loot in cellar", true},
{"Talk about history this place", true},
{"Rumor", true},
};
assert_enum(action, TalkRumor)
INSTELEM(actioni)