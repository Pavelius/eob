#include "main.h"

// ClimbWalls, HearNoise, MoveSilently, OpenLocks, RemoveTraps, ReadLanguages,

race_info bsmeta<race_info>::elements[] = {{"No race"},
{"Dwarf", {8, 3, 11, 3, 3, 3}, {18, 17, 18, 18, 18, 17}, {0, 0, 1, 0, 0, -1}, {BonusSaveVsPoison, BonusSaveVsSpells, BonusToHitVsGoblinoid, BonusACVsLargeEnemy}, {UseLargeWeapon}},
{"Elf", {3, 6, 7, 8, 3, 8}, {18, 18, 18, 18, 18, 18}, {0, 1, -1, 0, 0, 0}, {}, {UseLargeWeapon}, {{ResistCharm, 90}, {DetectSecrets, 15}}},
{"Half-elf", {3, 6, 6, 4, 3, 3}, {18, 18, 18, 18, 18, 18}, {0, 0, 0, 0, 0, 0}, {}, {UseLargeWeapon}, {{ResistCharm, 30}, {DetectSecrets, 15}}},
{"Halfling", {7, 7, 10, 6, 3, 3}, {18, 18, 18, 18, 17, 18}, {-1, 1, 0, 0, 0, 0}, {BonusSaveVsPoison, BonusSaveVsSpells}},
{"Human", {3, 3, 3, 3, 3, 3}, {18, 18, 18, 18, 18, 18}, {0, 0, 0, 0, 0, 0}, {}, {UseLargeWeapon}},
//
{"Humanoid", {3, 3, 3, 3, 3, 3}, {25, 25, 25, 25, 25, 25}, {}, {}, {UseLargeWeapon}},
{"Goblinoid", {3, 3, 3, 3, 3, 3}, {18, 18, 18, 18, 18, 18}, {}, {}, {UseLargeWeapon}},
{"Insectoid", {3, 3, 3, 1, 3, 3}, {25, 25, 25, 10, 10, 10}, {}, {}, {}, {{ResistCharm, 100}}},
{"Animal", {3, 3, 3, 1, 3, 3}, {25, 25, 25, 10, 10, 10}},
};
assert_enum(race, Animal);