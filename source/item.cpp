#include "main.h"

static enchantmenti magic_swords[] = {{Common},
{Uncommon, 0, {}, 1},
{Rare, 0, {}, 2},
{Rare, "luck", OfLuck, 1},
{Rare, "sharpness", OfSharpness, 1},
{Rare, "accuracy", OfAccuracy, 1},
{Rare, "defence", OfProtection, 1},
{Rare, 0, {}, 3},
{Rare, "luck", OfLuck, 2},
{Rare, "fire", Fire, 2},
{Rare, "frost", Cold, 2},
{Rare, "strenght drain", OfStrenghtDrain, 2},
{Rare, "holiness", OfHolyness, 2},
{VeryRare, 0, {}, 4},
{VeryRare, "fire brand", BurningHands, 3},
{VeryRare, "speed", Haste, 3},
{VeryRare, "vampirism", OfVampirism, 3},
{VeryRare, "holiness", OfHolyness, 3},
{Artifact, 0, {}, 5},
{Artifact, "speed", Haste, 4},
{Artifact, "holiness", OfHolyness, 4},
{Artifact, "fire brand", Fire, 4},
{Artifact, "vampirism", OfVampirism, 4},
{Artifact, "holiness", OfHolyness, 5},
};
static enchantmenti magic_weapon[] = {{Common},
{Uncommon, 0, {}, 1},
{Rare, 0, {}, 2},
{Rare, "luck", OfLuck, 1},
{Rare, "sharpness", OfSharpness, 1},
{Rare, "smashing", OfSmashing, 1},
{Rare, "accuracy", OfAccuracy, 1},
{Rare, 0, {}, 3},
{Rare, "luck", OfLuck, 2},
{Rare, "strenght drain", OfStrenghtDrain, 2},
{Rare, "holiness", OfHolyness, 2},
{VeryRare, 0, {}, 4},
{VeryRare, "speed", Haste, 3},
{VeryRare, "vampirism", OfVampirism, 3},
{VeryRare, "holiness", OfHolyness, 3},
{Artifact, 0, {}, 5},
};
static enchantmenti magic_bludgeon[] = {{Common},
{Uncommon, 0, {}, 1},
{Rare, 0, {}, 2},
{Rare, "smashing", OfSmashing, 2},
{Rare, 0, {}, 3},
{Rare, "luck", OfLuck, 2},
{Rare, "strenght drain", OfStrenghtDrain, 2},
{Rare, "holyness", OfHolyness, 2},
{VeryRare, 0, {}, 4},
{Artifact, 0, {}, 5},
};
static enchantmenti magic_potions[] = {{Common, "healing", CureLightWounds, 1},
{Common, "climbing", FeatherFall, 1},
{Common, "six sense", DetectMagic, 1},
{Uncommon, "extra-healing", CureLightWounds, 2},
{Uncommon, "cure disease", CureDisease, 2},
{Uncommon, "invisibility", Invisibility, 2},
{Uncommon, "knowledge", Identify, 2},
{Uncommon, "poison", Poison, 2},
{Rare, "speed", Haste, 3},
{VeryRare, "advice", OfAdvise, 4},
{Artifact, "strenght", Strenght, 5},
{Artifact, "dexterity", Dexterity, 5},
{Artifact, "intellegence", Intellegence, 5},
{Artifact, "god insight", OfAdvise, 5},
};
static enchantmenti magic_bracers[] = {{Common},
{Common, "bulgary+1", OpenLocks, 1},
{Uncommon, "sustenance", CureLightWounds, 1},
{Uncommon, "protection+1", OfProtection, 1},
{Uncommon, "bulgery+2", OpenLocks, 2},
{Rare, "protection+2", OfProtection, 2},
{Rare, "orge strenght", Strenght, 1},
{Rare, "archery", Dexterity, 1},
{VeryRare, "protection+3", OfProtection, 3},
{VeryRare, "giant strenght", Strenght, 2},
{VeryRare, "cloud giant strenght", Strenght, 3},
{VeryRare, "cat's grace", Dexterity, 2},
{Artifact, "protection+5", OfProtection, 5},
{Artifact, "titan strenght", Strenght, 5},
};
static enchantmenti magic_boots[] = {{Common},
{Uncommon, "jumping", ClimbWalls, 4},
{Rare, "elvenind", MoveSilently, 5},
{VeryRare, "speed", Haste},
{Artifact, "elvenind", MoveSilently, 5},
};
static enchantmenti magic_amulets[] = {{Common},
{Uncommon, "protection+1", OfProtection, 1},
{Uncommon, "magic sense", DetectMagic, 1},
{Rare, "speed", Haste},
{Rare, "knowledge", Identify},
{Rare, "protection+2", OfProtection, 2},
{VeryRare, "health", Constitution},
{VeryRare, "protection+3", OfProtection, 3},
{Artifact, "protection+4", OfProtection, 4},
};
static enchantmenti magic_rings[] = {{Common},
{Uncommon, "protection+1", OfProtection, 1},
{Uncommon, "resist fire", ResistFire, 5},
{Uncommon, "resist cold", ResistCold, 5},
{Uncommon, "resist poison", SaveVsPoison, 2},
{Uncommon, "luck", OfLuck, 1},
{Uncommon, "feather falling", ClimbWalls, 5},
{Rare, "advise", OfAdvise, 1},
{Rare, "resist magic", ResistMagic, 2},
{Rare, "wizardy I", OfWizardy, 1},
{Rare, "protection+2", OfProtection, 2},
{Rare, "invisibility", Invisibility, 1},
{Rare, "warrior", Haste, 1},
{VeryRare, "holyness", Bless, 1},
{VeryRare, "regeneration", OfRegeneration, 1},
{VeryRare, "protection+3", OfProtection, 3},
{VeryRare, "wizardy II", OfWizardy, 2},
{Artifact, "protection+4", OfProtection, 4},
{Artifact, "wizardy III", OfWizardy, 3},
};
static enchantmenti magic_shield[] = {{Common},
{Uncommon, 0, {}, 1},
{Uncommon, 0, {}, 2},
{Rare, "resist magic", {}, 3},
{Rare, 0, {}, 3},
{VeryRare, 0, {}, 4},
{Artifact, 0, {}, 5},
};
static enchantmenti magic_helmet[] = {{Common},
{Uncommon, 0, {}, 1},
{Uncommon, 0, {}, 2},
{Rare, 0, {}, 3},
{Rare, "resist magic", {}, 2},
{Rare, "intellegence", {}, 2},
{VeryRare, 0, {}, 4},
{Artifact, 0, {}, 5},
};
static enchantmenti magic_armor[] = {{Common},
{Uncommon, 0, {}, 1},
{Uncommon, 0, {}, 2},
{Rare, 0, {}, 3},
{Rare, "red dragon", ResistFireSpell, 2},
{Rare, "blue dragon", ResistColdSpell, 2},
{VeryRare, 0, {}, 4},
{Artifact, 0, {}, 5},
};
static enchantmenti magic_robe[] = {{Common},
{Uncommon, "protection+1", {}, 1},
{Rare, "protection+2", {}, 2},
{Rare, "protection+3", {}, 3},
{VeryRare, "protection+4", {}, 4},
{Artifact, "protection+5", {}, 5},
};
static enchantmenti magic_wand[] = {{Common, "magic missile", MagicMissile},
{Common, "fire", BurningHands},
{Common, "detect magic", DetectMagic},
{Common, "sleep", Sleep},
{Common, "mending", Mending},
{Uncommon, "acid", AcidArrow},
{Rare, "acid", AcidArrow},
{VeryRare, "acid", AcidArrow},
{Artifact, "acid", AcidArrow},
};
static enchantmenti magic_staff[] = {{Common},
{Uncommon, 0, {}, 1},
{Uncommon, "curing", CureLightWounds, 1},
{Uncommon, "insight", DetectMagic, 1},
{Rare, 0, {}, 2},
{Rare, "ignitify", BurningHands, 2},
{VeryRare, 0, {}, 3},
{Artifact, 0, {}, 4},
{Artifact, "corrosion", AcidArrow, 4},
{Artifact, 0, {}, 5},
};
static enchantmenti wizard_scrolls[] = {{Common, 0, BurningHands},
{Common, 0, DetectMagic},
{Common, 0, FeatherFall},
{Common, 0, MageArmor},
{Common, 0, MagicMissile},
{Common, 0, Mending},
{Common, 0, ReadLanguagesSpell},
{Common, 0, ShieldSpell},
{Common, 0, Sleep},
{Uncommon, 0, Blur},
{Uncommon, 0, Invisibility},
{Uncommon, 0, Knock},
{Uncommon, 0, ProduceFlame},
{Rare, 0, Haste},
};
static enchantmenti priest_scrolls[] = {{Common, 0, DetectEvil},
{Common, 0, CureLightWounds},
{Common, 0, ProtectionFromEvil},
{Common, 0, PurifyFood},
{Uncommon, 0, FlameBlade},
{Uncommon, 0, Goodberry},
{Uncommon, 0, SlowPoison},
{Uncommon, 0, CreateFood},
{Uncommon, 0, CureBlindnessDeafness},
{Rare, 0, CureDisease},
{Rare, 0, NegativePlanProtection},
{Rare, 0, RemoveCurse},
};
static enchantmenti old_tome[] = {{Common},
{Uncommon},
{Rare},
{VeryRare},
{Artifact},
};

