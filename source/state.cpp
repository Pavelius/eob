#include "main.h"

statei bsmeta<statei>::elements[] = {{"No state"},
{"Armored", SaveVsMagic},
{"Blessed", SaveVsMagic},
{"Climbed", SaveVsMagic},
{"Detected Evil", SaveVsMagic},
{"Detected Magic", SaveVsMagic},
{"Fire resisted", SaveVsMagic},
{"Invisible", SaveVsMagic},
{"Hasted", SaveVsMagic},
{"Protected from evil", SaveVsMagic},
{"Shielded", SaveVsMagic},
{"Sleeped", SaveVsMagic},
{"Speakable", SaveVsMagic},
{"Strenghted", SaveVsMagic},
{"Scared", SaveVsMagic},
//
{"Paralized", SaveVsParalization},
{"Weak Poison", SaveVsPoison},
{"Poison", SaveVsPoison},
{"Strong Poison", SaveVsPoison},
{"Lethal Poison", SaveVsPoison},
};
assert_enum(state, DeadlyPoison);