#include "main.h"
#include "archive.h"

INSTDATAC(creature, 32)

const char* get_name_part(short rec);

static const int monsters_thac0[] = {
	0, 1, 1, 3, 3, 5, 5, 7, 7, 9,
	9, 11, 11, 13, 13, 15, 15, 17, 17, 19
};
static char hit_probability[] = {
	-5, -5, -3, -3, -2, -2, -1, -1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 2, 2, 2, 3,
	3, 4, 4, 5, 6, 7
};
static char damage_adjustment[] = {
	-5, -5, -3, -3, -2, -2, -1, -1, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 2,
	3, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 14
};
char reaction_adjustment[] = {
	-7, -6, -4, -3, -2, -1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 2, 3, 3,
	4, 4, 4, 5, 5
};
static char defence_adjustment[] = {
	-5, -5, -4, -3, -2, -1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 3, 4, 4,
	4, 5, 5, 5, 6, 6
};
static char hit_points_adjustment[] = {-4,
-3, -2, -2, -1, -1, -1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 2, 3, 4, 5,
5, 6, 6, 6, 7, 7
};
static char wisdow_bonus_spells[][7] = {{1, 0, 0, 0, 0, 0, 0}, // Wisdow 13
{2, 0, 0, 0, 0, 0, 0}, // Wisdow 14
{2, 1, 0, 0, 0, 0, 0}, // Wisdow 15
{2, 2, 0, 0, 0, 0, 0}, // Wisdow 16
{2, 2, 1, 0, 0, 0, 0}, // Wisdow 17
{2, 2, 1, 1, 0, 0, 0}, // Wisdow 18
{3, 2, 2, 1, 0, 0, 0}, // Wisdow 19
{3, 3, 2, 2, 0, 0, 0}, // Wisdow 20
{3, 3, 3, 2, 1, 0, 0}, // Wisdow 21
{3, 3, 3, 3, 2, 0, 0}, // Wisdow 22
{4, 3, 3, 3, 2, 1, 0}, // Wisdow 23
{4, 3, 3, 3, 3, 2, 0}, // Wisdow 24
{4, 3, 3, 3, 3, 3, 1}, // Wisdow 25
};
static char charisma_reaction_bonus[] = {-8,
-7, -6, -5, -4, -3, -2, -1, 0, 0, 0,
0, 0, 1, 2, 3, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14
};
static char thac0_monster[] = {
	0, 1, 1, 3, 3, 5, 5, 7, 7, 9, 9, 11, 11, 13, 13, 15, 15, 17, 17
};
static char thac0_warrior[] = {
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
};
static char thac0_priest[] = {
	0, 0, 0, 0, 2, 2, 2, 4, 4, 4,
	6, 6, 6, 8, 8, 8, 10, 10, 10, 12, 12
};
static char thac0_rogue[] = {
	0, 0, 0, 1, 1, 2, 2, 3, 3, 4,
	4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9
};
static char thac0_wizard[] = {
	0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
	3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6
};
static char dwarven_constitution_bonuses[] = {
	0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
	2, 3, 3, 3, 4, 4, 4, 4, 5
};
static int experience_warrior[21] = {
	0, 0, 2000, 4000, 8000, 16000, 32000, 64000, 125000, 250000,
	500000, 750000, 1000000, 1250000, 1500000, 1750000, 2000000, 2250000, 2500000, 2750000, 3000000
};
static int experience_wizard[21] = {
	0, 0, 2500, 5000, 10000, 20000, 40000, 60000, 90000, 135000,
	250000, 375000, 750000, 1125000, 1500000, 1875000, 2250000, 2625000, 3000000, 3375000, 3750000
};
static int experience_priest[21] = {
	0, 0, 1500, 3000, 6000, 13000, 27500, 55000, 110000, 225000,
	450000, 675000, 900000, 1125000, 1350000, 1575000, 1800000, 2025000, 2250000, 2475000, 2700000
};
static int experience_rogue[21] = {
	0, 0, 1250, 2500, 5000, 10000, 20000, 40000, 70000, 110000,
	160000, 220000, 440000, 660000, 880000, 1100000, 1320000, 1540000, 1760000, 1980000, 2200000
};

static int* get_experience_table(class_s cls) {
	switch(cls) {
	case Cleric: return experience_priest;
	case Fighter: case Paladin: case Ranger: return experience_warrior;
	case Mage: return experience_wizard;
	default: return experience_rogue;
	}
}

// Make save vs posoin and/or get damage
void creature::update_poison(bool interactive) {
	auto hits = 0;
	if(is(SlowPoison))
		return;
	if(is(Poison)) {
		if(roll(SaveVsPoison, 0)) {
			hits += xrand(1, 3);
			remove(Poison);
		} else
			hits += xrand(1, 8);
	}
	if(hits > 0) {
		if(interactive) {
			char temp[260];
			mslog("%1 feel poison", getname(temp, zendof(temp)));
		}
		draw::animation::update();
		damage(Death, hits);
	}
}

bool creature::add(spell_s type, unsigned duration, save_s save, char save_bonus, ability_s save_type) {
	if(is(type))
		return true;
	switch(type) {
	case HoldPerson:
	case Sleep:
		// Elf has 90% immunity to paralization, sleep and charm
		if(roll(ResistCharm))
			return false;
		break;
	}
	if(save != NoSave) {
		switch(type) {
		case HoldPerson:
			save_type = SaveVsParalization;
			break;
		case Poison:
		case Disease:
			save_type = SaveVsPoison;
			break;
		}
		if(roll(save_type)) {
			switch(save) {
			case SaveNegate:
				return false;
			case SaveHalf:
				duration = duration / 2;
				if(!duration)
					return false;
				break;
			}
		}
	}
	active_spells.set(type);
	addboost(type, duration);
	return true;
}

int creature::gethd() const {
	if(kind)
		return bsdata<monsteri>::elements[kind].hd[0];
	return levels[0];
}

int	creature::getawards() const {
	if(kind)
		return bsdata<monsteri>::elements[kind].getexperience();
	return gethd() * 100;
}

size_s creature::getsize() const {
	if(kind)
		return bsdata<monsteri>::elements[kind].size;
	return Medium;
}

resource_s creature::getres() const {
	if(kind)
		return bsdata<monsteri>::elements[kind].rfile;
	return NONE;
}

static int getweapon(wear_s weapon) {
	switch(weapon) {
	case LeftHand: return 1;
	case Head: return 2;
	default: return 0;
	}
}

void creature::get(combati& result, wear_s weapon, creature* enemy) const {
	result.attack = OneAttack;
	auto hd = gethd();
	auto t = getbestclass();
	auto k = getstrex();
	if(kind)
		result.bonus += maptbl(monsters_thac0, (int)bsdata<monsteri>::elements[kind].hd[0]);
	else
		result.bonus += getthac0(t, get(t));
	result.bonus += maptbl(hit_probability, k);
	if(wears[weapon]) {
		auto& wi = bsdata<itemi>::elements[wears[weapon].gettype()].weapon;
		wears[weapon].get(result, enemy);
		if(enemy) {
			// RULE: holy weapon do more damage to undead
			if(enemy->is(Undead)) {
				auto b = wears[weapon].get(OfHolyness);
				result.bonus += b;
				result.damage.b += b * 2;
			}
		}
		result.weapon = const_cast<item*>(&wears[weapon]);
		if(result.weapon->is(Natural))
			result.damage.b += wi.damage_large.b * hd;
	} else
		result.damage = {1, 2};
	auto race = getrace();
	if(is(BonusVsElfWeapon) && wears[weapon].is(UseTheifWeapon))
		result.bonus++;
	// Weapon secialist get bonus to hit (only to main hand?)
	if(getspecialist(wears[weapon].gettype())) {
		result.attack = OneAndTwoAttacks;
		result.bonus++;
	}
	if(is(Haste))
		result.bonus += 2;
	if(is(Bless))
		result.bonus += 1;
	if(is(Fear))
		result.bonus -= 4;
	if(is(Blindness))
		result.bonus -= 4;
	result.bonus -= drain_energy;
	result.damage.b += maptbl(damage_adjustment, k);
	result.damage.b -= drain_energy;
}