BSDATA(itemi) = {{"No item"},
{"Battle axe", {7, 4, 1}, 0, RightHand, {UseLargeWeapon, UseMartialWeapon}, {UseInHand, Versatile, Deadly}, {OneAttack, Slashing, -7, {1, 8}, {1, 8}}, {}, magic_weapon},
{"Axe", {7, 4, 1}, 0, RightHand, {UseMartialWeapon}, {UseInHand, Deadly}, {OneAttack, Slashing, -4, {1, 6}, {1, 4}}, {}, magic_weapon},
{"Club", {76, 1}, 0, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -4, {1, 6}, {1, 4}}, {}, magic_bludgeon},
{"Dagger", {15, 3}, 0, RightHand, {}, {Quick, UseInHand}, {OneAttack, Pierce, -2, {1, 4}, {1, 3}}, {}, magic_weapon},
{"Flail", {5, 2, 1}, 0, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -7, {1, 6, 1}, {1, 6}}, {}, magic_bludgeon},
{"Halberd", {3, 5, 1}, 0, RightHand, {UseLargeWeapon, UseMartialWeapon}, {UseInHand, TwoHanded, Deadly, Quick}, {OneAttack, Slashing, -9, {1, 10}, {2, 6}}, {}, magic_weapon},
{"Warhammer", {99, 10, 1}, 0, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -4, {1, 4, 1}, {1, 4}}, {}, magic_bludgeon},
{"Mace", {4, 1, 1}, 0, RightHand, {}, {UseInHand}, {OneAttack, Bludgeon, -6, {1, 6, 1}, {1, 6}}, {}, magic_bludgeon},
{"Spear", {6, 3, 1}, 0, RightHand, {UseLargeWeapon}, {UseInHand, Versatile}, {OneAttack, Pierce, -6, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Staff", {8, 3, 1}, 0, RightHand, {}, {TwoHanded, UseInHand}, {OneAttack, Bludgeon, -8, {1, 6}, {1, 4}}, {}, magic_staff},
{"Bastard sword", {45, 0, 1}, 0, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile, UseInHand}, {OneAttack, Slashing, -6, {2, 4}, {2, 8}}, {}, magic_swords},
{"Longsword", {1, 0, 1}, 0, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick, UseInHand}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_swords},
{"Short sword", {2, 0, 1}, 0, RightHand, {UseTheifWeapon}, {Quick, UseInHand}, {OneAttack, Slashing, -3, {1, 6}, {1, 8}}, {}, magic_swords},
{"Two-handed sword", {42, 0, 1}, 0, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded, UseInHand}, {OneAttack, Slashing, -10, {1, 10}, {3, 6}}, {}, magic_swords},

