#include "main.h"

static variant var_magic_swords[] = {{}, Speed, Fire, Cold, CriticalRange, CriticalMultiply, Attack, AC};
static variant var_magic_weapon[] = {{}, Speed, Fire, Attack, CriticalRange, Undead};
static variant var_magic_bludgeon[] = {{}, Fire, CriticalMultiply, Damage, Undead};
static variant var_ring_red[] = {ResistFire, ResistCold, AC, Undead};

static enchant_s magic_swords[] = {OfSpeed, OfFire, OfCold, OfSharpness, OfAccuracy, OfProtection, OfVampirism, OfLuck, OfHolyness, OfStrenghtDrain};
static enchant_s magic_weapon[] = {OfSpeed, OfFire, OfAccuracy, OfSharpness, OfHolyness};
static enchant_s magic_bludgeon[] = {OfFire, OfSmashing, OfDamage, OfHolyness};
static enchant_s ring_red[] = {OfWizardy, OfFireResistance, OfProtection, OfAdvise};
static enchant_s ring_green[] = {OfRegeneration, OfHealing, OfPoison, OfPoisonResistance, OfProtection};
static enchant_s ring_blue[] = {OfClimbing, OfSpeed, OfProtection, OfLuck, OfInvisibility};
static enchant_s potion_red[] = {OfSpeed, OfFireResistance, OfKnowledge};
static enchant_s potion_green[] = {OfNeutralizePoison, OfClimbing, OfStrenght};
static enchant_s potion_blue[] = {OfClimbing, OfHealing, OfHealing, OfRegeneration, OfAdvise};
static enchant_s magic_boots[] = {OfSpeed, OfClimbing};
static enchant_s magic_bracers[] = {OfSpeed, OfProtection};
static enchant_s magic_amulets[] = {OfSpeed, OfProtection};
static enchant_s magic_shield[] = {OfFireResistance, OfMagicResistance};
static enchant_s magic_helm[] = {OfIntellegence, OfCharisma};
static enchant_s magic_armor[] = {OfFireResistance, OfPoisonResistance, OfMagicResistance, OfClimbing};
static enchant_s magic_robe[] = {OfProtection, OfSpeed, OfMagicResistance, OfFireResistance, OfPoisonResistance, OfMagicResistance};

static spell_s wand_spells[] = {MagicMissile, BurningHands, DetectMagic, Sleep, Mending};
static spell_s staff_spells[] = {MagicMissile, BurningHands, DetectMagic, Sleep, Mending};
static spell_s wizard_spells[] = {BurningHands, DetectMagic, FeatherFall, MageArmor, MagicMissile, Mending,
ReadLanguagesSpell, ShieldSpell, ShokingGrasp, Sleep,
Blur, Invisibility, Knock, ProduceFlame};
static spell_s priest_spells[] = {Bless, CureLightWounds, DetectEvil, ProtectionFromEvil, PurifyFood,
Aid, FlameBlade, Goodberry, HoldPerson, SlowPoison,
CreateFood, CureBlindnessDeafness, CureDisease, NegativePlanProtection};

