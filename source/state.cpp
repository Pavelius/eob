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
};
assert_enum(state, Paralized);