{"Bow", {10, 6, 1, Arrow}, 0, RightHand, {UseTheifWeapon}, {TwoHanded, Ranged, UseInHand}, {TwoAttacks, Pierce, -8, {1, 8}, {1, 8}}, {}, {}, Arrow},
{"Sling", {18, 4, 0, Stone}, 0, RightHand, {}, {Ranged, UseInHand}, {OneAttack, Bludgeon, -6, {1, 4}, {1, 4}}, {}, {}, Stone},

{"Robe", {32, 8, 1}, 0, Body, {UseArcane}, {}, {}, {}, magic_robe},
{"Leather armor", {31, 8, 1}, 0, Body, {UseLeatherArmor}, {}, {}, {2}, magic_armor},
{"Studded leather armor", {31, 8, 1}, 0, Body, {UseLeatherArmor}, {}, {}, {3}, magic_armor},
{"Scale mail", {30, 9, 1}, 0, Body, {UseMetalArmor}, {}, {}, {4, 2}, magic_armor},
{"Chain mail", {29, 9, 1}, 0, Body, {UseMetalArmor}, {}, {}, {5}, magic_armor},
{"Banded mail", {28, 9, 1}, 0, Body, {UseMetalArmor}, {}, {}, {7, 3}, magic_armor},
{"Plate mail", {26, 9, 1}, 0, Body, {UseMetalArmor}, {}, {}, {8, 4}, magic_armor},