void creature::add(item value) {
	for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i + 1)) {
		auto pi = getitem(i);
		if(!pi || *pi)
			continue;
		*pi = value;
		break;
	}
}

item creature::get(wear_s id) const {
	auto pi = const_cast<creature*>(this)->getitem(id);
	if(!pi)
		return NoItem;
	return *pi;
}

bool creature::roll(ability_s id, int bonus) const {
	auto n = get(id);
	if(id <= LastAbility)
		n *= 5;
	n += bonus * 5;
	if(n <= 0)
		return false;
	auto d = d100();
	return d <= n;
}

bool creature::isinvisible() const {
	return is(Invisibility) || getenchant(Invisibility, 1);
}

bool creature::isready() const {
	return gethits() > 0
		&& !is(HoldPerson)
		&& !is(Sleep);
}

void creature::update_hour(bool interactive) {
	if(is(Disease)) {
		if(roll(SaveVsPoison))
			disease_progress--;
		else
			disease_progress++;
		if(disease_progress <= 0) {
			disease_progress = 0;
			remove(Disease);
		}
	}
}

void creature::update_turn(bool interactive) {
	// Poison effect
	update_poison(interactive);
	// Regeneration effect
	for(auto slot = Head; slot <= Legs; slot = (wear_s)(slot + 1)) {
		auto pi = getitem(slot);
		if(pi && *pi) {
			auto power = pi->getpower();
			if(power.type == Enchant) {
				auto magic = pi->getmagic();
				switch(power.value) {
				case OfRegeneration: damage(Heal, magic); break;
				}
			}
		}
	}
	// Every turn heroes use food
	if(ishero())
		subenergy();
}

void creature::update(bool interactive) {
	if(is(AcidArrow))
		damage(Magic, dice::roll(2, 4), 5);
	remove(Moved);
	if(!kind)
		update_levelup(interactive);
}

bool creature::isenemy(creature* target) const {
	return ishero() != target->ishero();
}

void creature::say(const char* format, ...) {
	return sayv(format, xva_start(format));
}

void creature::sayv(const char* format, const char* vl) {
	char name[64]; getname(name, zendof(name));
	char message[1024]; szprintv(message, zendof(message), format, vl);
	mslog("%1 \"%2\"", name, message);
}

bool creature::canspeak(race_s language) const {
	if(language == Human)
		return true;
	if(is(ReadLanguagesSpell))
		return true;
	auto race = getrace();
	if(race == language)
		return true;
	return false;
}

void creature::equip(item it) {
	static const wear_s slots[] = {Head, Neck, Body, RightRing, LeftRing, Elbow, Legs, Quiver, RightHand, LeftHand};
	for(auto slot : slots) {
		auto pi = getitem(slot);
		if(!pi || *pi)
			continue;
		if(isallow(it, slot)) {
			*pi = it;
			return;
		}
	}
	add(it);
}

int	creature::getbonus(variant id, wear_s slot) const {
	if(kind) {
		if(slot == RightHand && bsdata<monsteri>::elements[type].is(id))
			return 2;
		return 0;
	}
	return wears[slot].get(id);
}

int creature::gethitpenalty(int bonus) const {
	if(is(Ambidextrity))
		return 0;
	auto dex = get(Dexterity);
	auto bon = maptbl(reaction_adjustment, dex);
	bonus += bon;
	if(bonus > 0)
		bonus = 0;
	return bonus;
}

void creature::subenergy() {
	if(food > 0)
		food--;
}

void creature::attack_drain(creature* defender, char& value, int& hits) {
	if(is(NegativePlanProtection) && roll(SaveVsParalization)) {
		hits = 0;
		damage(Magic, dice::roll(2, 6));
		defender->remove(NegativePlanProtection);
	} else
		value++;
	// Dead from draining
	if(defender->drain_energy >= defender->gethd()
		|| get(Strenght) <= 1
		|| get(Constitution) <= 1)
		hits = defender->gethits() + 10;
}

static int getresisted(int value, int percent) {
	if(!percent)
		return value;
	return value * (100 - percent) / 100;
}

void creature::attack(creature* defender, wear_s slot, int bonus) {
	combati wi = {}; get(wi, slot, defender);
	auto ac = defender->getac();
	// RULE: invisible characters hard to hit and more likely to hit
	if(isinvisible())
		wi.bonus += 4;
	// RULE: Dwarf can hit goblinoid by 5% better that others
	if(is(BonusToHitVsGoblinoid) && defender->race == Goblinoid)
		wi.bonus += 1;
	// RULE: Ranger add +4 THAC0 when fight humanoid and goblonoids
	if(is(BonusDamageVsEnemy) && (defender->race == Humanoid || defender->race == Goblinoid))
		wi.bonus += 4;
	auto magic_bonus = 0;
	if(wi.weapon)
		magic_bonus = wi.weapon->getmagic();
	if(ishero())
		subenergy();
	for(auto atn = (bsdata<attacki>::elements[wi.attack].attacks_p2r + (game.getrounds() % 2)) / 2; atn > 0; atn--) {
		auto tohit = 20 - (wi.bonus + bonus) - (10 - ac);
		auto rolls = xrand(1, 20);
		auto hits = -1;
		auto crhit = 20 - wi.critical_range;
		auto damage_type = wi.type;
		tohit = imax(2, imin(20, tohit));
		if(rolls >= tohit || rolls >= crhit) {
			auto damage = wi.damage;
			hits = damage.roll();
			if(getbonus(Fire, slot)) {
				damage_type = Fire;
				hits += xrand(1, 6);
			}
			if(getbonus(Cold, slot)) {
				damage_type = Cold;
				hits += xrand(1, 4) + 1;
			}
			// RULE: crtitical hit can deflected
			if(rolls >= crhit) {
				// RULE: critical damage depends on weapon and count in dices
				if(!defender->roll(CriticalDeflect)) {
					damage.b = 0; // Only initial dice rolled second time
					damage.c += wi.critical_multiplier;
					hits += damage.roll();
				}
				if(wi.weapon) {
					// RULE: Staff and rods magic use this
					auto power = wi.weapon->getpower();
					if(power.type == Spell) {
						auto spell = (spell_s)power.value;
						if(bsdata<spelli>::elements[spell].effect.proc == effecti::apply_damage)
							cast(spell, Mage, wi.weapon->getmagic(), defender);
						else
							cast(spell, Mage, wi.weapon->getmagic(), this);
					}
				}
			}
			// RULE: vampiric ability allow user to drain blood and regain own HP
			auto vampirism = getbonus(OfVampirism, slot);
			if(vampirism) {
				auto hits_healed = xrand(1, 4) + vampirism;
				if(hits_healed > hits)
					hits_healed = hits;
				this->damage(Heal, hits_healed);
			}
		}
		// Fear attack (Not depend on attack result)
		if(getbonus(OfFear, slot))
			defender->add(Fear, xrand(1, 3) * 10, SaveNegate);
		// When attacking sleeping creature she wake up!
		defender->remove(Sleep);
		// Show result
		draw::animation::attack(this, slot, hits);
		if(hits != -1) {
			// Poison attack
			if(getbonus(OfPoison, slot))
				defender->add(Poison, Instant, SaveNegate);
			// Paralize attack
			if(getbonus(OfParalize, slot))
				defender->add(HoldPerson, xrand(1, 3), SaveNegate);
			// Drain ability
			if(getbonus(OfEnergyDrain, slot))
				attack_drain(defender, defender->drain_energy, hits);
			if(getbonus(OfStrenghtDrain, slot))
				attack_drain(defender, defender->drain_strenght, hits);
			defender->damage(damage_type, hits, magic_bonus);
			// If weapon have charges waste it
			if(wi.weapon) {
				if(wi.weapon->is(Charged))
					wi.weapon->setcharges(wi.weapon->getcharges() - 1);
			}
		} else
			draw::animation::render();
		draw::animation::update();
		draw::animation::clear();
		// Weapon can be broken
		if(rolls == 1 && wi.weapon) {
			if(wi.weapon->damage(0, 0))
				usequick();
			return;
		}
	}
}

