#include "main.h"

BSDATA(actioni) = {{"Nothing to do", 0},
{"Heal all party"},
{"Ressurect bones"},
{"Start combat"},
{"Leave away"},
{"Win combat"},
{"Party gain experience"},
//{"Each player gain experience"},
{"Start trade"},
{"Each player make death save"},
{"Trap inflict damage"},
{"Each player has", 1},
{"Some players has", 1},
{"Each player don't has", 1},
{"Some players don't has", 1},
{"Add item"},
{"Remove item"},
};
assert_enum(action, RemoveItem)
INSTELEM(actioni)