itemi bsdata<itemi>::elements[] = {{"No item"},
{"Battle axe", {7, 4, 1}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile, Deadly}, {OneAttack, Slashing, -7, {1, 8}, {1, 8}}, {}, magic_weapon},
{"Axe", {7, 4, 1}, RightHand, {UseMartialWeapon}, {Deadly}, {OneAttack, Slashing, -4, {1, 6}, {1, 4}}, {}, magic_weapon},
{"Club", {76, 1}, RightHand, {}, {}, {OneAttack, Bludgeon, -4, {1, 6}, {1, 4}}, {}, magic_bludgeon},
{"Dagger", {15, 3}, RightHand, {}, {Quick}, {OneAttack, Pierce, -2, {1, 4}, {1, 3}}, {}, magic_weapon},
{"Flail", {5, 2, 1}, RightHand, {}, {}, {OneAttack, Bludgeon, -7, {1, 6, 1}, {1, 6}}, {}, magic_weapon},
{"Halberd", {3, 5, 1}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded, Deadly, Quick}, {OneAttack, Slashing, -9, {1, 10}, {2, 6}}, {}, magic_weapon},
{"Warhammer", {99, 10, 1}, RightHand, {}, {}, {OneAttack, Bludgeon, -4, {1, 4, 1}, {1, 4}}, {}, magic_bludgeon},
{"Mace", {4, 1, 1}, RightHand, {}, {}, {OneAttack, Bludgeon, -6, {1, 6, 1}, {1, 6}}, {}, magic_bludgeon},
{"Spear", {6, 3, 1}, RightHand, {UseLargeWeapon}, {Versatile}, {OneAttack, Pierce, -6, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Staff", {8, 3, 1}, RightHand, {}, {TwoHanded}, {OneAttack, Bludgeon, -8, {1, 6}, {1, 4}}, {}, {}, staff_spells},
{"Bastard sword", {45, 0, 1}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile}, {OneAttack, Slashing, -6, {2, 4}, {2, 8}}, {}, magic_weapon},
{"Longsword", {1, 0, 1}, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_weapon},
{"Short sword", {2, 0, 1}, RightHand, {UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -3, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Two-handed sword", {42, 0, 1}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded}, {OneAttack, Slashing, -10, {1, 10}, {3, 6}}, {}, magic_weapon},
//
{"Bow", {10, 6, 1, Arrow}, RightHand, {UseTheifWeapon}, {TwoHanded, Ranged}, {TwoAttacks, Pierce, -8, {1, 8}, {1, 8}}},
{"Sling", {18, 4, 0, Stone}, RightHand, {}, {Ranged}, {OneAttack, Bludgeon, -6, {1, 4}, {1, 4}}},
//
{"Robe", {32, 8, 1}, Body, {UseArcane}, {Wonderful}, {}, {}, magic_robe},
{"Leather armor", {31, 8, 1}, Body, {UseLeatherArmor}, {}, {}, {2}, magic_armor},
{"Studded leather armor", {31, 8, 1}, Body, {UseLeatherArmor}, {}, {}, {3}, magic_armor},
{"Scale mail", {30, 9, 1}, Body, {UseMetalArmor}, {}, {}, {4, 2}, magic_armor},
{"Chain mail", {29, 9, 1}, Body, {UseMetalArmor}, {}, {}, {5}, magic_armor},
{"Banded mail", {28, 9, 1}, Body, {UseMetalArmor}, {}, {}, {7, 3}, magic_armor},
{"Plate mail", {26, 9, 1}, Body, {UseMetalArmor}, {}, {}, {8, 4}, magic_armor},
//
{"Helm", {20, 6}, Head, {UseShield}, {Wonderful}, {}, {0, 2}, magic_helm},
{"Shield", {23, 7, 1}, LeftHand, {UseShield}, {}, {}, {1, 2}, magic_shield},
{"Boots", {21, 9}, Legs, {}, {Wonderful}, {}, {0, 1}, magic_boots},
//
{"Bracers", {25, 16}, Elbow, {}, {Wonderful}, {}, {}, magic_bracers},
{"Necklage", {33, 13}, Neck, {}, {Wonderful}, {}, {}, magic_amulets},
{"Necklage", {34, 13}, Neck, {}, {Wonderful}, {}, {}, magic_amulets},
{"Jewelry", {108, 13}, Neck, {}, {Wonderful}, {}, {}, magic_amulets},
//
{"Arrow", {16, 5}},
{"Dart", {14, 0}, RightHand},
{"Stone", {19, 2}, RightHand},
//
{"Bones", {43, 7}, {}, {}, {}},
{"Map", {86, 12}, {}, {}, {}},
//
{"Holy Symbol", {53, 20}, {}, {UseDivine}, {UseInHand}},
{"Holy Symbol", {27, 20}, {}, {UseDivine, UseMartialWeapon}, {UseInHand}},
{"Spell book", {35, 11}, {}, {UseArcane}, {UseInHand}},
{"Lockpicks", {54, 1}, {}, {UseTheif}},
//
{"Wand", {52, 10}, {}, {UseArcane}, {Magical, Wonderful, UseInHand, Charged}, {}, {}, {}, wand_spells},
{"Scroll", {36, 12}, {}, {UseScrolls, UseArcane}, {Magical, Wonderful}, {}, {}, {}, wizard_spells},
{"Scroll", {85, 12}, {}, {UseScrolls, UseDivine}, {Magical, Wonderful}, {}, {}, {}, priest_spells},
// Keys
{"Shelf key", {46, 8}},
{"Silver key", {47, 8}},
{"Cooper key", {48, 8}},
{"Skull key", {87, 8}},
{"Spider key", {62, 8}},
{"Moon key", {88, 8}},
{"Diamond key", {102, 8}},
{"Green key", {50, 8}},
// Rings
{"Ring", {55, 15}, RightRing, {UseArcane}, {Wonderful}, {}, {}, ring_red},
{"Ring", {78, 15}, RightRing, {}, {Wonderful}, {}, {}, ring_blue},
{"Ring", {79, 15}, RightRing, {}, {Wonderful}, {}, {}, ring_green},
// Potions
{"Potion", {39, 19}, {}, {}, {Wonderful, Magical}, {}, {}, potion_red},
{"Potion", {40, 19}, {}, {}, {Wonderful, Magical}, {}, {}, potion_blue},
{"Potion", {41, 19}, {}, {}, {Wonderful, Magical}, {}, {}, potion_green},
// Gems
{"Red gem", {93, 22}},
{"Blue gem", {94, 22}},
{"Green gem", {95, 22}},
{"Purple gem", {96, 22}},
// Food
{"Ration", {38, 14}},
{"Iron ration", {37, 14}},
// Unique/Special items
{"Dust of Ages", {97, 25}, {}, {UseArcane}, {Unique}},
{"Horn", {59, 23}, {}, {}, {Unique}},
{"Mantist Head", {51, 18}, {}, {}, {Unique}},
{"Scepeter", {66, 17}, RightHand, {}, {Unique, Wonderful}, {OneAttack, Bludgeon, -5, {1, 6}, {1, 6}}, {}, magic_weapon},
{"Silver sword", {65, 0}, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick, Unique, SevereDamageUndead}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_weapon},
{"Stone amulet", {64, 13}, Neck, {}, {Unique}},
{"Stone dagger", {60, 3}, {}, {}, {Unique}},
{"Stone gem", {57, 22}, {}, {}, {Unique}},
{"Stone holy symbol", {63, 20}, {}, {UseDivine}, {Unique}},
{"Stone orb", {61, 18}, {}, {}, {Unique}},
// Natural weapon
{"Slam", {}, RightHand, {}, {Natural}, {OneAttack, Bludgeon, -2, {1, 8}, {}}},
{"Slam", {}, RightHand, {}, {Natural}, {OneAttack, Bludgeon, -2, {1, 4}, {}}},
{"Claws", {}, RightHand, {}, {Natural}, {TwoAttacks, Slashing, -3, {1, 4}, {}}},
{"Bite", {}, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 6}, {}}},
{"Bite", {}, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 4, 1}, {}}},
{"Bite", {}, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {2, 6}, {}}},
{"Shoking hang", {80}, RightHand, {}, {Natural, Charged}, {OneAttack, Electricity, -4, {1, 8}, {0, 0, 1}}},
{"Flame blade", {82}, RightHand, {}, {Natural, Charged, SevereDamageUndead}, {OneAttack, Fire, -5, {1, 4, 4}, {}}},
{"Flame hand", {81}, RightHand, {}, {Natural, Charged, Ranged}, {OneAttack, Fire, -1, {1, 4, 1}, {}}},
};
assert_enum(item, LastItem);
static_assert(sizeof(item) == 4, "Not valid items count");

