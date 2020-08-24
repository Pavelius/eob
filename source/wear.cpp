#include "main.h"

INSTDATA(weari) = {{"Backpack"},
{"Backpack+1"},
{"Backpack+2"},
{"Backpack+3"},
{"Backpack+4"},
{"Backpack+5"},
{"Backpack+6"},
{"Backpack+7"},
{"Backpack+8"},
{"Backpack+9"},
{"Backpack+10"},
{"Backpack+11"},
{"Backpack+12"},
{"Backpack+13"},
{"Head"},
{"Neck"},
{"Body"},
{"Right hand"},
{"Left hand"},
{"Right ring"},
{"Left ring"},
{"Elbow"},
{"Legs"},
{"Quiver"},
{"Belt"},
{"Belt+1"},
{"Belt+2"},
};
assert_enum(wear, LastBelt)
INSTELEM(weari)
DGMETA(weari) = {{0, "Name", DGREQ(name)},
{}};