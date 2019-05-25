#include "main.h"

static enchant_s magic_swords[] = {OfSpeed, OfFire, OfCold, OfSharpness, OfAccuracy, OfProtection, OfVampirism, OfLuck, OfHolyness};
static enchant_s magic_weapon[] = {OfSpeed, OfFire, OfAccuracy, OfSharpness, OfHolyness};
static enchant_s magic_bludgeon[] = {OfFire, OfSmashing, OfDamage, OfHolyness};
static enchant_s ring_red[] = {OfSpeed, OfFireResistance, OfProtection, OfAdvise};
static enchant_s ring_green[] = {OfRegeneration, OfHealing, OfPoison, OfPoisonResistance, OfProtection};
static enchant_s ring_blue[] = {OfClimbing, OfWizardy, OfProtection, OfLuck, OfInvisibility};
static enchant_s potion_red[] = {OfSpeed, OfFireResistance};
static enchant_s potion_green[] = {OfNeutralizePoison, OfClimbing, OfStrenght};
static enchant_s potion_blue[] = {OfClimbing, OfHealing};
static enchant_s magic_boots[] = {OfSpeed};
static enchant_s magic_bracers[] = {OfSpeed, OfProtection};
static enchant_s magic_shield[] = {OfFireResistance, OfMagicResistance};

