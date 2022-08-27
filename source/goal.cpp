#include "main.h"

BSDATA(goali) = {
	{"KillBoss", "Boss killed"},
	{"KillAlmostAllMonsters", "Monsters killed"},
	{"ExploreMostDungeon", "Explored levels"},
	{"GrabAlmostAllGems", "Collect gems"},
	{"GrabAllSpecialItems", "Collect special items"},
};
assert_enum(goali, GrabAllSpecialItems)
BSDATAF(goali)