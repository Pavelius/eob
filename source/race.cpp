#include "main.h"

BSDATA(racei) = {
	{1, "Dwarf", {8, 3, 11, 3, 3, 3}, {18, 17, 18, 18, 18, 17}, {0, 0, 1, 0, 0, -1},
	{BonusSaveVsPoison, BonusSaveVsSpells, BonusToHitVsGoblinoid, BonusACVsLargeEnemy}, {UseLargeWeapon},
	{{OpenLocks, 10}, {RemoveTraps, 15}, {ClimbWalls, -10}, {ReadLanguages, -10}}},
	{1, "Elf", {3, 6, 7, 8, 3, 8}, {18, 18, 18, 18, 18, 18}, {0, 1, -1, 0, 0, 0},
	{BonusVsElfWeapon}, {UseLargeWeapon},
	{{ResistCharm, 90}, {DetectSecrets, 15}, {OpenLocks, -5}, {MoveSilently, 5}, {HearNoise, 5}}},
	{1, "Half-elf", {3, 6, 6, 4, 3, 3}, {18, 18, 18, 18, 18, 18}, {0, 0, 0, 0, 0, 0},
	{}, {UseLargeWeapon},
	{{ResistCharm, 90}, {DetectSecrets, 15}, {MoveSilently, 5}}},
	{1, "Halfling", {7, 7, 10, 6, 3, 3}, {18, 18, 18, 18, 17, 18}, {-1, 1, 0, 0, 0, 0},
	{BonusSaveVsPoison, BonusSaveVsSpells}, {},
	{{OpenLocks, 5}, {MoveSilently, 10}, {HearNoise, 5}, {ClimbWalls, -15}, {ReadLanguages, -5}}},
	{1, "Human", {3, 3, 3, 3, 3, 3}, {18, 18, 18, 18, 18, 18}, {0, 0, 0, 0, 0, 0},
	{}, {UseLargeWeapon}},
	//
	{0, "Humanoid", {3, 3, 3, 3, 3, 3}, {25, 25, 25, 25, 25, 25}, {}, {}, {UseLargeWeapon}},
	{0, "Goblinoid", {3, 3, 3, 3, 3, 3}, {18, 18, 18, 18, 18, 18}, {}, {}, {UseLargeWeapon}},
	{0, "Insectoid", {3, 3, 3, 1, 3, 3}, {25, 25, 25, 10, 10, 10}, {}, {}, {UseLargeWeapon}, {{ResistCharm, 100}}},
	{0, "Animal", {3, 3, 3, 1, 3, 3}, {25, 25, 25, 10, 10, 10}},
};
assert_enum(race, Animal)
BSDATAF(racei)