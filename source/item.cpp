#include "main.h"

static variant magic_swords[] = {{}, OfSpeed, OfFire, OfCold, OfSharpness, OfAccuracy, OfProtection, OfVampirism, OfLuck, OfHolyness, OfStrenghtDrain};
static variant magic_weapon[] = {{}, OfSpeed, OfFire, OfAccuracy, OfSharpness, OfHolyness};
static variant magic_bludgeon[] = {{}, OfFire, OfSmashing, OfDamage, OfHolyness};
static variant ring_red[] = {{}, OfWizardy, ResistFire, ResistCold, OfProtection, OfAdvise};
static variant ring_green[] = {{}, OfRegeneration, OfHealing, SaveVsPoison, OfProtection};
static variant ring_blue[] = {{}, ClimbWalls, OfSpeed, OfProtection, OfLuck, OfInvisibility};
static variant potion_red[] = {OfSpeed, ResistCold, ResistFire, OfKnowledge};
static variant potion_green[] = {OfNeutralizePoison, ClimbWalls, Strenght};
static variant potion_blue[] = {ClimbWalls, OfHealing, OfHealing, OfRegeneration, OfAdvise};
static variant magic_boots[] = {{}, OfSpeed, ClimbWalls};
static variant magic_bracers[] = {{}, OfSpeed, OfProtection, Strenght, Dexterity, OpenLocks};
static variant magic_amulets[] = {{}, OfSpeed, OfProtection};
static variant magic_shield[] = {{}, ResistFire, ResistMagic};
static variant magic_helm[] = {{}, Intellegence};
static variant magic_armor[] = {{}, ResistCold, ResistFire, SaveVsPoison, ResistMagic};
static variant magic_robe[] = {{}, OfProtection, ResistCold, ResistMagic, ResistFire, SaveVsPoison};

