#include "main.h"

statei bsmeta<statei>::elements[] = {{"No state"},
{"Anger", SaveVsMagic},
{"Food", SaveVsMagic},
{"Armored", SaveVsMagic},
{"Blessed", SaveVsMagic},
{"Detected Evil", SaveVsMagic},
{"Detected Magic", SaveVsMagic},
{"Fire resistant", SaveVsMagic},
{"Invisible", SaveVsMagic},
{"Hasted", SaveVsMagic},
{"Protected from evil", SaveVsMagic},
{"Shielded", SaveVsMagic},
{"Sleeped", SaveVsMagic},
{"Speakable", SaveVsMagic},
{"Strenghted", SaveVsMagic},
{"Turned", SaveVsMagic},
//
{"Paralized", SaveVsParalization},
{"Weak Poison", SaveVsPoison},
{"Poison", SaveVsPoison},
{"Strong Poison", SaveVsPoison},
{"Lethal Poison", SaveVsPoison},
};
assert_enum(state, DeadlyPoison);