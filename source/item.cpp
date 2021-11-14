#include "main.h"

static enchantmenti magic_swords[] = {
	{Common},
	{Uncommon, 0, {{}, 1}},
	{Rare, 0, {{}, 2}},
	{Rare, "luck+1", {BonusSave, 1}},
	{Rare, "sharpness+1", {OfSharpness, 1}},
	{Rare, "accuracy+1", {OfAccuracy, 1}},
	{Rare, "defence+1", {AC, 1}},
	{Rare, 0, {{}, 3}},
	{Rare, "fear+2", {MageFear, 2}},
	{Rare, "luck+2", {AC, 2}},
	{Rare, "fire+2", {Fire, 2}},
	{Rare, "frost+2", {Cold, 2}},
	{Rare, "strenght drain+2", {OfStrenghtDrain, 2}},
	{Rare, "holiness+2", {OfHolyness, 2}},
	{VeryRare, 0, {{}, 4}},
	{VeryRare, "fire brand+3", {BurningHands, 3}},
	{VeryRare, "speed+3", {Haste, 3}},
	{VeryRare, "vampirism+3", {OfVampirism, 3}},
	{VeryRare, "holiness+3", {OfHolyness, 3}},
	{Artifact, 0, {{}, 5}},
	{Artifact, "speed+4", {Haste, 4}},
	{Artifact, "holiness+4", {OfHolyness, 4}},
	{Artifact, "fire brand+4", {Fire, 4}},
	{Artifact, "vampirism+4", {OfVampirism, 4}},
	{Artifact, "holiness+5", {OfHolyness, 5}},
};
static enchantmenti magic_weapon[] = {
	{Common},
	{Uncommon, 0, {{}, 1}},
	{Rare, 0, {{}, 2}},
	{Rare, "luck+1", {BonusSave, 1}},
	{Rare, "sharpness+1", {OfSharpness, 1}},
	{Rare, "smashing+1", {OfSmashing, 1}},
	{Rare, "accuracy+1", {OfAccuracy, 1}},
	{Rare, 0, {{}, 3}},
	{Rare, "luck+2", {BonusSave, 2}},
	{Rare, "strenght drain+2", {OfStrenghtDrain, 2}},
	{Rare, "holiness+2", {OfHolyness, 2}},
	{VeryRare, 0, {{}, 4}},
	{VeryRare, "speed+3", {Haste, 3}},
	{VeryRare, "vampirism+3", {OfVampirism, 3}},
	{VeryRare, "holiness+3", {OfHolyness, 3}},
	{Artifact, 0, {{}, 5}},
};
static enchantmenti magic_bludgeon[] = {
	{Common},
	{Uncommon, 0, {{}, 1}},
	{Rare, 0, {{}, 2}},
	{Rare, "smashing+2", {OfSmashing, 2}},
	{Rare, 0, {{}, 3}},
	{Rare, "luck+2", {BonusSave, 2}},
	{Rare, "strenght drain+2", {OfStrenghtDrain, 2}},
	{Rare, "holyness+2", {OfHolyness, 2}},
	{VeryRare, 0, {{}, 4}},
	{Artifact, 0, {{}, 5}},
};
static enchantmenti magic_potions[] = {
	{Common, "healing", {CureLightWounds, 1}},
	{Common, "climbing", {FeatherFall, 1}},
	{Common, "six sense", {DetectMagic, 1}},
	{Common, "insight", {BonusExperience, 1}},
	{Uncommon, "extra-healing", {CureLightWounds, 1}},
	{Uncommon, "cure disease", {CureDisease, 1}},
	{Uncommon, "invisibility", {Invisibility, 1}},
	{Uncommon, "knowledge", {Identify, 1}},
	{Uncommon, "poison", {Poison, 1}},
	{Uncommon, "insight", {BonusExperience, 2}},
	{Rare, "insight", {BonusExperience, 3}},
	{Rare, "speed", {Haste, 1}},
	{VeryRare, "advice", {BonusExperience, 4}},
	{Artifact, "strenght", {Strenght, 1}},
	{Artifact, "dexterity", {Dexterity, 1}},
	{Artifact, "intellegence", {Intellegence, 1}},
	{Artifact, "god insight", {BonusExperience, 5}},
};
static enchantmenti magic_bracers[] = {
	{Common},
	{Uncommon, "bulgary+2", {OpenLocks, 2}},
	{Uncommon, "protection+1", {AC, 1}},
	{Uncommon, "bulgary+3", {OpenLocks, 3}},
	{Uncommon, "traps+3", {RemoveTraps, 3}},
	{Rare, "protection+2", {AC, 2}},
	{Rare, "orge strenght", {Strenght, 1}},
	{Rare, "archery", {Dexterity, 1}},
	{VeryRare, "protection+3", {AC, 3}},
	{VeryRare, "giant strenght", {Strenght, 2}},
	{VeryRare, "cloud giant strenght", {Strenght, 3}},
	{VeryRare, "cat's grace", {Dexterity, 2}},
	{Artifact, "protection+5", {AC, 5}},
	{Artifact, "titan strenght", {Strenght, 5}},
};
static enchantmenti magic_boots[] = {
	{Common},
	{Uncommon, "jumping", {ClimbWalls, 4}},
	{Rare, "elvenind", {MoveSilently, 5}},
	{VeryRare, "speed", {Haste, 1}},
	{Artifact, "elvenind", {MoveSilently, 5}},
};
static enchantmenti magic_amulets[] = {
	{Common},
	{Uncommon, "protection+1", {AC, 1}},
	{Uncommon, "magic sense", {DetectMagic, 1}},
	{Rare, "speed", {Haste, 1}},
	{Rare, "knowledge", {Identify, 1}},
	{Rare, "protection+2", {AC, 2}},
	{VeryRare, "health", {Constitution, 1}},
	{VeryRare, "protection+3", {AC, 3}},
	{Artifact, "protection+4", {AC, 4}},
};
static enchantmenti magic_rings[] = {
	{Common},
	{Uncommon, "protection+1", {AC, 1}},
	{Uncommon, "sustenance", {CureLightWounds, 1}},
	{Uncommon, "resist fire", {ResistFire, 2}},
	{Uncommon, "resist cold", {ResistCold, 2}},
	{Uncommon, "resist poison", {SaveVsPoison, 2}},
	{Uncommon, "luck+1", {BonusSave, 1}},
	{Uncommon, "feather falling", {ClimbWalls, 5}},
	{Uncommon, "protection+2", {AC, 2}},
	{Uncommon, "holyness", {Bless, 1}},
	{Rare, "insight", {BonusExperience, 1}},
	{Rare, "luck+2", {BonusSave, 2}},
	{Rare, "resist magic", {ResistMagic, 2}},
	{Rare, "wizardy I", {OfWizardy, 1}},
	{Rare, "protection+3", {AC, 3}},
	{Rare, "invisibility", {Invisibility, 1}},
	{Rare, "warrior", {Haste, 1}},
	{VeryRare, "advise", {BonusExperience, 2}},
	{VeryRare, "luck+3", {BonusSave, 3}},
	{VeryRare, "regeneration", {Regeneration, 1}},
	{VeryRare, "protection+4", {AC, 4}},
	{VeryRare, "wizardy II", {OfWizardy, 2}},
	{Artifact, "protection+5", {AC, 5}},
	{Artifact, "wizardy III", {OfWizardy, 3}},
};
static enchantmenti magic_shield[] = {
	{Common},
	{Uncommon, 0, {{}, 1}},
	{Uncommon, 0, {{}, 2}},
	{Rare, 0, {{}, 3}},
	{Rare, "resist magic", {ResistMagic, 3}},
	{VeryRare, 0, {{}, 4}},
	{Artifact, 0, {{}, 5}},
};
static enchantmenti magic_helmet[] = {
	{Common},
	{Uncommon, "resist+1", {ResistMagic, 1}},
	{Rare, "intellegence", {Intellegence, 2}},
	{Rare, "leadership", {Charisma, 3}},
	{Rare, "resist+2", {ResistMagic, 2}},
	{VeryRare, 0, {{}, 4}},
	{Artifact, 0, {{}, 5}},
};
static enchantmenti magic_armor[] = {
	{Common},
	{Uncommon, 0, {{}, 1}},
	{Rare, 0, {{}, 2}},
	{Rare, "red dragon", {ResistFireSpell, 2}},
	{Rare, "blue dragon", {ResistColdSpell, 2}},
	{VeryRare, 0, {{}, 4}},
	{Artifact, 0, {{}, 5}},
};
static enchantmenti magic_robe[] = {
	{Common},
	{Uncommon, "detoxication", {SaveVsPoison, 4}},
	{Uncommon, "resist fire", {ResistFire, 1}},
	{Uncommon, "resist cold", {ResistCold, 1}},
	{Uncommon, "protection+1", {BonusSave, 1}},
	{Rare, "protection+2", {BonusSave, 2}},
	{Rare, "protection+3", {BonusSave, 3}},
	{VeryRare, "protection+4", {BonusSave, 4}},
	{Artifact, "protection+5", {BonusSave, 5}},
};
static enchantmenti magic_wand[] = {
	{Common, "magic missile", {MagicMissile, 2}},
	{Common, "fire", {BurningHands, 2}},
	{Common, "detect magic", {DetectMagic, 3}},
	{Common, "sleep", {Sleep, 2}},
	{Common, "mending", {Mending, 1}},
	{Uncommon, "acid", {AcidArrow, 1}},
	{Rare, "paralize", {HoldPerson, 3}},
	{Rare, "fear", {Fear, 1}},
	{VeryRare, "knowledge", {Identify, 1}},
	{Artifact, "acid", {AcidArrow, 1}},
};
static enchantmenti magic_staff[] = {
	{Common},
	{Uncommon, 0, {{}, 1}},
	{Uncommon, "curing+1", {CureLightWounds, 1}},
	{Uncommon, "insight+1", {DetectMagic, 1}},
	{Rare, 0, {{}, 2}},
	{Rare, "ignitify+2", {BurningHands, 2}},
	{VeryRare, 0, {{}, 3}},
	{Artifact, 0, {{}, 4}},
	{Artifact, "corrosion+4", {AcidArrow, 4}},
	{Artifact, 0, {{}, 5}},
};
static enchantmenti wizard_scrolls[] = {
	{Common, 0, {BurningHands, 1}},
	{Common, 0, {DetectMagic, 1}},
	{Common, 0, {FeatherFall, 1}},
	{Common, 0, {MageArmor, 1}},
	{Common, 0, {MagicMissile, 1}},
	{Common, 0, {Mending, 1}},
	{Common, 0, {ReadLanguagesSpell, 1}},
	{Common, 0, {ShieldSpell, 1}},
	{Common, 0, {Sleep, 1}},
	{Uncommon, 0, {Blur, 1}},
	{Uncommon, 0, {Invisibility, 1}},
	{Uncommon, 0, {Knock, 1}},
	{Uncommon, 0, {ProduceFlame, 1}},
	{Rare, 0, {Haste, 1}},
};
static enchantmenti priest_scrolls[] = {
	{Common, 0, {DetectEvil, 1}},
	{Common, 0, {CureLightWounds, 1}},
	{Common, 0, {ProtectionFromEvil, 1}},
	{Common, 0, {PurifyFood, 1}},
	{Uncommon, 0, {FlameBlade, 1}},
	{Uncommon, 0, {Goodberry, 1}},
	{Uncommon, 0, {SlowPoison, 1}},
	{Uncommon, 0, {CreateFood, 1}},
	{Uncommon, 0, {CureBlindnessDeafness, 1}},
	{Rare, 0, {CureDisease, 1}},
	{Rare, 0, {NegativePlanProtection, 1}},
	{Rare, 0, {RemoveCurse, 1}},
};
static enchantmenti old_tome[] = {{Common},
	{Uncommon},
	{Rare},
	{VeryRare},
	{Artifact},
};