static char random_cleric_spell_level() {
	static char levels[] = {
		1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5,
		6, 6,
		7};
	return maprnd(levels);
}

static char random_mage_spell_level() {
	static char levels[] = {
		1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5,
		6, 6,
		7,
		8,
		9};
	return maprnd(levels);
}

static item_s getscroll(spell_s id) {
	if(creature::getlevel(id, Mage))
		return MageScroll;
	return PriestScroll;
}

static int special_magic_bonus() {
	auto d = rand() % 100;
	if(d < 50)
		return 0;
	else if(d < 75)
		return 1;
	else if(d < 92)
		return 2;
	else
		return 3;
}

static int standart_magic_bonus() {
	auto d = rand() % 100;
	if(d < 60)
		return 1;
	else if(d < 90)
		return 2;
	else
		return 3;
}

item::item(item_s type, int chance_magic, int chance_cursed, int chance_special) : item(type) {
	if(is(Magical))
		chance_magic = 100;
	if(is(Wonderful))
		chance_special = 100;
	if(is(Charged))
		setcharges(dice::roll(3, 6));
	if(d100() < chance_magic) {
		if(bsdata<itemi>::elements[type].enchantments.count && (d100() < chance_special)) {
			subtype = bsdata<itemi>::elements[type].enchantments.data[rand() % bsdata<itemi>::elements[type].enchantments.count];
			magic = special_magic_bonus();
		} else if(bsdata<itemi>::elements[type].spells.count && (d100() < chance_special)) {
			subtype = (enchant_s)bsdata<itemi>::elements[type].spells.data[rand() % bsdata<itemi>::elements[type].spells.count];
			magic = special_magic_bonus();
		} else
			magic = standart_magic_bonus();
	}
	if(d100() < chance_cursed)
		setcursed(1);
}