{"Helm", {20, 6}, 0, Head, {UseShield}, {}, {}, {0, 2}, magic_helmet},
{"Shield", {23, 7, 1}, 0, LeftHand, {UseShield}, {}, {}, {1, 2}, magic_shield},
{"Boots", {21, 9}, 0, Legs, {}, {}, {}, {0, 1}, magic_boots},

{"Bracers", {25, 16}, 0, Elbow, {}, {}, {}, {}, magic_bracers},
{"Necklage", {33, 13}, 1, Neck, {}, {}, {}, {}, magic_amulets},
{"Necklage", {34, 13}, 2, Neck, {}, {}, {}, {}, magic_amulets},
{"Jewelry", {108, 13}, 3, Neck, {}, {}, {}, {}, magic_amulets},

{"Arrow", {16, 5}, 0, Quiver, {}, {Countable}},
{"Dart", {14, 0}, 0, RightHand, {}, {Countable}},
{"Stone", {19, 2}, 0, Quiver, {}, {Countable}},

{"Bones", {43, 7}, 0, {}, {}, {}},
{"Map", {86, 12}, 0, {}, {}, {}},

{"Holy Symbol", {53, 20}, 0, {}, {UseDivine}, {UseInHand}},
{"Holy Symbol", {27, 20}, 0, {}, {UseDivine, UseMartialWeapon}, {UseInHand}},
{"Spell book", {35, 11}, 0, {}, {UseArcane}, {UseInHand}},
{"Lockpicks", {54, 1}, 0, {}, {UseTheif}},

{"Wand", {52, 10}, 0, {}, {UseArcane}, {UseInHand, Charged}, {}, {}, magic_wand},
{"Scroll", {36, 12}, 0, {}, {UseScrolls, UseArcane}, {}, {}, {}, wizard_scrolls},
{"Scroll", {85, 12}, 0, {}, {UseScrolls, UseDivine}, {}, {}, {}, priest_scrolls},

{"Shelf key", {46, 8}},
{"Silver key", {47, 8}},
{"Cooper key", {48, 8}},
{"Skull key", {87, 8}},
{"Spider key", {62, 8}},
{"Moon key", {88, 8}},
{"Diamond key", {102, 8}},
{"Green key", {50, 8}},

{"Ring", {55, 15}, 1, RightRing, {UseArcane}, {}, {}, {}, magic_rings},
{"Ring", {78, 15}, 1, RightRing, {}, {}, {}, {}, magic_rings},
{"Ring", {79, 15}, 1, RightRing, {}, {}, {}, {}, magic_rings},

{"Potion", {39, 19}, 0, {}, {}, {}, {}, {}, magic_potions},
{"Potion", {40, 19}, 0, {}, {}, {}, {}, {}, magic_potions},
{"Potion", {41, 19}, 0, {}, {}, {}, {}, {}, magic_potions},