BSDATA(itemi) = {
	{"No item"},
	{"Battle axe", Uncommon, {7, 4, 1}, 5 * GP, 0, Weapons, RightHand, {UseLargeWeapon, UseMartialWeapon}, {UseInHand, Versatile, Deadly}, {OneAttack, Slashing, -7, {1, 8}, {1, 8}}, {}, magic_weapon},
	{"Axe", Common, {7, 4, 1}, 2 * GP, 0, Weapons, RightHand, {UseMartialWeapon}, {UseInHand, Deadly}, {OneAttack, Slashing, -4, {1, 6}, {1, 4}}, {}, magic_weapon},
	{"Club", Common, {76, 1}, 0, 0, Weapons, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -4, {1, 6}, {1, 4}}, {}, magic_bludgeon},
	{"Dagger", Common, {15, 3}, 1 * GP, 0, Weapons, RightHand, {}, {Quick, UseInHand}, {OneAttack, Pierce, -2, {1, 4}, {1, 3}}, {}, magic_weapon},
	{"Flail", Uncommon, {5, 2, 1}, 15 * GP, 0, Weapons, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -7, {1, 6, 1}, {1, 6}}, {}, magic_bludgeon},
	{"Halberd", Rare, {3, 5, 1}, 10 * GP, 0, Weapons, RightHand, {UseLargeWeapon, UseMartialWeapon}, {UseInHand, TwoHanded, Deadly, Quick}, {OneAttack, Slashing, -9, {1, 10}, {2, 6}}, {}, magic_weapon},
	{"Warhammer", Rare, {99, 10, 1}, 2 * GP, 0, Weapons, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -4, {1, 4, 1}, {1, 4}}, {}, magic_bludgeon},
	{"Mace", Uncommon, {4, 1, 1}, 8 * GP, 0, Weapons, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -6, {1, 6, 1}, {1, 6}}, {}, magic_bludgeon},
	{"Spear", Common, {6, 3, 1}, 1 * GP, 0, Weapons, RightHand, {UseLargeWeapon}, {UseInHand, Versatile}, {OneAttack, Pierce, -6, {1, 6}, {1, 8}}, {}, magic_weapon},
	{"Staff", Uncommon, {8, 3, 1}, 0, 0, Weapons, RightHand, {}, {TwoHanded, UseInHand}, {OneAttack, Bludgeon, -8, {1, 6}, {1, 4}}, {}, magic_staff},
	{"Bastard sword", VeryRare, {45, 0, 1}, 25 * GP, 0, Weapons, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile, UseInHand}, {OneAttack, Slashing, -6, {2, 4}, {2, 8}}, {}, magic_swords},
	{"Longsword", VeryRare, {1, 0, 1}, 15 * GP, 0, Weapons, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick, UseInHand}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_swords},
	{"Short sword", VeryRare, {2, 0, 1}, 10 * GP, 0, Weapons, RightHand, {UseTheifWeapon}, {Quick, UseInHand}, {OneAttack, Slashing, -3, {1, 6}, {1, 8}}, {}, magic_swords},
	{"Two-handed sword", Artifact, {42, 0, 1}, 50 * GP, 0, Weapons, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded, UseInHand}, {OneAttack, Slashing, -10, {1, 10}, {3, 6}}, {}, magic_swords},
	{"Bow", VeryRare, {10, 6, 1, Arrow}, 75 * GP, 0, Weapons, RightHand, {UseTheifWeapon}, {TwoHanded, Ranged, UseInHand}, {TwoAttacks, Pierce, -8, {1, 8}, {1, 8}}, {}, {}, Arrow},
	{"Sling", Common, {18, 4, 0, Stone}, 1 * GP, 0, Weapons, RightHand, {}, {Ranged, UseInHand}, {OneAttack, Bludgeon, -6, {1, 4}, {1, 4}}, {}, {}, Stone},

	{"Robe", Rare, {32, 8, 1}, 3 * GP, 0, Clothes, Body, {UseArcane}, {}, {}, {}, magic_robe},
	{"Leather armor", Common, {31, 8, 1}, 5 * GP, 0, Armors, Body, {UseLeatherArmor}, {}, {}, {2}, magic_armor},
	{"Studded leather armor", Uncommon, {31, 8, 1}, 20 * GP, 0, Armors, Body, {UseLeatherArmor}, {}, {}, {3}, magic_armor},
	{"Scale mail", VeryRare, {30, 9, 1}, 120 * GP, 0, Armors, Body, {UseMetalArmor}, {}, {}, {4, 2}, magic_armor},
	{"Chain mail", Rare, {29, 9, 1}, 75 * GP, 0, Armors, Body, {UseMetalArmor}, {}, {}, {5}, magic_armor},
	{"Banded mail", VeryRare, {28, 9, 1}, 200 * GP, 0, Armors, Body, {UseMetalArmor}, {}, {}, {7, 3}, magic_armor},
	{"Plate mail", Artifact, {26, 9, 1}, 500 * GP, 0, Armors, Body, {UseMetalArmor}, {}, {}, {8, 4}, magic_armor},

	{"Helm", Uncommon, {20, 6}, 2 * GP, 0, Armors, Head, {UseShield}, {}, {}, {0, 2}, magic_helmet},
	{"Shield", Rare, {23, 7, 1}, 7 * GP, 0, Armors, LeftHand, {UseShield}, {}, {}, {1, 2}, magic_shield},
	{"Boots", Common, {21, 9}, 1 * GP, 0, Clothes, Legs, {}, {}, {}, {0, 1}, magic_boots},

	{"Bracers", Rare, {25, 16}, 2 * GP, 0, Armors, Elbow, {}, {}, {}, {}, magic_bracers},
	{"Necklage", Rare, {33, 13}, 15 * GP, 1, {Jewelry}, Neck, {}, {}, {}, {}, magic_amulets},
	{"Necklage", VeryRare, {34, 13}, 50 * GP, 2, {Jewelry}, Neck, {}, {}, {}, {}, magic_amulets},
	{"Jewelry", Artifact, {108, 13}, 100 * GP, 3, {Jewelry}, Neck, {}, {}, {}, {}, magic_amulets},

	{"Arrow", Common, {16, 5}, 1 * GP, 0, Weapons, Quiver, {}, {Countable}},
	{"Dart", Common, {14, 0}, 1 * GP, 0, Weapons, RightHand, {}, {Countable}},
	{"Stone", Common, {19, 2}, 0, 0, Weapons, Quiver, {}, {Countable}},

	{"Bones", Common, {43, 7}, 0, 0, Tools, {}, {}, {}},
	{"Map", Common, {86, 12}, 0, 0, Papers, {}, {}, {}},

	{"Holy Symbol", Rare, {53, 20}, 0, 0, Tools, {}, {UseDivine}, {UseInHand}},
	{"Holy Symbol", Rare, {27, 20}, 0, 0, Tools, {}, {UseDivine, UseMartialWeapon}, {UseInHand}},
	{"Spell book", VeryRare, {35, 11}, 0, 0, Books, {}, {UseArcane}, {UseInHand}},
	{"Lockpicks", Rare, {54, 1}, 5 * GP, 0, Tools, Backpack, {UseTheif}, {Expandable}},

	{"Wand", Rare, {52, 10}, 100 * GP, 0, Devices, {}, {UseArcane}, {UseInHand, Charged}, {}, {}, magic_wand},
	{"Scroll", Uncommon, {36, 12}, 40 * GP, 0, Papers, {}, {UseScrolls, UseArcane}, {}, {}, {}, wizard_scrolls},
	{"Scroll", Uncommon, {85, 12}, 25 * GP, 0, Papers, {}, {UseScrolls, UseDivine}, {}, {}, {}, priest_scrolls},

	{"Shelf key", Artifact, {46, 8}, 0, 0, Tools},
	{"Silver key", Artifact, {47, 8}, 0, 0, Tools},
	{"Cooper key", Artifact, {48, 8}, 0, 0, Tools},
	{"Skull key", Artifact, {87, 8}, 0, 0, Tools},
	{"Spider key", Artifact, {62, 8}, 0, 0, Tools},
	{"Moon key", Artifact, {88, 8}, 0, 0, Tools},
	{"Diamond key", Artifact, {102, 8}, 0, 0, Tools},
	{"Green key", Artifact, {50, 8}, 0, 0, Tools},

	{"Ring", Rare, {55, 15}, 5 * GP, 1, Jewelry, RightRing, {UseArcane}, {}, {}, {}, magic_rings},
	{"Ring", Rare, {78, 15}, 30 * GP, 1, Jewelry, RightRing, {}, {}, {}, {}, magic_rings},
	{"Ring", VeryRare, {79, 15}, 70 * GP, 1, Jewelry, RightRing, {}, {}, {}, {}, magic_rings},

	{"Potion", Rare, {39, 19}, 50 * GP, 0, Potions, Backpack, {}, {}, {}, {}, magic_potions},
	{"Potion", Rare, {40, 19}, 60 * GP, 0, Potions, Backpack, {}, {Expandable}, {}, {}, magic_potions},
	{"Potion", Rare, {41, 19}, 70 * GP, 0, Potions, Backpack, {}, {}, {}, {}, magic_potions},

	{"Red gem", VeryRare, {93, 22}, 200 * GP, 5, Jewelry},
	{"Blue gem", VeryRare, {94, 22}, 400 * GP, 6, Jewelry},
	{"Green gem", VeryRare, {95, 22}, 500 * GP, 8, Jewelry},
	{"Purple gem", VeryRare, {96, 22}, 1000 * GP, 10, Jewelry},

	{"Ration", Common, {38, 14}, 1 * GP, 0, Food, Backpack, {}, {Expandable}},
	{"Iron ration", Uncommon, {37, 14}, 2 * GP, 0, Food, Backpack, {}, {Expandable}},

	{"Dust of Ages", Artifact, {97, 25}, 0, 0, Tools, {}, {UseArcane}, {Unique}},
	{"Horn", Artifact, {59, 23}, 0, 0, Tools, {}, {}, {Unique, Famed}},
	{"Mantist Head", Artifact, {51, 18}, 0, 0, Tools, {}, {}, {Unique}},
	{"Scepeter", Artifact, {66, 17}, 0, 0, Weapons, RightHand, {}, {Unique, Famed}, {OneAttack, Bludgeon, -5, {1, 6}, {1, 6}}, {}, magic_weapon},
	{"Silver sword", Artifact, {65, 0}, 0, 0, Weapons, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick, Unique, Famed, SevereDamageUndead}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_weapon},
	{"Stone amulet", Artifact, {64, 13}, 0, 0, Tools, Neck, {}, {Unique}},
	{"Stone dagger", Artifact, {60, 3}, 0, 0, Weapons, {}, {}, {Unique}},
	{"Stone gem", Artifact, {57, 22}, 0, 0, Jewelry, {}, {}, {Unique}},
	{"Stone holy symbol", Artifact, {63, 20}, 0, 0, Tools, {}, {UseDivine}, {Unique}},
	{"Stone orb", Artifact, {61, 18}, 0, 0, Devices, {}, {}, {Unique}},

	{"Slam", Common, {}, 0, 0, Weapons, RightHand, {}, {Natural}, {OneAttack, Bludgeon, -2, {1, 8}, {}}},
	{"Slam", Common, {}, 0, 0, Weapons, RightHand, {}, {Natural}, {OneAttack, Bludgeon, -2, {1, 4}, {}}},
	{"Claws", Common, {}, 0, 0, Weapons, RightHand, {}, {Natural}, {TwoAttacks, Slashing, -3, {1, 4}, {}}},
	{"Bite", Common, {}, 0, 0, Weapons, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 6}, {}}},
	{"Bite", Common, {}, 0, 0, Weapons, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 4, 1}, {}}},
	{"Bite", Common, {}, 0, 0, Weapons, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {2, 6}, {}}},
	{"Shoking hang", Common, {80}, 0, 0, Weapons, RightHand, {}, {Natural, Charged}, {OneAttack, Electricity, -4, {1, 8}, {0, 0, 1}}},
	{"Flame blade", Common, {82}, 0, 0, Weapons, RightHand, {}, {Natural, Charged, SevereDamageUndead}, {OneAttack, Fire, -5, {1, 4, 4}, {}}},
	{"Flame hand", Common, {81}, 0, 0, Weapons, RightHand, {}, {Natural, Charged, Ranged}, {OneAttack, Fire, -1, {1, 4, 1}, {}}},
};
assert_enum(item, LastItem)
INSTELEM(itemi);
static_assert(sizeof(item) == 4, "Not valid items count");

