#include "main.h"

static enchant_s magic_swords[] = {OfSpeed, OfFire, OfCold, OfSharpness, OfAccuracy, OfProtection, OfVampirism, OfLuck, OfHolyness};
static enchant_s magic_weapon[] = {OfSpeed, OfFire, OfAccuracy, OfSharpness, OfHolyness};
static enchant_s magic_bludgeon[] = {OfFire, OfSmashing, OfDamage, OfHolyness};
static enchant_s ring_red[] = {OfSpeed, OfFireResistance, OfProtection, OfAdvise};
static enchant_s ring_green[] = {OfRegeneration, OfHealing, OfPoison, OfPoisonResistance, OfProtection};
static enchant_s ring_blue[] = {OfClimbing, OfWizardy, OfProtection, OfLuck, OfInvisibility};
static enchant_s potion_red[] = {OfSpeed, OfFireResistance};
static enchant_s potion_green[] = {OfNeutralizePoison, OfClimbing, OfStrenght};
static enchant_s potion_blue[] = {OfClimbing, OfHealing, OfAdvise};
static enchant_s magic_boots[] = {OfSpeed};
static enchant_s magic_bracers[] = {OfSpeed, OfProtection};
static enchant_s magic_shield[] = {OfFireResistance, OfMagicResistance};


static int get_item_portrait(int rec) {
	switch(rec) {
	case HolySymbol: return 20;
	default:
		if(rec >= KeyShelf && rec <= KeyGreen)
			return 8;
		return 0;
	}
}

itemi bsmeta<itemi>::elements[] = {{"No item"},
{"Battle axe", {7, 4}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile, Deadly}, {OneAttack, Slashing, -7, {1, 8}, {1, 8}}, {}, magic_weapon},
{"Axe", {7, 4}, RightHand, {UseMartialWeapon}, {Deadly}, {OneAttack, Slashing, -4, {1, 6}, {1, 4}}, {}, magic_weapon},
{"Club", {76, 1}, RightHand, {}, {}, {OneAttack, Bludgeon, -4, {1, 6}, {1, 4}}, {}, magic_bludgeon},
{"Dagger", {15, 3}, RightHand, {}, {Quick}, {OneAttack, Pierce, -2, {1, 4}, {1, 3}}, {}, magic_weapon},
{"Flail", {5, 2}, RightHand, {}, {}, {OneAttack, Bludgeon, -7, {1, 6, 1}, {1, 6}}, {}, magic_weapon},
{"Halberd", {3, 5}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded, Deadly, Quick}, {OneAttack, Slashing, -9, {1, 10}, {2, 6}}, {}, magic_weapon},
{"Warhammer", {99, 10}, RightHand, {}, {}, {OneAttack, Bludgeon, -4, {1, 4, 1}, {1, 4}}, {}, magic_bludgeon},
{"Mace", {4, 1}, RightHand, {}, {}, {OneAttack, Bludgeon, -6, {1, 6, 1}, {1, 6}}, {}, magic_bludgeon},
{"Spear", {6, 3}, RightHand, {UseLargeWeapon}, {Versatile}, {OneAttack, Pierce, -6, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Staff", {8, 3}, RightHand, {}, {TwoHanded}, {OneAttack, Bludgeon, -8, {1, 6}, {1, 4}}, {}, magic_weapon},
{"Bastard sword", {45, 0}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile}, {OneAttack, Slashing, -6, {2, 4}, {2, 8}}, {}, magic_weapon},
{"Longsword", {1, 0}, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_weapon},
{"Short sword", {2, 0}, RightHand, {UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -3, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Two-handed sword", {42, 0}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded}, {OneAttack, Slashing, -10, {1, 10}, {3, 6}}, {}, magic_weapon},
//
{"Bow", {10, 6}, RightHand, {UseTheifWeapon}, {TwoHanded, Ranged}, {OneAttack, Pierce, -8, {1, 8}, {1, 8}}},
{"Sling", {18, 4}, RightHand, {}, {Ranged}, {OneAttack, Bludgeon, -6, {1, 4}, {1, 4}}},
//
{"Robe", {32, 8}, Body},
{"Leather armor", {31, 8}, Body, {UseLeatherArmor}, {}, {}, {2}},
{"Studded leather armor", {31, 8}, Body, {UseLeatherArmor}, {}, {}, {3}},
{"Scale mail", {30, 9}, Body, {UseMetalArmor}, {}, {}, {4, 2}},
{"Chain mail", {29, 9}, Body, {UseMetalArmor}, {}, {}, {5}},
{"Banded mail", {28, 9}, Body, {UseMetalArmor}, {}, {}, {7, 1}},
{"Plate mail", {26, 9}, Body, {UseMetalArmor}, {}, {}, {8, 2}},
//
{"Helm", {20, 6}, Head, {UseShield}, {}, {}, {0, 2}},
{"Shield", {23, 7}, LeftHand, {UseShield}, {}, {}, {1, 2}, magic_shield},
{"Boots", {21, 9}, Legs, {}, {}, {}, {}, magic_boots},
//
{"Bracers", {25, 16}, Elbow, {}, {}, {}, {0, 2}, magic_bracers},
//
{"Arrow", {16, 5}},
{"Dart", {14, 0}, RightHand},
{"Stone", {19, 2}, RightHand},
//
{"Bones", {43, 7}, {}, {}, {}},
{"Map", {86, 12}, {}, {}, {}},
//
{"Holy Symbol", {53, 20}, {}, {UseDivine}},
{"Spell book", {35, 11}, {}, {UseArcane}},
{"Lockpicks", {54, 1}, {}, {UseTheif}},
//
{"Wand", {52, 10}, {}, {UseArcane}},
{"Scroll", {36, 12}, {}, {UseScrolls, UseArcane}},
{"Scroll", {85, 12}, {}, {UseScrolls, UseDivine}},
//
{"Shelf key", {46, 8}},
{"Silver key", {47, 8}},
{"Cooper key", {48, 8}},
{"Skull key", {87, 8}},
{"Spider key", {62, 8}},
{"Moon key", {88, 8}},
{"Diamond key", {102, 8}},
{"Green key", {50, 8}},
// Кольца
{"Ring", {55, 15}, RightRing, {}, {Wonderful}, {}, {}, ring_red},
{"Ring", {78, 15}, RightRing, {}, {Wonderful}, {}, {}, ring_blue},
{"Ring", {79, 15}, RightRing, {}, {Wonderful}, {}, {}, ring_green},
//
{"Potion", {39, 19}, {}, {}, {Wonderful, Magical}, {}, {}, potion_red},
{"Potion", {40, 19}, {}, {}, {Wonderful, Magical}, {}, {}, potion_blue},
{"Potion", {41, 19}, {}, {}, {Wonderful, Magical}, {}, {}, potion_green},
//
{"Red gem", {93, 22}},
{"Blue gem", {94, 22}},
{"Green gem", {95, 22}},
{"Purple gem", {96, 22}},
// Еда
{"Ration", {38, 14}},
{"Iron ration", {37, 14}},
//
{"Slam", {}, RightHand, {}, {Natural}, {OneAttack, Bludgeon, -2, {1, 8}, {}}},
{"Claws", {}, RightHand, {}, {Natural}, {TwoAttacks, Slashing, -3, {1, 4}, {}}},
{"Bite", {}, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 6}, {}}},
{"Bite", {}, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {1, 4, 1}, {}}},
{"Bite", {}, RightHand, {}, {Natural}, {OneAttack, Pierce, -4, {2, 6}, {}}},
{"Shoking grasp", {80}, RightHand, {}, {Natural, Charged}, {OneAttack, Electricity, -4, {1, 8}, {0, 0, 1}}},
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