{"Red gem", {93, 22}, 5},
{"Blue gem", {94, 22}, 6},
{"Green gem", {95, 22}, 8},
{"Purple gem", {96, 22}, 10},

{"Ration", {38, 14}},
{"Iron ration", {37, 14}},

{"Dust of Ages", {97, 25}, 0, {}, {UseArcane}, {Unique}},
{"Horn", {59, 23}, 0, {}, {}, {Unique}},
{"Mantist Head", {51, 18}, 0, {}, {}, {Unique}},
{"Scepeter", {66, 17}, 0, RightHand, {}, {Unique}, {OneAttack, Bludgeon, -5, {1, 6}, {1, 6}}, {}, magic_weapon},
{"Silver sword", {65, 0}, 0, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick, Unique, SevereDamageUndead}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_weapon},
{"Stone amulet", {64, 13}, 0, Neck, {}, {Unique}},
{"Stone dagger", {60, 3}, 0, {}, {}, {Unique}},
{"Stone gem", {57, 22}, 0, {}, {}, {Unique}},
{"Stone holy symbol", {63, 20}, 0, {}, {UseDivine}, {Unique}},
{"Stone orb", {61, 18}, 0, {}, {}, {Unique}},

{"Slam", {}, 0, RightHand, {}, {Natural}, {OneAttack, Bludgeon, -2, {1, 8}, {}}},
{"Slam", {}, 0, RightHand, {}, {Natural}, {OneAttack, Bludgeon, -2, {1, 4}, {}}},
{"Claws", {}, 0, RightHand, {}, {Natural}, {TwoAttacks, Slashing, -3, {1, 4}, {}}},
{"Bite", {}, 0, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 6}, {}}},
{"Bite", {}, 0, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 4, 1}, {}}},
{"Bite", {}, 0, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {2, 6}, {}}},
{"Shoking hang", {80}, 0, RightHand, {}, {Natural, Charged}, {OneAttack, Electricity, -4, {1, 8}, {0, 0, 1}}},
{"Flame blade", {82}, 0, RightHand, {}, {Natural, Charged, SevereDamageUndead}, {OneAttack, Fire, -5, {1, 4, 4}, {}}},
{"Flame hand", {81}, 0, RightHand, {}, {Natural, Charged, Ranged}, {OneAttack, Fire, -1, {1, 4, 1}, {}}},
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

rarity_s item::getrandomrarity(int level) {
	static int source[][Artifact] = {{},
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
	return getmagic() != 0;
}

int	item::getac() const {
	return bsdata<itemi>::elements[type].armor.ac;
}

int	item::getdeflect() const {
	return bsdata<itemi>::elements[type].armor.critical_deflect;
}

int	item::getspeed() const {
	return bsdata<itemi>::elements[type].weapon.speed;
}

int item::getportrait() const {
	return bsdata<itemi>::elements[type].image.avatar;
}

int	item::get(variant value) const {
	auto pe = getenchantment();
	if(!pe)
		return 0;
	if(pe->power == value) {
		if(iscursed())
			return -pe->magic;
		return pe->magic;
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
	result.bonus += wi.bonus + get(OfAccuracy) + magic_bonus;
	result.damage.b += get(Bludgeon) + magic_bonus;
	if(is(Deadly))
		result.critical_multiplier++;
	if(is(Quick))
		result.critical_range++;
	if(enemy) {
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
			else if(pe->magic) {
				auto magic = getmagic();
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
		return -pe->magic;
	return pe->magic;
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
	} else if(is(UseLeatherArmor)) {
		switch(skill) {
		case OpenLocks: return 10;
		case RemoveTraps: return 15;
		case ClimbWalls: return 30;
		case MoveSilently: return 40;
		}
	}
	return 0;
}

variant item::getpower() const {
	auto& ei = bsdata<itemi>::elements[type];
	if(!ei.enchantments.count)
		return variant();
	return ei.enchantments.data[subtype].power;
}

const enchantmenti* item::getenchantment() const {
	auto& ei = bsdata<itemi>::elements[type].enchantments;
	if(!ei)
		return 0;
	return ei.data + subtype;
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