static unsigned char find_power(const aref<enchantmenti>& source, variant v) {
	for(auto& e : source) {
		if(e.power == v)
			return source.indexof(&e);
	}
	return 0xFF;
}

static unsigned char find_power(const aref<enchantmenti>& source, variant v, int magic) {
	for(auto& e : source) {
		if(e.power == v && e.power.bonus == magic)
			return source.indexof(&e);
	}
	return 0xFF;
}

rarity_s item::getrandomrarity(int level) {
	static int source[][Artifact] = {
		{},
		{75, 95, 00, 00}, // level 1
		{70, 90, 00, 00},
		{65, 85, 99, 00},
		{60, 80, 97, 00},
		{55, 75, 95, 99}, // level 5
		{50, 70, 92, 98},
		{45, 65, 90, 98},
		{40, 60, 88, 97},
		{35, 55, 86, 96},
		{30, 50, 84, 95}, // level 10
		{25, 46, 82, 94},
		{21, 42, 80, 93},
		{18, 38, 78, 92},
		{16, 34, 76, 91},
		{14, 30, 74, 90}, // Level 15
	};
	int mod_level = imin((unsigned)level, sizeof(source) / sizeof(source[0]));
	int result[Artifact];
	memcpy(result, source[mod_level], sizeof(result));
	auto r = d100();
	for(auto i = 0; i < Artifact; i++) {
		if(result[i] == 0 || r <= result[i])
			return (rarity_s)i;
	}
	return Artifact;
}