void creature::attack(short unsigned index, direction_s d, int bonus, bool ranged) {
	auto defender = game.getdefender(index, d, this);
	if(!defender)
		return;
	auto wp1 = get(RightHand);
	auto wp2 = get(LeftHand);
	auto wp3 = get(Head);
	if(wp1.istwohanded() || !wp2.ismelee())
		wp2 = NoItem;
	if(!wp3.ismelee())
		wp3 = NoItem;
	if(ranged && !wp1.isranged())
		return;
	if(wp1.isranged())
		attack(defender, RightHand, bonus);
	else {
		if(wp2) {
			attack(defender, RightHand, bonus + gethitpenalty(-4));
			attack(defender, LeftHand, bonus + gethitpenalty(-6));
		} else
			attack(defender, RightHand, bonus);
		if(wp3)
			attack(defender, Head, bonus);
	}
}

void creature::addexp(int value) {
	// RULE: ring of Advise
	auto b = getbonus(OfAdvise);
	if(b)
		value += value * b / 20;
	// RULE: if class ability if hight enought you gain additional 10% experience
	auto pa = bsdata<classi>::elements[type].ability;
	if(get(pa) >= 16)
		value += value / 10;
	experience += value;
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::finish() {
	hits_rolled = 0;
	memset(levels, 0, sizeof(levels));
	memset(spells, 0, sizeof(spells));
	memset(prepared, 0, sizeof(prepared));
	known_spells.clear();
	active_spells.clear();
	feats.add(bsdata<racei>::elements[race].feats);
	feats.add(bsdata<classi>::elements[type].feats);
	usability.add(bsdata<racei>::elements[race].usability);
	usability.add(bsdata<classi>::elements[type].usability);
	if(kind)
		hits_rolled = gethitdice().roll();
	else {
		for(int i = 0; i < 3; i++) {
			auto c = getclass(getclass(), i);
			if(!c)
				continue;
			raise_level(c);
		}
	}
	hits = gethitsmaximum();
	food = getfoodmax();
	if(is(NoExeptionalStrenght))
		str_exeptional = 0;
}

int get_cleric_spell_level(const spellprogi* pr, int hd) {
	if(!pr)
		return 0;
	auto p = pr->elements[hd];
	auto result = 0;
	for(auto i = 1; i <= 9; i++) {
		if(p[i])
			result = i;
	}
	return result;
}

void creature::raise_level(class_s type) {
	auto level = get(type);
	if(getclass(this->type, 0) == type)
		levels[0] = level + 1;
	else if(getclass(this->type, 1) == type)
		levels[1] = level + 1;
	else if(getclass(this->type, 2) == type)
		levels[2] = level + 1;
	int hd = bsdata<classi>::elements[type].hd;
	if(!hd)
		return;
	auto hp = 1 + (rand() % hd);
	if(level == 0) {
		hp = hd;
		if(type == Mage) {
			setknown(DetectMagic);
			random_spells(type, 1, 6);
			prepare_random_spells(type, 1);
			preparespells();
		} else if(type == Cleric) {
			random_spells(type, 1, 32);
			prepare_random_spells(type, 1);
			preparespells();
		} else if(type == Paladin)
			preparespells();
	} else {
		if(type == Cleric) {
			auto spell_level = get_cleric_spell_level(getprogress(Cleric), level + 1);
			if(spell_level)
				random_spells(type, spell_level, 32);
		}
	}
	hits_rolled += hp;
}

void creature::random_equipment() {
	if(kind)
		return;
	auto cls = getbestclass();
	auto race = getrace();
	switch(cls) {
	case Fighter:
	case Paladin:
		switch(race) {
		case Dwarf:
			wears[RightHand] = item(AxeBattle);
			wears[LeftHand] = item(Shield);
			break;
		case Halfling:
			wears[RightHand] = item(SwordShort);
			break;
		default:
			wears[RightHand] = item(SwordLong);
			break;
		}
		wears[Body] = item(ArmorStuddedLeather);
		break;
	case Ranger:
		wears[RightHand] = item(SwordLong);
		wears[LeftHand] = item(SwordShort);
		wears[Body] = item(ArmorLeather);
		break;
	case Cleric:
		wears[RightHand] = item(Mace);
		wears[Body] = item(ArmorLeather);
		break;
	case Mage:
		wears[RightHand] = item(Dagger);
		wears[Body] = item(Robe);
		break;
	default:
		wears[RightHand] = item(Dagger);
		wears[Body] = item(ArmorLeather);
		break;
	}
	add(RationIron);
	if(get(Mage))
		equip(MagicBook);
	if(get(Cleric))
		equip(HolySymbol);
	if(get(Paladin))
		equip(HolyWarriorSymbol);
	if(get(Theif))
		add(TheifTools);
	if(get(Fighter))
		add(Ration);
}

void creature::prepare_random_spells(class_s type, int level) {
	adat<spell_s, 64> spells;
	int maximum_spells = getspellsperlevel(type, level);
	int prepared_spells = 0;
	for(auto i = spell_s(1); i < FirstSpellAbility; i = (spell_s)(i + 1)) {
		if(!isknown(i))
			continue;
		if(getlevel(i, type) == level) {
			prepared_spells += get(i);
			spells.add(i);
		}
	}
	zshuffle(spells.data, spells.count);
	if(spells.count > (unsigned)maximum_spells)
		spells.count = maximum_spells;
	for(auto e : spells)
		setprepare(e, 1);
}

void creature::random_spells(class_s type, int level, int count) {
	adat<spell_s, 64> spells;
	spells.count = select(spells.data, zendof(spells.data), type, level);
	zshuffle(spells.data, spells.count);
	for(auto e : spells) {
		if(count-- <= 0)
			break;
		if(isknown(e))
			continue;
		if(type == Mage && !roll(LearnSpell))
			continue;
		setknown(e);
	}
}

int	creature::get(class_s type) const {
	auto& ci = bsdata<classi>::elements[this->type].classes;
	for(unsigned i = 0; i < ci.count; i++) {
		if(ci.data[i] == type)
			return levels[i];
	}
	return 0;
}

int creature::getside() const {
	if(kind)
		return side;
	if(party[0] == this)
		return 0;
	else if(party[1] == this)
		return 1;
	else if(party[2] == this)
		return 2;
	return 3;
}

short creature::gethitsmaximum() const {
	if(kind)
		return hits_rolled + bsdata<monsteri>::elements[type].hd[1];
	auto hd = gethd();
	auto rl = (int)hits_rolled / getclasscount();
	auto cs = get(Constitution);
	auto md = maptbl(hit_points_adjustment, cs);
	if(md > 2 && !is(BonusHP))
		md = 2;
	rl += md * hd;
	if(rl < hd)
		rl = hd;
	return rl;
}

int creature::getspeed() const {
	auto r = 0;
	auto k = get(Dexterity);
	r += maptbl(reaction_adjustment, k);
	r += wears[RightHand].getspeed();
	r += wears[LeftHand].getspeed();
	r += getbonus(OfSpeed);
	if(is(Haste))
		r += 2;
	if(is(Blindness))
		r -= 2;
	if(is(Deafness))
		r -= 1;
	return r;
}

int creature::getac() const {
	auto r = 0;
	auto k = get(Dexterity);
	r += maptbl(defence_adjustment, k);
	r += wears[Head].getac();
	r += wears[Body].getac();
	r += wears[RightHand].getac();
	r += wears[LeftHand].getac();
	r += getbonus(OfProtection);
	if(kind)
		r += (10 - bsdata<monsteri>::elements[kind].ac);
	if(is(Haste))
		r += 2;
	if(is(MageArmor))
		r += 4;
	if(is(ShieldSpell))
		r += 7;
	if(is(Blindness))
		r -= 4;
	// One of this
	if(isinvisible())
		r += 4;
	else if(is(Blur))
		r += 3;
	return r;
}

int creature::getclasscount() const {
	auto r = bsdata<classi>::elements[type].classes.count;
	return r ? r : 1;
}

void creature::update_levelup(bool interactive) {
	auto exp = getexperience() / getclasscount();
	for(auto i = Cleric; i <= Theif; i = (class_s)(i + 1)) {
		auto clv = get(i);
		if(!clv)
			continue;
		auto tbl = get_experience_table(i);
		if(exp >= tbl[clv + 1]) {
			char temp[64];
			raise_level(i);
			mslog("%1 gain level of experience", getname(temp, zendof(temp)));
		}
	}
}

int	creature::getthac0(class_s cls, int level) const {
	switch(cls) {
	case NoClass: return maptbl(thac0_monster, level);
	case Fighter:
	case Paladin:
	case Ranger: return maptbl(thac0_warrior, level);
	case Theif: return maptbl(thac0_rogue, level);
	case Cleric: return maptbl(thac0_priest, level);
	default: return maptbl(thac0_wizard, level);
	}
}

void creature::random_name() {
	if(kind)
		return;
	auto race = getrace();
	auto gender = getgender();
	auto need_correct = true;
	if(race == HalfElf) {
		// RULE: Half-elves get names mostly elvish than humans
		if(d100() < 60)
			race = Elf;
		else
			race = Human;
	}
	if(race == Dwarf)
		need_correct = false;
	auto fe = game.getrandom(0, race, gender, -1);
	if(!fe)
		fe = game.getrandom(0, NoRace, gender, -1);
	auto se = game.getrandom(1, race, gender, need_correct ? fe : -1);
	if(!se)
		se = game.getrandom(1, NoRace, gender, need_correct ? fe : -1);
	name[0] = fe;
	name[1] = se;
}

int creature::get_base_save_throw(ability_s st) const {
	auto c = getclass();
	auto rc = getrace();
	auto b1 = getclass(c, 0);
	auto b2 = getclass(c, 1);
	auto b3 = getclass(c, 2);
	auto con = get(Constitution);
	auto n = gethd();
	auto r = 16 - n;
	if(r < 5)
		r = 5;
	if(b1 == Fighter || b2 == Fighter)
		r -= 1;
	if(b1 == Paladin)
		r -= 3;
	if(b1 == Cleric || b2 == Cleric) {
		if(st == SaveVsPoison || st == SaveVsParalization)
			r -= 2;
	}
	if(b1 == Mage || b2 == Mage) {
		if(st == SaveVsMagic)
			r -= 2;
	}
	if(b1 == Theif || b2 == Theif || b3 == Theif) {
		if(st == SaveVsTraps)
			r -= 2;
	}
	if(rc == Dwarf || rc == Halfling) {
		switch(st) {
		case SaveVsPoison:
		case SaveVsMagic:
			r -= maptbl(dwarven_constitution_bonuses, con);
			break;
		}
	}
	if(is(ProtectionFromEvil))
		r--;
	return (21 - r) * 5;
}

void creature::preparespells() {
	for(auto i = spell_s(1); i <= LastSpellAbility; i = (spell_s)(i + 1)) {
		auto level = getlevel(i, Mage);
		auto count = getprepare(i);
		if((wears[LeftRing].get(OfWizardy) && wears[LeftRing].get(OfWizardy) == level)
			|| (wears[RightRing].get(OfWizardy) && wears[RightRing].get(OfWizardy) == level))
			count *= 2;
		set(i, count);
	}
	if(get(Cleric))
		set(TurnUndead, 2);
	if(get(Paladin)) {
		set(DetectEvil, 2);
		set(LayOnHands, 1);
		if(get(Paladin) >= 3)
			set(TurnUndead, 1);
		if(get(Paladin) >= 5)
			set(CureDisease, 1);
	}
}

int creature::getspellsperlevel(class_s cls, int spell_level) const {
	auto pr = getprogress(cls);
	if(!pr)
		return 0;
	auto level = get(cls);
	if(level > 20)
		level = 20;
	int b = pr->elements[level][spell_level];
	if(spell_level > 9)
		spell_level = 9;
	if(cls == Cleric && b) {
		if(spell_level > 7)
			spell_level = 7;
		int wisdow = get(Wisdow);
		if(wisdow >= 13)
			b += maptbl(wisdow_bonus_spells, wisdow - 13)[spell_level - 1];
	}
	return b;
}

void creature::getname(stringbuilder& sb) const {
	if(kind)
		sb.add(bsdata<monsteri>::elements[kind].name);
	else
		sb.add("%1%2", get_name_part(name[0]), get_name_part(name[1]));
}

const char* creature::getname(char* result, const char* result_maximum) const {
	if(kind)
		return bsdata<monsteri>::elements[kind].name;
	szprint(result, result_maximum, "%1%2",
		get_name_part(name[0]), get_name_part(name[1]));
	return result;
}

int creature::getenchant(variant id, int bonus) const {
	if(id.type == Enchant) {
		if(bsdata<monsteri>::elements[kind].is((enchant_s)id.value))
			return bonus;
	}
	// All bonuses no stack each other
	static wear_s slots[] = {Head, Neck, Body, RightRing, LeftRing, Elbow, Legs};
	for(auto s : slots) {
		if(!wears[s])
			continue;
		auto pe = wears[s].getenchantment();
		if(pe->power == id) {
			if(wears[s].iscursed())
				return -bonus;
			return bonus;
		}
	}
	return 0;
}

int creature::getbonus(variant id) const {
	if(id.type == Enchant) {
		if(bsdata<monsteri>::elements[kind].is((enchant_s)id.value))
			return 2; // All monsters have enchantment of 2
	}
	// All bonuses no stack each other
	static wear_s slots[] = {Head, Neck, Body, RightRing, LeftRing, Elbow, Legs};
	auto r = 0;
	for(auto s : slots) {
		auto v = wears[s].get(id);
		if(v > r)
			r = v;
	}
	return r;
}

char creature::racial_bonus(char* data) const {
	return data[getrace()];
}

int creature::getspecialist(item_s type) const {
	auto cls = getclass();
	if(cls != Fighter)
		return 0;
	switch(getrace()) {
	case Dwarf: return (type == AxeBattle || type == Mace || type == HammerWar) ? 1 : 0;
	case HalfElf: case Elf: return (type == SwordLong || type == SwordShort || type == Bow) ? 1 : 0;
	case Halfling: return (type == SwordShort) ? 1 : 0;
	default: return (type == SwordLong || type == SwordBastard || type == SwordTwoHanded) ? 1 : 0;
	}
}

void creature::setframe(short* frames, short index) const {
	if(kind) {
		auto po = bsdata<monsteri>::elements[type].overlays;
		frames[0] = po[0] * 6 + index;
		frames[1] = po[1] ? po[1] * 6 + index : 0;
		frames[2] = po[2] ? po[2] * 6 + index : 0;
		frames[3] = po[3] ? po[3] * 6 + index : 0;
	} else {
		frames[0] = index;
		frames[1] = 0;
	}
}

direction_s creature::getdirection() const {
	if(kind)
		return direction;
	return game.getdirection();
}

short unsigned creature::getindex() const {
	if(kind)
		return index;
	return game.getcamera();
}

bool creature::have(item_s v) const {
	for(auto& e : wears) {
		if(e && e.gettype() == v)
			return true;
	}
	return false;
}

bool creature::have(aref<class_s> source) const {
	for(auto e : source) {
		if(get(e))
			return true;
	}
	return false;
}

bool creature::isuse(const item v) const {
	return bsdata<itemi>::elements[v.gettype()].usability.is(usability);
}

void creature::set(direction_s v) {
	if(kind)
		direction = v;
	else
		game.setcamera(getindex(), v);
}

void creature::setside(int value) {
	if(kind)
		side = value;
}

bool creature::isallow(class_s id, race_s r) {
	return bsdata<classi>::elements[id].races.is(r);
}

bool creature::isallow(alignment_s id, class_s c) {
	return !bsdata<alignmenti>::elements[id].restricted.is(c);
}

class_s creature::getclass(class_s id, int pos) {
	auto& e = bsdata<classi>::elements[id].classes;
	if(e.count > (unsigned)pos)
		return e.data[pos];
	return NoClass;
}

bool creature::isallow(const item it, wear_s slot) const {
	if(!it)
		return true;
	if(slot >= Backpack && slot <= LastBackpack)
		return true;
	auto w = it.getwear();
	if(slot >= FirstBelt && slot <= LastBelt) {
		if(w == Body || w == Head || w == Neck || w == RightRing || w == Elbow)
			return false;
		return true;
	}
	if(slot == LeftHand) {
		// RULE: two-handed items cannot be equiped in left hand
		if(it.istwohanded())
			return false;
		// RULE: no items can be used in left hand when wear two handed item
		auto rh = get(RightHand);
		if(rh.istwohanded())
			return false;
		return isuse(it);
	} else if(slot == RightHand)
		return true;
	if(!isuse(it))
		return false;
	if(slot == LeftRing)
		slot = RightRing;
	return w == slot;
}

static void remove_hits(short& bonus, int v) {
	if(bonus) {
		if(bonus >= v) {
			bonus -= v;
			v = 0;
		} else {
			v -= bonus;
			bonus = 0;
		}
	}
}

void creature::damage(damage_s type, int hits, int magic_bonus) {
	if(type == Heal)
		hits = -hits;
	if(hits < 0) {
		auto c = this->hits - hits;
		auto m = gethitsmaximum();
		if(c > m)
			c = m;
		sethits(c);
		return;
	}
	if(type == Bludgeon && is(ResistBludgeon))
		hits /= 2;
	if(type == Slashing && is(ResistSlashing))
		hits = (hits + 1) / 2;
	if(type == Pierce && is(ResistPierce))
		hits /= 2;
	if(type == Cold)
		hits = getresisted(hits, get(ResistCold));
	if(type == Fire)
		hits = getresisted(hits, get(ResistFire));
	if((magic_bonus == 0) && is(ImmuneNormalWeapon)
		&& (type == Bludgeon || type == Slashing || type == Pierce))
		hits = 0;
	if(hits == 0)
		return;
	auto h = hits;
	remove_hits(hits_aid, h);
	auto c = this->hits - h;
	sethits(c);
	draw::animation::damage(this, hits);
	// If we kill enemy monster
	if(gethits() <= 0)
		kill();
}

void creature::kill() {
	if(ishero())
		return;
	// Add experience
	auto hitd = gethd();
	auto value = getawards();
	addexp(value, hitd);
	// Drop items
	auto index = getindex();
	auto side = getside();
	for(auto par = FirstInvertory; par <= LastInvertory; par = (wear_s)(par + 1)) {
		auto it = wears[par];
		if(!it || it.is(Natural))
			continue;
		if(it.is(Unique) || (d100() < 15)) {
			it.setidentified(0);
			location.dropitem(index, it, side);
		}
	}
	game.add(kind);
	clear();
}

void creature::addexp(int value, int killing_hit_dice) {
	int count = 0;
	for(auto v : party) {
		auto pc = v.getcreature();
		if(pc && pc->isready())
			count++;
	}
	if(count) {
		int value_per_member = imax(1, value / count);
		for(auto v : party) {
			auto pc = v.getcreature();
			if(pc && pc->isready()) {
				pc->addexp(value_per_member);
				if(killing_hit_dice) {
					if(pc->get(Fighter) || pc->get(Paladin) || pc->get(Ranger))
						pc->addexp(10 * killing_hit_dice);
				}
			}
		}
	}
}

bool creature::use(ability_s skill, short unsigned index, int bonus, bool* firsttime, int exp, bool interactive) {
	if(firsttime)
		*firsttime = false;
	if(get(skill) <= 0)
		return false;
	if(skill == HearNoise && !set(skill, index)) {
		if(firsttime)
			*firsttime = true;
		return false;
	}
	if(roll(skill, bonus)) {
		if(exp)
			addexp(exp);
		return true;
	} else {
		if(interactive)
			mslog("You are failed");
	}
	return false;
}

bool creature::swap(item* itm1, item* itm2) {
	static const char* dontwear[2] = {"I don't wear this", "I do not use this"};
	auto p1 = game.getcreature(itm1);
	auto s1 = game.getwear(itm1);
	auto p2 = game.getcreature(itm2);
	auto s2 = game.getwear(itm2);
	auto interactive = p1->ishero() || p2->ishero();
	if(!p1->isallowremove(*itm1, s1, interactive))
		return false;
	if(!p1->isallowremove(*itm2, s2, interactive))
		return false;
	if(!p1->isallow(*itm2, s1)) {
		if(interactive)
			p1->say(dontwear[0]);
		return false;
	}
	if(!p2->isallow(*itm1, s2)) {
		if(interactive)
			p2->say(dontwear[0]);
		return false;
	}
	iswap(*itm1, *itm2);
	return true;
}

bool creature::isallowremove(const item i, wear_s slot, bool interactive) {
	if(slot >= Head && slot <= Legs) {
		if(i.iscursed()) {
			static const char* speech[] = {"It's mine!",
				"Get yours hands off!",
				"I don't leave this",
			};
			if(interactive)
				say(maprnd(speech));
			return false;
		} else if(i.isnatural()) {
			static const char* speech[] = {"It's part of me!",
				"No, I can't remove this!"
			};
			if(interactive)
				say(maprnd(speech));
			return false;
		}
	}
	return true;
}

static bool read_message(creature* pc, dungeon* pd, dungeon::overlayi* po) {
	auto language = pd->getlanguage();
	if(!pc->canspeak(language))
		return false;
	switch(po->subtype) {
	case MessageHabbits:
		pc->say("%1 and %2 dwelve this halls",
			getstr(pd->head.habbits[0]), getstr(pd->head.habbits[1]));
		break;
	case MessageMagicWeapons:
		if(!pd->stat.weapons)
			pc->say("You don't find any magic weapon here");
		else
			pc->say("Find here %1i magic weapons", pd->stat.weapons);
		break;
	case MessageMagicRings:
		if(!pd->stat.rings)
			pc->say("You don't find any magic rings here", pd->stat.rings);
		else
			pc->say("Find here %1i magic rings", pd->stat.rings);
		break;
	case MessageSecrets:
		if(!pd->stat.secrets)
			pc->say("This halls don't have any secrets", pd->stat.secrets);
		else
			pc->say("Find %1i secret doors", pd->stat.secrets);
		break;
	case MessageTraps:
		if(!pd->stat.traps)
			pc->say("This level is safe");
		else
			pc->say("Avoid %1i deadly traps", pd->stat.traps);
		break;
	case MessageAtifacts:
		if(!pd->stat.artifacts)
			pc->say("You don't find any mighty artifact here");
		else
			pc->say("There is %1i mighty artifacts nearby", pd->stat.artifacts);
		break;
	default:
		pc->say("%1 eat his friend", getstr(pd->head.habbits[0]));
		break;
	}
	return true;
}

void read_message(dungeon* pd, dungeon::overlayi* po) {
	creature* pc = 0;
	for(auto v : party) {
		auto p = v.getcreature();
		if(!p || !p->isready())
			continue;
		if(!pc)
			pc = p;
		if(read_message(p, pd, po))
			return;
	}
	if(!pc)
		return;
	auto language = pd->getlanguage();
	switch(language) {
	case Dwarf: pc->say("Some kind of dwarven runes"); break;
	case Elf: pc->say("Some kind of elvish scripts"); break;
	default: pc->say("Some unrecognised language"); break;
	}
}

static int compare_char(const void* p1, const void* p2) {
	return *((char*)p2) - *((char*)p1);
}

void creature::roll_ability() {
	char result[8];
	for(auto& e : result)
		e = xrand(1, 6) + xrand(1, 6) + xrand(1, 6);
	qsort(result, sizeof(result) / sizeof(result[0]), sizeof(result[0]), compare_char);
	zshuffle(result, 6);
	int max_position = -1;
	int max_value = 0;
	for(int i = 0; i < 6; i++) {
		if(result[i] > max_value) {
			max_position = i;
			max_value = result[i];
		}
	}
	auto bst_position = bsdata<classi>::elements[type].ability;
	if(max_position != -1)
		iswap(result[max_position], result[bst_position]);
	// Check maximum by class
	for(int j = 0; j < 6; j++) {
		int m = bsdata<classi>::elements[type].minimum[j];
		if(result[j] < m)
			result[j] = m;
	}
	// Check minimum by race
	for(auto j = 0; j < 6; j++) {
		auto m = bsdata<racei>::elements[race].minimum[j];
		if(result[j] < m)
			result[j] = m;
	}
	// Check maximum by race
	for(auto j = 0; j < 6; j++) {
		auto m = bsdata<racei>::elements[race].maximum[j];
		if(result[j] > m)
			result[j] = m;
	}
	// Adjust ability
	for(auto j = 0; j < 6; j++)
		result[j] += bsdata<racei>::elements[race].adjustment[j];
	// Расставим атрибуты по местам
	for(auto j = 0; j < 6; j++)
		ability[j] = result[j];
	str_exeptional = xrand(1, 100);
}

int creature::getstrex() const {
	auto result = get(Strenght);
	if(result > 18)
		result += 6;
	else if(result == 18 && str_exeptional > 0) {
		if(str_exeptional <= 50)
			result += 1;
		else if(str_exeptional <= 75)
			result += 2;
		else if(str_exeptional <= 90)
			result += 3;
		else if(str_exeptional <= 99)
			result += 4;
		else
			result += 5;
	}
	return result;
}

bool creature::setweapon(item_s v, int charges) {
	if(wears[RightHand]) {
		say("No, my hand is busy!");
		return false;
	}
	item it(v);
	it.setcharges(charges);
	wears[RightHand] = it;
	return true;
}

creature* get_most_damaged() {
	creature* result = 0;
	int difference = 0;
	for(auto v : party) {
		auto e = v.getcreature();
		if(!e)
			continue;
		int hp = e->gethits();
		int mhp = e->gethitsmaximum();
		if(hp == mhp)
			continue;
		auto n = mhp - hp;
		if(n > difference) {
			result = e;
			difference = n;
		}
	}
	return result;
}

static void try_autocast(creature* pc) {
	spell_s healing_spells[] = {CureLightWounds, LayOnHands, Goodberry};
	for(auto e : healing_spells) {
		while(pc->get(e) > 0) {
			auto target = get_most_damaged();
			if(!target)
				break;
			pc->cast(e, Cleric, 0, target);
		}
	}
}

void creature::camp(item& it) {
	for(auto v : party) {
		auto e = v.getcreature();
		if(!e)
			continue;
		if(!e->isready())
			continue;
		try_autocast(e);
	}
	game.passtime(60 * 8);
	auto food = it.gettype();
	auto poisoned = it.iscursed();
	if(poisoned)
		mslog("Food was poisoned!");
	for(auto v : party) {
		auto pc = v.getcreature();
		if(!pc)
			continue;
		// RULE: Ring of healing get addition healing
		auto healed = pc->getenchant(CureLightWounds, 10);
		if(poisoned) {
			// RULE: Cursed food add weak poison
			pc->add(Poison, Instant, NoSave);
		} else {
			switch(food) {
			case Ration:
				healed += xrand(1, 3);
				break;
			case RationIron:
				healed += xrand(2, 6);
				break;
			}
		}
		pc->food = pc->getfoodmax();
		// Remove additional hit points
		pc->hits_aid = 0;
		// Remove enchanted weapon
		if(pc->wears[RightHand].is(Charged) && pc->wears[RightHand].is(Natural))
			pc->wears[RightHand].clear();
		// Heal damage
		pc->damage(Heal, healed);
		// Prepare spells
		pc->preparespells();
		// Recharge some items
		for(auto i = FirstInvertory; i <= LastInvertory; i = (wear_s)(i + 1)) {
			auto pi = pc->getitem(i);
			if(!pi || !(*pi))
				continue;
			auto type = pi->gettype();
			switch(type) {
			case MagicWand:
			case Staff:
				if(pi->iscursed())
					break;
				// RULE: Only mages can recharge spells
				if(pc->get(Mage) < 5)
					break;
				if(pi->getcharges() < 50)
					pi->setcharges(pi->getcharges() + 1);
				break;
			case MageScroll:
			case PriestScroll:
				// Autodetect scrolls by itellegence check
				if((type == MageScroll && pc->get(Mage))
					|| (type == PriestScroll && (pc->get(Cleric) || pc->get(Paladin) || pc->get(Ranger)))
					|| pc->get(Theif) >= 3) {
					if(pi->isidentified())
						break;
					if(pc->roll(Intellegence)) {
						pi->setidentified(1);
						char temp[128]; stringbuilder sb(temp); pi->getname(sb);
						pc->say("It's %1", temp);
					}
				}
				break;
			case RedPotion:
			case GreenPotion:
			case BluePotion:
				if(!pc->get(Mage))
					break;
				if(!pi->isidentified()) {
					if(pc->roll(Intellegence)) {
						pi->setidentified(1);
						char temp[128]; stringbuilder sb(temp); pi->getname(sb);
						pc->say("It's %1", temp);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

void creature::apply(spell_s id, int magic, unsigned duration) {
	dice dc;
	switch(id) {
	case CureLightWounds:
		dc.create(1 + magic * 2, 8, 4);
		damage(Heal, dc.roll(), 0);
		break;
	case CureDisease:
		remove(Disease);
		break;
	case CureBlindnessDeafness:
		remove(Blindness);
		remove(Deafness);
		break;
	case Identify:
		for(auto i = 0; i < magic; i++)
			identify(true);
		break;
	default:
		duration += duration*magic;
		add(id, duration, NoSave, 0);
		break;
	}
}

void creature::poison(save_s save, char save_bonus) {
	if(save != NoSave && roll(SaveVsPoison, save_bonus))
		return;
	active_spells.set(Poison);
}

bool creature::use(item* pi) {
	unsigned short forward_index = to(game.getcamera(), game.getdirection());
	auto pc = game.getcreature(pi);
	if(!pc || pc->gethits() <= 0)
		return false;
	auto slot = game.getwear(pi);
	if(!pc->isuse(*pi)) {
		pc->say("I don't know what to do with this");
		return false;
	}
	if(pi->is(UseInHand) && slot != RightHand && slot != LeftHand) {
		pc->say("I must use this in hand");
		return false;
	}
	// Weapon is special case
	if((slot == RightHand || slot == LeftHand)) {
		if((!(*pi) || pi->ismelee())) {
			game.attack(forward_index, false);
			return true;
		} else if(pi->isranged()) {
			auto original = game.getcamera();
			auto index = location.gettarget(game.getcamera(), game.getdirection());
			if(index != Blocked) {
				auto ranged = rangeto(original, index) > 1;
				game.attack(index, ranged);
				if(ranged)
					location.move(index, to(game.getdirection(), Down));
			}
			return true;
		}
	}
	spell_s spell_element;
	char name[128]; stringbuilder sb(name); pi->getname(sb);
	bool consume = true;
	auto type = pi->gettype();
	auto po = location.getoverlay(game.getcamera(), game.getdirection());
	auto magic = pi->getmagic();
	auto power = pi->getpower();
	switch(type) {
	case BluePotion:
	case GreenPotion:
	case RedPotion:
		if(power.type == Ability) {
			if(pi->iscursed()) {
				if(pc->ability[power.value]>0)
					pc->ability[power.value]--;
				pc->say("I feel really bad!");
			} else {
				pc->ability[power.value]++;
				pc->say("I feel greater power!");
			}
		} else if(pi->iscursed()) {
			static const char* text[] = {"Shit!", "It's poisoned!", "I feel bad."};
			pc->poison(NoSave);
			pc->say(maprnd(text));
		} else if(power.type == Spell) {
			auto& si = bsdata<spelli>::elements[power.value];
			si.effect.proc(pc, pc, si.effect, 10, SaveVsMagic);
		} else if(power.type == Enchant) {
			switch(power.value) {
			case OfAdvise:
				if(pi->isartifact())
					pc->addexp(50000);
				else
					pc->addexp(5000);
				break;
			}
		} else if(power.type == Ability) {
			pc->ability[power.value]++;
			pc->say("I feel greater power!");
		}
		break;
	case Ration:
	case RationIron:
		if(pi->isbroken()) {
			mslog("%1 is not eadible!", name);
			return false;
		}
		if(!draw::dlgask("Do you want make camp?"))
			return false;
		creature::camp(*pi);
		break;
	case MagicBook:
	case HolySymbol:
	case HolyWarriorSymbol:
		consume = false;
		spell_element = pc->choosespell((type == MagicBook) ? Mage : Cleric);
		if(!spell_element)
			return false;
		pc->cast(spell_element, (type == MagicBook) ? Mage : Cleric, 0);
		break;
	case TheifTools:
		consume = false;
		if(location.get(forward_index) == CellPit) {
			if(pc->use(RemoveTraps, forward_index, 10 + magic * 2, 0, 100, true)) {
				location.set(forward_index, CellPassable);
				mslog("You remove pit");
			}
		} else if(po && po->type == CellTrapLauncher) {
			if(location.isactive(po))
				pc->say("This trap already disabled");
			else if(pc->use(RemoveTraps, forward_index, magic * 5, 0, 100, true)) {
				location.setactive(po, true);
				mslog("You disable trap");
			}
		} else if(po && (po->type == CellKeyHole1 || po->type == CellKeyHole2)) {
			auto bonus = 0;
			if(po->type == CellKeyHole2)
				bonus -= 10;
			if(location.isactive(po))
				pc->say("This lock already open");
			else if(pc->use(OpenLocks, forward_index, magic * 3, 0, 100, true)) {
				location.setactive(po, true);
				mslog("You pick lock");
			}
		} else {
			pc->say("This usable on pit, lock or trap");
			return false;
		}
		if(magic < 0) {
			mslog("Lockpicks bite your finger and turn to dust");
			pc->damage(Pierce, dice::roll(1, 3));
			consume = true;
		} else if(d100() < 15 - magic)
			pi->damage("Your %1 is damaged", "You broke %1");
		break;
	case MagicWand:
		consume = false;
		if(power.type != Spell)
			return false;
		spell_element = (spell_s)power.value;
		if(pi->getcharges()) {
			if(magic <= 0)
				magic = 1;
			if(pc->cast(spell_element, Mage, magic)) {
				pi->setidentified(1);
				pi->setcharges(pi->getcharges() - 1);
				if(pi->iscursed()) {
					// RULE: Cursed wands want drawback
					if(d100() < 30)
						pc->damage(Pierce, dice::roll(1, 6));
				}
			} else {
				char name[32];
				mslog("Nothing happened, when %1 try use wand", pc->getname(name, zendof(name)));
			}
		}
		break;
	case DungeonMap:
		forward_index = location.getsecret();
		if(forward_index) {
			static const char* speech[] = {
				"Aha! This map show secret door!",
				"I know this! Map show me secret door!",
			};
			pc->say(maprnd(speech));
			draw::animation::appear(location, forward_index, 2);
		}
		break;
	case MageScroll:
	case PriestScroll:
		consume = false;
		if(!pi->isidentified()) {
			pc->say("When camping try to identify this");
			return false;
		}
		if(power.type == Spell) {
			spell_element = (spell_s)power.value;
			auto cls = (type == MageScroll) ? Mage : Cleric;
			if(pc->cast(spell_element, cls, iabs(magic)))
				consume = true;
			else {
				char name[64];
				mslog("Nothing happened, when %1 try use scroll", pc->getname(name, zendof(name)));
			}
		}
		break;
	case KeyShelf: case KeySilver: case KeyCooper: case KeySkull: case KeySpider:
	case KeyMoon: case KeyDiamond: case KeyGreen:
		if(po && (po->type == CellKeyHole1 || po->type == CellKeyHole2)) {
			if(location.getkeytype(po->type) == type) {
				location.setactive(po, true);
				creature::addexp(100, 0);
				mslog("You open lock");
			} else {
				pc->say("This does not fit");
				return false;
			}
		} else {
			pc->say("This must be used on key hole");
			return false;
		}
		break;
	default:
		return false;
	}
	if(consume)
		pi->clear();
	return true;
}

void creature::identifyall() {
	for(auto& e : wears)
		e.setidentified(1);
}

bool creature::identify(bool interactive) {
	static const char* talk[] = {"Look!", "I know this!", "Wait a minute."};
	for(auto& e : wears) {
		if(e.ismagical() && !e.isidentified()) {
			e.setidentified(1);
			if(interactive) {
				char temp[128]; stringbuilder sb(temp); e.getname(sb);
				say("%1 It's %2.", maprnd(talk), temp);
			}
			return true;
		}
	}
	if(interactive)
		say("I don't see any magical items.");
	return false;
}

bool creature::mending(bool interactive) {
	auto result = 0;
	for(auto& e : wears) {
		if(e.isbroken() && e.gettype() != Ration && e.gettype() != RationIron) {
			e.setbroken(0);
			result++;
		}
	}
	if(interactive) {
		if(result > 0)
			say("I fix %1i items.", result);
		else
			say("There is no damaged items.");
	}
	return result != 0;
}

int	creature::getfoodmax() const {
	return get(Constitution) * 10;
}

const spellprogi* creature::getprogress(class_s v) const {
	return bsdata<classi>::elements[type].spells[(v == Mage) ? 0 : 1];
}

unsigned creature::select(spell_s* result, const spell_s* result_maximum, class_s type, int level) {
	auto p = result;
	for(auto rec = spell_s(1); rec < FirstSpellAbility; rec = (spell_s)(rec + 1)) {
		if(getlevel(rec, type) != level)
			continue;
		*p++ = rec;
	}
	return p - result;
}

int creature::getcasterlevel(class_s id) const {
	auto pr = getprogress(id);
	if(!pr)
		return 0;
	auto hd = gethd();
	return pr->elements[hd][0];
}

item* creature::find(item_s v) const {
	for(auto& e : wears) {
		if(!e)
			continue;
		if(e.gettype() == v)
			return const_cast<item*>(&e);
	}
	return 0;
}

void creature::setmoved(bool value) {
	if(value)
		active_spells.set(Moved);
	else
		active_spells.remove(Moved);
}

reaction_s creature::rollreaction(int bonus) const {
	static reaction_s indifferent[19] = {Friendly, Friendly,
		Cautious, Cautious, Cautious, Cautious, Cautious, Cautious,
		Threatening, Threatening, Threatening, Threatening, Threatening, Threatening,
		Hostile, Hostile, Hostile, Hostile, Hostile};
	auto cha = getparty(Charisma);
	bonus += maptbl(charisma_reaction_bonus, cha);
	auto result = (rand() % 10) + (rand() % 10) + 2 - bonus;
	result = imax(2, imin(20, result));
	auto result_table = indifferent;
	return result_table[result - 2];
}

int	creature::getparty(ability_s v) {
	auto total = 0;
	auto count = 0;
	for(auto ev : party) {
		auto p = ev.getcreature();
		if(!p || !(*p) || !p->isready())
			continue;
		total += p->get(v);
		count++;
	}
	if(!count)
		return 0;
	return total / count;
}

void creature::interract() {
	auto old_reaction = reaction;
	auto new_reaction = reaction;
	if(reaction == Indifferent)
		new_reaction = rollreaction(0);
	if(old_reaction != new_reaction) {
		location.set(index, new_reaction);
		encounter(new_reaction);
		if(!*this)
			return;
	}
	auto party_index = game.getcamera();
	auto party_direction = game.getdirection();
	switch(reaction) {
	case Flight:
		// Just run away
		location.move(index, party_direction);
		break;
	default:
		mslog("You are under attack!");
		location.turnto(party_index, to(direction, Down));
		game.attack(index, false);
		break;
	}
}

void creature::apply(apply_proc proc, bool interactive) {
	for(auto ev : party) {
		auto p = ev.getcreature();
		if(p)
			(p->*proc)(interactive);
	}
}

void creature::addparty(item i, bool interactive) {
	for(auto ev : party) {
		auto p = ev.getcreature();
		if(!p)
			continue;
		for(auto& e : p->wears) {
			if(e)
				continue;
			e = i;
			if(interactive) {
				char t1[260]; stringbuilder s1(t1); i.getname(s1);
				char t2[260]; stringbuilder s2(t2); p->getname(s2);
				mslog("%1 gain %2", t2, t1);
			}
			return;
		}
	}
	location.dropitem(game.getcamera(), i, game.getside(0, game.getdirection()));
}

bool creature::usequick() {
	item* pi = 0;
	if(!wears[RightHand])
		pi = &wears[RightHand];
	else if(!wears[LeftHand])
		pi = &wears[LeftHand];
	else
		return false;
	item* ps = 0;
	for(auto i = FirstBelt; i <= LastBelt; i = (wear_s)(i + 1)) {
		if(!wears[i])
			continue;
		ps = &wears[i];
		break;
	}
	if(!ps)
		return false;
	return swap(pi, ps);
}

void creature::uncurse() {
	for(auto i = Head; i <= LastBelt; i = (wear_s)(i + 1)) {
		if(wears[i].iscursed() && wears[i].isidentified()) {
			char temp[260]; stringbuilder sb(temp); wears[i].getname(sb);
			mslog("%1 is turned to dust", temp);
			wears[i].clear();
		}
	}
}

int creature::getpartyindex() const {
	return game.getindex(this);
}

void creature::dressoff() {
	dress_wears(-1);
}

void creature::dresson() {
	dress_wears(1);
}

void creature::dress_wears(int m) {
	for(auto id = Head; id <= Legs; id = (wear_s)(id + 1)) {
		auto it = wears[id];
		if(!it)
			continue;
		auto sp = it.getpower();
		if(!sp)
			continue;
		if(sp.type == Ability) {
			auto& ei = bsdata<abilityi>::elements[sp.value];
			ability[sp.value] += ei.multiplier*it.getmagic()*m;
		}
	}
}

void creature::remove(spell_s v) {
	if(active_spells.is(v)) {
		active_spells.remove(v);
		removeboost(v);
		switch(v) {
		case Disease:
			disease_progress = 0;
			break;
		}
	}
}

void creature::select(itema& result) {
	for(auto& e : wears) {
		if(!e)
			continue;
		result.add(&e);
	}
}

bool creature::haveforsale() const {
	itema items;
	const_cast<creature*>(this)->select(items);
	items.forsale(false);
	return items.getcount() > 0;
}

bool creature::ismatch(const variant v) const {
	switch(v.type) {
	case Alignment: return getalignment() == v.value;
	case Ability: return get((ability_s)v.value) > 0;
	case Action:
		switch(v.value) {
		case HealParty: return gethits() < gethitsmaximum();
		case RessurectBones: return have(Bones);
		case Trade: return haveforsale();
		case RemoveItem1: case RemoveItem2: case RemoveItem3:
		case RemoveItemS1: case RemoveItemS2: return have(game.items[v.value - RemoveItem1]);
		default: return true;
		}
	case Class: return get((class_s)v.value) > 0;
	case Gender: return getgender() == v.value;
	case Race: return getrace() == v.value;
	case Item: return have((item_s)v.value);
	case Spell: return isknown((spell_s)v.value);
	case Reaction: return getreaction() == v.value;
	}
	return false;
}

bool creature::ismatch(const messagei& v) const {
	for(auto e : v.variants) {
		if(!e)
			break;
		if(!ismatch(e))
			return false;
	}
	return true;
}