item::item(spell_s spell) : item(getscroll(spell)) {
	setspell(spell);
	setidentified(1);
}

void item::clear() {
	memset(this, 0, sizeof(*this));
}

bool item::ismagical() const {
	return cursed || magic || subtype;
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

spell_s item::getspell() const {
	if(bsdata<itemi>::elements[type].spells.count == 0)
		return NoSpell;
	return (spell_s)subtype;
}

enchant_s item::getenchant() const {
	if(bsdata<itemi>::elements[type].enchantments.count == 0)
		return NoEnchant;
	return subtype;
}

int	item::get(enchant_s value) const {
	if(bsdata<itemi>::elements[type].enchantments.count != 0
		&& subtype == value)
		return getmagic();
	return 0;
}

void item::get(combati& result, const creature* enemy) const {
	auto& wi = bsdata<itemi>::elements[type].weapon;
	auto size = enemy ? enemy->getsize() : Medium;
	if(size == Large && !is(Natural))
		result.damage = wi.damage_large;
	else
		result.damage = wi.damage;
	result.type = wi.type;
	result.attack = wi.attack;
	result.bonus += wi.bonus + get(OfAccuracy) + getmagic();
	result.damage.b += get(OfDamage) + getmagic();
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
		if(type == MagicWand || type == MageScroll || type == PriestScroll) {
			auto spell = getspell();
			sc.adds("of %1", getstr(spell));
		} else {
			auto enchant = getenchant();
			auto spell = getspell();
			if(enchant)
				sc.adds("of %1", getstr(enchant));
			else if(spell)
				sc.adds("of %1", getstr(spell));
			if(type != PotionBlue && type != PotionGreen && type != PotionRed) {
				auto magic = getmagic();
				if(magic) {
					if(bsdata<enchanti>::elements[enchant].names)
						sc.adds(bsdata<enchanti>::elements[enchant].names[imin(5, iabs(magic)) - 1]);
					else
						sc.adds("%+1i", magic);
				}
			}
		}
	}
}

void item::setspell(spell_s spell) {
	if(bsdata<itemi>::elements[type].spells.count != 0)
		subtype = (enchant_s)spell;
}

bool item::ismelee() const {
	return (type >= AxeBattle && type <= SwordTwoHanded)
		|| (type >= Slam && type <= LastItem);
}

int	item::getmagic() const {
	auto r = magic;
	if(subtype)
		r++;
	if(cursed != 0)
		return - (r + 1);
	return r;
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
		if(is(Natural) || is(Unique) || magic == 3)
			return false;
		// Magical items more durable (cursed break every time)
		if(getmagic() > 0 && (d100() < (getmagic() * 15)))
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
		broken = 1;
	}
	return false;
}

int item::getarmorpenalty(skill_s skill) const {
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