item::item(item_s type, variant power) {
	clear();
	this->type = type;
	setpower(power);
}

item::item(item_s type, variant power, int magic) {
	clear();
	this->type = type;
	setpower(power, magic);
}

item::item(item_s type, rarity_s power) {
	clear();
	this->type = type;
	setpower(power);
}

void item::finish() {
	if(is(Charged))
		setcharges(dice::roll(3, 6));
	else if(is(Countable))
		setcount(dice::roll(3, 4));
}

void item::setpower(variant power) {
	auto i = find_power(bsdata<itemi>::elements[type].enchantments, power);
	if(i == 0xFF)
		return;
	subtype = i;
	finish();
}

void item::setpower(variant power, int magic) {
	auto i = find_power(bsdata<itemi>::elements[type].enchantments, power, magic);
	if(i == 0xFF)
		return;
	subtype = i;
	finish();
}

static unsigned char getpowerindex(item_s type, rarity_s rarity) {
	auto& ei = bsdata<itemi>::elements[type];
	adat<unsigned char, 256> source;
	for(auto& e : ei.enchantments) {
		if(e.rarity == rarity)
			source.add(ei.enchantments.indexof(&e));
	}
	if(!source)
		return 0;
	return source.data[rand() % source.count];
}

item& item::setpower(rarity_s rarity) {
	subtype = getpowerindex(type, rarity);
	finish();
	return *this;
}