static bool havespell(item_s t) {
	return t == PriestScroll || t == MageScroll || t == MagicWand;
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
	adat<spell_s, 32> spells;
	static spell_s random_spells[] = {MagicMissile, BurningHands, DetectMagic, Sleep};
	switch(type) {
	case MagicWand:
		setspell(maprnd(random_spells));
		setcharges(dice::roll(3, 6));
		magic = special_magic_bonus();
		break;
	case MageScroll:
		spells.count = creature::select(spells.data, zendof(spells.data), Mage, 1);
		if(spells.count)
			setspell(spells.data[rand() % spells.count]);
		magic = special_magic_bonus();
		break;
	case PriestScroll:
		spells.count = creature::select(spells.data, zendof(spells.data), Cleric, 1);
		if(spells.count)
			setspell(spells.data[rand() % spells.count]);
		magic = special_magic_bonus();
		break;
	default:
		if(is(Magical))
			chance_magic = 100;
		if(is(Wonderful))
			chance_special = 100;
		if(d100() < chance_magic) {
			if(bsmeta<itemi>::elements[type].enchantments.count && (d100() < chance_special)) {
				subtype = bsmeta<itemi>::elements[type].enchantments.data[rand() % bsmeta<itemi>::elements[type].enchantments.count];
				magic = special_magic_bonus();
			} else
				magic = standart_magic_bonus();
		}
		break;
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
	return bsmeta<itemi>::elements[type].armor.ac;
}

int	item::getdeflect() const {
	return bsmeta<itemi>::elements[type].armor.critical_deflect;
}

int	item::getspeed() const {
	return bsmeta<itemi>::elements[type].weapon.speed;
}

int item::getportrait() const {
	return bsmeta<itemi>::elements[type].image.avatar;
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
		r++;
	if(iscursed())
		return - r - 1;
	return r;
}

void item::setcharges(int value) {
	if(value<=0)
		clear();
	else
		charges = value;
}