itemi bsmeta<itemi>::elements[] = {{"No item"},
{"Battle axe", 7, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile, Deadly}, {OneAttack, Slashing, -7, {1, 8}, {1, 8}}, {}, magic_weapon},
{"Axe", 7, RightHand, {UseMartialWeapon}, {Deadly}, {OneAttack, Slashing, -4, {1, 6}, {1, 4}}, {}, magic_weapon},
{"Club", 76, RightHand, {}, {}, {OneAttack, Bludgeon, -4, {1, 6}, {1, 4}}, {}, magic_bludgeon},
{"Dagger", 15, RightHand, {}, {Quick}, {OneAttack, Pierce, -2, {1, 4}, {1, 3}}, {}, magic_weapon},
{"Flail", 5, RightHand, {}, {}, {OneAttack, Bludgeon, -7, {1, 6, 1}, {1, 6}}, {}, magic_weapon},
{"Halberd", 3, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded, Deadly, Quick}, {OneAttack, Slashing, -9, {1, 10}, {2, 6}}, {}, magic_weapon},
{"Warhammer", 99, RightHand, {}, {}, {OneAttack, Bludgeon, -4, {1, 4, 1}, {1, 4}}, {}, magic_bludgeon},
{"Mace", 4, RightHand, {}, {}, {OneAttack, Bludgeon, -6, {1, 6, 1}, {1, 6}}, {}, magic_bludgeon},
{"Spear", 6, RightHand, {UseLargeWeapon}, {Versatile}, {OneAttack, Pierce, -6, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Staff", 8, RightHand, {}, {TwoHanded}, {OneAttack, Bludgeon, -8, {1, 6}, {1, 4}}, {}, magic_weapon},
{"Bastard sword", 45, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile}, {OneAttack, Slashing, -6, {2, 4}, {2, 8}}, {}, magic_weapon},
{"Longsword", 1, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_weapon},
{"Short sword", 2, RightHand, {UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -3, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Two-handed sword", 42, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded}, {OneAttack, Slashing, -10, {1, 10}, {3, 6}}, {}, magic_weapon},
//
{"Bow", 10, RightHand, {UseTheifWeapon}, {TwoHanded, Ranged}, {OneAttack, Pierce, -8, {1, 8}, {1, 8}}},
{"Sling", 18, RightHand, {}, {Ranged}, {OneAttack, Bludgeon, -6, {1, 4}, {1, 4}}},
//
{"Robe", 32, Body},
{"Leather armor", 31, Body, {UseLeatherArmor}, {}, {}, {2}},
{"Studded leather armor", 31, Body, {UseLeatherArmor}, {}, {}, {3}},
{"Scale mail", 30, Body, {UseMetalArmor}, {}, {}, {4, 2}},
{"Chain mail", 29, Body, {UseMetalArmor}, {}, {}, {5}},
{"Banded mail", 28, Body, {UseMetalArmor}, {}, {}, {7, 1}},
{"Plate mail", 26, Body, {UseMetalArmor}, {}, {}, {8, 2}},
//
{"Helm", 20, Head, {UseShield}, {}, {}, {0, 2}},
{"Shield", 23, LeftHand, {UseShield}, {}, {}, {1, 2}, magic_shield},
{"Boots", 21, Legs, {}, {}, {}, {}, magic_boots},
//
{"Bracers", 25, Elbow, {}, {}, {}, {0, 2}, magic_bracers},
//
{"Arrow", 16},
{"Dart", 14, RightHand},
{"Stone", 19, RightHand},
//
{"Bones", 43, {}, {}, {}},
{"Map", 86, {}, {}, {}},
//
{"Holy Symbol", 53, {}, {UseDivine}},
{"Spell book", 35, {}, {UseArcane}},
{"Lockpicks", 54, {}, {UseTheif}},
//
{"Wand", 52, {}, {UseArcane}},
{"Scroll", 36, {}, {UseScrolls, UseArcane}},
{"Scroll", 85, {}, {UseScrolls, UseDivine}},
//
{"Shelf key", 46},
{"Silver key", 47},
{"Cooper key", 48},
{"Skull key", 87},
{"Spider key", 62},
{"Moon key", 88},
{"Diamond key", 102},
{"Green key", 50},
// Кольца
{"Ring", 55, RightRing, {}, {}, {}, {}, ring_red},
{"Ring", 78, RightRing, {}, {}, {}, {}, ring_blue},
{"Ring", 79, RightRing, {}, {}, {}, {}, ring_green},
//
{"Potion", 39, {}, {}, {}, {}, {}, potion_red},
{"Potion", 40, {}, {}, {}, {}, {}, potion_blue},
{"Potion", 41, {}, {}, {}, {}, {}, potion_green},
//
{"Red gem", 93},
{"Blue gem", 94},
{"Green gem", 95},
{"Purple gem", 96},
// Еда
{"Ration", 38},
{"Iron ration", 37},
//
{"Slam", 0, RightHand, {}, {}, {OneAttack, Bludgeon, -2, {1, 8}, {1, 8}}},
{"Claws", 0, RightHand, {}, {}, {TwoAttacks, Slashing, -3, {1, 4}, {1, 4}}},
{"Bite", 0, RightHand, {}, {}, {OneAttack, Pierce, -4, {1, 6}, {1, 6}}},
{"Bite", 0, RightHand, {}, {}, {OneAttack, Pierce, -4, {2, 6}, {2, 6}}},
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

static bool havespell(item_s t) {
	return t == PriestScroll || t == MageScroll || t == MagicWand;
}

static item_s getscroll(spell_s id) {
	if(creature::getlevel(id, Mage))
		return MageScroll;
	return PriestScroll;
}

item::item(item_s type, int chance_magic) : item(type) {
	if(d100() < chance_magic) {
		auto d = rand() % 100;
		if(d < 50)
			magic = 0;
		else if(d < 75)
			magic = 1;
		else if(d < 92)
			magic = 2;
		else
			magic = 3;
		if(bsmeta<itemi>::elements[type].enchantments.count)
			subtype = bsmeta<itemi>::elements[type].enchantments.data[rand() % bsmeta<itemi>::elements[type].enchantments.count];
		if(!ismagical())
			setcursed(1);
	}
	if(type == MagicWand) {
		static spell_s random_spells[] = {SpellMagicMissile, SpellBurningHands, SpellDetectMagic, SpellSleep};
		setspell(maprnd(random_spells));
		setcharges(dice::roll(3, 6));
	} else if(type == MageScroll) {
		adat<spell_s, 32> spells;
		spells.count = creature::select(spells.data, zendof(spells.data), Mage, 1);
		if(spells.count)
			setspell(spells.data[rand() % spells.count]);
	} else if(type == PriestScroll) {
		adat<spell_s, 32> spells;
		spells.count = creature::select(spells.data, zendof(spells.data), Cleric, 1);
		if(spells.count)
			setspell(spells.data[rand() % spells.count]);
	}
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
	return bsmeta<itemi>::elements[type].armor.ac;
}

int	item::getdeflect() const {
	return bsmeta<itemi>::elements[type].armor.critical_deflect;
}

int	item::getspeed() const {
	return bsmeta<itemi>::elements[type].weapon.speed;
}

int item::getportrait() const {
	return bsmeta<itemi>::elements[type].portrait;
}

spell_s item::getspell() const {
	if(!havespell(type))
		return NoSpell;
	return (spell_s)subtype;
}

enchant_s item::getenchant() const {
	if(havespell(type))
		return NoEnchant;
	return subtype;
}

int	item::get(enchant_s value) const {
	if(!havespell(type) && subtype == value)
		return getmagic();
	return 0;
}

void item::get(combati& result, const creature* enemy) const {
	auto& wi = bsmeta<itemi>::elements[type].weapon;
	auto size = enemy ? enemy->getsize() : Medium;
	if(size == Large)
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
	result.critical_range += get(OfSharpness);
	result.critical_multiplier += get(OfSmashing);
}

char* item::getname(char* result, const char* result_maximum) const {
	stringcreator sc(result, result_maximum);
	if(isbroken()) {
		if(type == RationIron || type == Ration)
			sc.adds("rotten");
		else
			sc.adds("broken");
	}
	if(isidentified() && iscursed())
		sc.adds("cursed");
	sc.adds(bsmeta<itemi>::elements[type].name);
	if(isidentified()) {
		if(type == MagicWand || type == MageScroll || type == PriestScroll) {
			auto spell = getspell();
			sc.adds("of %1", getstr(spell));
		} else {
			auto enchant = getenchant();
			if(enchant)
				sc.adds("of %1", getstr(enchant));
			if(type != PotionBlue && type != PotionGreen && type != PotionRed) {
				auto magic = getmagic();
				if(magic) {
					if(bsmeta<enchanti>::elements[enchant].names)
						sc.adds(bsmeta<enchanti>::elements[enchant].names[imin(5, iabs(magic)) - 1]);
					else
						sc.adds("%+1i", magic);
				}
			}
		}
	}
	szupper(result, 1);
	return result;
}

void item::setspell(spell_s spell) {
	if(havespell(type))
		subtype = (enchant_s)spell;
}

bool item::ismelee() const {
	return (type >= AxeBattle && type <= SwordTwoHanded)
		|| (type >= Slam && type <= LastItem);
}

int	item::getmagic() const {
	auto r = magic;
	if(subtype && !havespell(type))
		r += bsmeta<enchanti>::elements[subtype].magic;
	if(iscursed()) {
		if(r == 0)
			return -1;
		return -r;
	}
	return r;
}