void item::clear() {
	memset(this, 0, sizeof(*this));
}

bool item::isartifact() const {
	auto pe = getenchantment();
	return pe ? (pe->rarity == Artifact) : false;
}

bool item::ismagical() const {
	auto pe = getenchantment();
	if(!pe)
		return false;
	return pe->power.bonus > 0;
}

int item::getportrait() const {
	return bsdata<itemi>::elements[type].image.avatar;
}

int item::get(enchant_s v) const {
	auto pe = getenchantment();
	if(!pe)
		return 0;
	if(pe->power.type == Enchant && pe->power.value == v) {
		if(iscursed())
			return -pe->power.bonus;
		return pe->power.bonus;
	} else
		return 0;
}

void item::get(combati& result, const creature* enemy) const {
	auto& wi = bsdata<itemi>::elements[type].weapon;
	auto size = enemy ? enemy->getsize() : Medium;
	if(size == Large && !is(Natural))
		result.damage = wi.damage_large;
	else
		result.damage = wi.damage;
	auto magic_bonus = getmagic();
	result.type = wi.type;
	result.attack = wi.attack;
	result.bonus += wi.bonus + magic_bonus + getenchant(OfAccuracy);
	result.damage.b += magic_bonus;
	if(is(Deadly))
		result.critical_multiplier++;
	if(is(Quick))
		result.critical_range++;
	if(enemy && enemy->is(Undead)) {
		auto holyness = getenchant(OfHolyness);
		result.bonus += holyness;
		result.damage.b += holyness * 2;
		if(is(SevereDamageUndead))
			result.damage.b += 2;
	}
	result.critical_range += get(OfSharpness);
	result.critical_multiplier += get(OfSmashing);
}