static variant wand_spells[] = {MagicMissile, BurningHands, DetectMagic, Sleep, Mending};
static variant staff_spells[] = {MagicMissile, BurningHands, DetectMagic, Sleep, Mending};
static variant wizard_spells[] = {BurningHands, DetectMagic, FeatherFall, MageArmor, MagicMissile, Mending,
ReadLanguagesSpell, ShieldSpell, ShokingGrasp, Sleep,
Blur, Invisibility, Knock, ProduceFlame};
static variant priest_spells[] = {Bless, CureLightWounds, DetectEvil, ProtectionFromEvil, PurifyFood,
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
{"Staff", {8, 3, 1}, RightHand, {}, {TwoHanded}, {OneAttack, Bludgeon, -8, {1, 6}, {1, 4}}, {}, staff_spells},
{"Bastard sword", {45, 0, 1}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {Versatile}, {OneAttack, Slashing, -6, {2, 4}, {2, 8}}, {}, magic_weapon},
{"Longsword", {1, 0, 1}, RightHand, {UseLargeWeapon, UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -5, {1, 8}, {1, 12}}, {}, magic_weapon},
{"Short sword", {2, 0, 1}, RightHand, {UseTheifWeapon}, {Quick}, {OneAttack, Slashing, -3, {1, 6}, {1, 8}}, {}, magic_weapon},
{"Two-handed sword", {42, 0, 1}, RightHand, {UseLargeWeapon, UseMartialWeapon}, {TwoHanded}, {OneAttack, Slashing, -10, {1, 10}, {3, 6}}, {}, magic_weapon},
//
{"Bow", {10, 6, 1, Arrow}, RightHand, {UseTheifWeapon}, {TwoHanded, Ranged}, {TwoAttacks, Pierce, -8, {1, 8}, {1, 8}}},
{"Sling", {18, 4, 0, Stone}, RightHand, {}, {Ranged}, {OneAttack, Bludgeon, -6, {1, 4}, {1, 4}}},
//
{"Robe", {32, 8, 1}, Body, {UseArcane}, {}, {}, {}, magic_robe},
{"Leather armor", {31, 8, 1}, Body, {UseLeatherArmor}, {}, {}, {2}, magic_armor},
{"Studded leather armor", {31, 8, 1}, Body, {UseLeatherArmor}, {}, {}, {3}, magic_armor},
{"Scale mail", {30, 9, 1}, Body, {UseMetalArmor}, {}, {}, {4, 2}, magic_armor},
{"Chain mail", {29, 9, 1}, Body, {UseMetalArmor}, {}, {}, {5}, magic_armor},
{"Banded mail", {28, 9, 1}, Body, {UseMetalArmor}, {}, {}, {7, 3}, magic_armor},
{"Plate mail", {26, 9, 1}, Body, {UseMetalArmor}, {}, {}, {8, 4}, magic_armor},
//
{"Helm", {20, 6}, Head, {UseShield}, {}, {}, {0, 2}, magic_helm},
{"Shield", {23, 7, 1}, LeftHand, {UseShield}, {}, {}, {1, 2}, magic_shield},
{"Boots", {21, 9}, Legs, {}, {}, {}, {0, 1}, magic_boots},
//
{"Bracers", {25, 16}, Elbow, {}, {}, {}, {}, magic_bracers},
{"Necklage", {33, 13}, Neck, {}, {}, {}, {}, magic_amulets},
{"Necklage", {34, 13}, Neck, {}, {}, {}, {}, magic_amulets},
{"Jewelry", {108, 13}, Neck, {}, {}, {}, {}, magic_amulets},
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
{"Wand", {52, 10}, {}, {UseArcane}, {UseInHand, Charged}, {}, {}, wand_spells},
{"Scroll", {36, 12}, {}, {UseScrolls, UseArcane}, {}, {}, {}, wizard_spells},
{"Scroll", {85, 12}, {}, {UseScrolls, UseDivine}, {}, {}, {}, priest_spells},
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
{"Ring", {55, 15}, RightRing, {UseArcane}, {}, {}, {}, ring_red},
{"Ring", {78, 15}, RightRing, {}, {}, {}, {}, ring_blue},
{"Ring", {79, 15}, RightRing, {}, {}, {}, {}, ring_green},
// Potions
{"Potion", {39, 19}, {}, {}, {}, {}, {}, potion_red},
{"Potion", {40, 19}, {}, {}, {}, {}, {}, potion_blue},
{"Potion", {41, 19}, {}, {}, {}, {}, {}, potion_green},
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
{"Scepeter", {66, 17}, RightHand, {}, {Unique}, {OneAttack, Bludgeon, -5, {1, 6}, {1, 6}}, {}, magic_weapon},
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

static unsigned char find_power(const aref<variant>& source, variant v) {
	for(auto& e : source) {
		if(e == v)
			return source.indexof(&e);
	}
	return 0xFF;
}

void item::create(item_s type, variant power, int magic) {
	clear();
	auto pi = find_power(bsdata<itemi>::elements[type].enchantments, power);
	if(pi == 0xFF)
		return;
	if(magic == -1) {
		if(power.type == Ability && !bsdata<abilityi>::elements[power.value].multiplier) {
			// Nothing to do
		} else
			magic = special_magic_bonus();
	}
	this->type = type;
	this->subtype = pi;
	this->magic = magic;
	if(is(Charged))
		setcharges(dice::roll(3, 6));
}

void item::create(item_s type, int chance_magic, int chance_cursed, int chance_special) {
	clear();
	this->type = type;
	if(is(Charged))
		setcharges(dice::roll(3, 6));
	auto& ei = bsdata<itemi>::elements[type];
	auto isspecial = (ei.enchantments.count > 0);
	if(isspecial && ei.enchantments[0]) {
		subtype = rand() % ei.enchantments.count;
		if(d100() < chance_magic)
			magic = special_magic_bonus();
	} else if(d100() < chance_magic) {
		if(isspecial && chance_special < d100()) {
			subtype = 1 + (rand() % ei.enchantments.count - 1);
			magic = special_magic_bonus();
		} else
			magic = standart_magic_bonus();
	}
	if(d100() < chance_cursed)
		setcursed(1);
}

void item::clear() {
	memset(this, 0, sizeof(*this));
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
	return (getpower() == value) ? getmagic() : 0;
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

static void add_power(stringbuilder& sb, const char* name, const char** names, int bonus, int multiplier) {
	const char* p = 0;
	auto need_plus = true;
	if(names && bonus >= 0 && bonus <= 4 && names[bonus]) {
		need_plus = false;
		p = names[bonus];
	} else if(names && names[0])
		p = names[0];
	else
		p = name;
	if(!p)
		return;
	sb.add(" of %1", p);
	if(need_plus && bonus && multiplier)
		sb.adds("%+1i", bonus);
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
		auto magic = getmagic();
		auto power = getpower();
		switch(power.type) {
		case Spell:
			add_power(sc, bsdata<spelli>::elements[power.value].name,
				0, 0,
				0);
			break;
		case Enchant:
			add_power(sc, 0,
				bsdata<enchanti>::elements[power.value].names, magic,
				bsdata<enchanti>::elements[power.value].multiplier);
			break;
		case Ability:
			add_power(sc, bsdata<abilityi>::elements[power.value].name,
				bsdata<abilityi>::elements[power.value].nameof, magic,
				bsdata<abilityi>::elements[power.value].multiplier);
			break;
		default:
			if(magic)
				sc.adds("%+1i", magic);
			break;
		}
	}
}

bool item::ismelee() const {
	return (type >= AxeBattle && type <= SwordTwoHanded)
		|| (type >= Slam && type <= LastItem);
}

int	item::getmagic() const {
	auto r = magic;
	if(cursed != 0)
		return -(r + 1);
	auto& ei = bsdata<itemi>::elements[type];
	if(ei.enchantments.count > 0) {
		if(!ei.enchantments.data[0]) {
			if(subtype)
				r++;
		} else
			r++;
	}
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
	return ei.enchantments.data[subtype];
}

bool item::cast(creature* caster, spell_s id, int level, bool interactive, bool run) {
	switch(id) {
	case DetectMagic:
		if(!ismagical())
			return false;
		break;
	case DetectEvil:
		if(!iscursed())
			return false;
		break;
	case Identify:
		if(isidentified() || !ismagical())
			return false;
		if(run)
			identified = true;
		break;
	case Mending:
		if(!isbroken() || type==Ration || type==RationIron)
			return false;
		if(run)
			broken = false;
		break;
	case PurifyFood:
		if(!isbroken() || (type != Ration && type != RationIron))
			return false;
		if(run)
			broken = false;
		break;
	default:
		return false;
	}
	return true;
}