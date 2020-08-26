#include "main.h"

attacki bsdata<attacki>::elements[] = {{"Automatic hit", 0},
{"1 attack", 2},
{"3/2 attack", 3, "3/2"},
{"2 attacks", 4, "2"},
{"If one hit", 0},
{"If all hits", 0},
{"On critical hit", 0},
};
assert_enum(attack, OnCriticalHit)
INSTELEM(attacki)