void item::getname(stringbuilder& sc) const {
	if(isbroken()) {
		if(type == RationIron || type == Ration)
			sc.adds("rotten");
		else
			sc.adds("damaged");
	}
	if(isidentified() && iscursed())
		sc.adds("cursed");
	sc.adds(bsdata<itemi>::elements[type].name);
	if(isidentified()) {
		auto pe = getenchantment();
		if(pe) {
			if(pe->name)
				sc.adds("of %1", pe->name);
			else if(pe->power)
				sc.adds("of %1", pe->power.getname());
			else {
				auto magic = getmagic();
				if(magic)
					sc.add("%+1i", magic);
			}
		}
	}
}

bool item::ismelee() const {
	return (type >= AxeBattle && type <= SwordTwoHanded)
		|| (type >= Slam && type <= LastItem);
}

int	item::getmagic() const {
	auto pe = getenchantment();
	if(!pe)
		return 0;
	if(iscursed())
		return -pe->power.bonus;
	return pe->power.bonus;
}

void item::setcharges(int value) {
	if(value <= 0)
		clear();
	else
		charges = value;
}

bool item::damage(const char* text_damage, const char* text_broke) {
	char name[128];
	if(broken) {
		// Not all items can be broken
		if(is(Natural) || is(Unique) || isartifact())
			return false;
		if(ismagical() && (d100() < 50))
			return false;
		if(text_broke) {
			stringbuilder sb(name); getname(sb);
			mslog(text_broke, name);
		}
		clear();
		return true;
	} else {
		if(text_damage) {
			stringbuilder sb(name); getname(sb);
			mslog(text_damage, name);
		}
		if(is(Countable)) {
			use();
			return !(*this);
		}
		broken = 1;
	}
	return false;
}

int item::getarmorpenalty(ability_s skill) const {
	if(is(UseMetalArmor)) {
		switch(skill) {
		case OpenLocks: return 20;
		case RemoveTraps: return 30;
		case ClimbWalls: return 60;
		case MoveSilently: return 60;
		}
	}
	return 0;
}

bonusi item::getpower() const {
	auto& ei = bsdata<itemi>::elements[type];
	if(!ei.enchantments.count)
		return bonusi();
	return ei.enchantments.data[subtype].power;
}

const enchantmenti* item::getenchantment() const {
	auto& ei = bsdata<itemi>::elements[type].enchantments;
	if(!ei)
		return 0;
	return ei.data + subtype;
}

int	item::getenchant(enchant_s v) const {
	auto pe = getenchantment();
	if(pe && pe->power.type == Enchant && pe->power.value == v)
		return iscursed() ? -3 : 3;
	return 0;
}

int	item::getcost() const {
	auto r = bsdata<itemi>::elements[type].cost;
	if(!r)
		return 0;
	if(isidentified()) {
		if(iscursed())
			return 0;
		if(getpower())
			r += 1;
	}
	return r;
}

int item::getcount() const {
	if(is(Countable))
		return charges + 1;
	return 1;
}

void item::setcount(int v) {
	if(!v)
		clear();
	else if(is(Countable))
		charges = v - 1;
}

bool item::stack(item& v) {
	unsigned char maximum_count = 98;
	if(!is(Countable) || type != v.type || subtype != v.subtype || flags != v.flags)
		return false;
	int c1 = charges;
	int c2 = v.charges;
	auto result = false;
	c1 += c2 + 1;
	if(c1 <= maximum_count) {
		charges = c1;
		v.clear();
		result = true;
	} else {
		charges = maximum_count;
		v.charges = c1 - maximum_count - 1;
	}
	return result;
}

creature* item::getowner() const {
	if(!this)
		return 0;
	if((void*)this >= location.monsters
		&& (void*)this < location.monsters + sizeof(location.monsters) / sizeof(location.monsters[0]))
		return location.monsters + ((creature*)this - location.monsters);
	if(bsdata<creature>::source.indexof(this) != -1)
		return bsdata<creature>::elements + bsdata<creature>::source.indexof(this);
	if((void*)this >= bsdata<creature>::elements
		&& (void*)this < bsdata<creature>::source.end())
		return bsdata<creature>::elements + ((creature*)this - bsdata<creature>::elements);
	return 0;
}

size_s itemi::getsize() const {
	return this->image.size ? Large : Small;
}

bool item::ispower(variant v) const {
	if(!type)
		return false;
	auto p = getenchantment();
	if(!p)
		return false;
	return p->power == v;
}

bool item::issmall() const {
	return gete().image.size == 0;
}

wear_s item::getequiped() const {
	auto p = getowner();
	if(p)
		return p->getslot(this);
	return Backpack;
}

void item::select(adat<item>& result, good_s good, rarity_s rarity) {
	for(auto& ei : bsdata<itemi>()) {
		if(!ei.enchantments)
			continue;
		if(ei.goods != good)
			continue;
		auto type = (item_s)(&ei - bsdata<itemi>::elements);
		for(auto& ep : ei.enchantments) {
			if(ep.rarity != rarity)
				continue;
			auto pi = result.add();
			pi->clear();
			pi->type = type;
			pi->subtype = &ep - ei.enchantments.data;
			pi->finish();
		}
	}
}