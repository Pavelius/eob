#include "main.h"
#include "archive.h"

BSDATAC(creature, 32)

const char* get_name_part(short rec);

static wear_s wear_slots[] = {Head, Neck, Body, Elbow, Legs};
char reaction_adjustment[] = {
	-7, -6, -4, -3, -2, -1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 2, 3, 3,
	4, 4, 4, 5, 5
};
static char hit_points_adjustment[] = {
	-4,
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
		if(interactive)
			mslog("%1 feel poison", getname());
		draw::animation::update();
		damage(Magic, hits);
	}
}

bool creature::add(spell_s type, unsigned duration, save_s save, char save_bonus, ability_s save_type) {
	if(is(type))
		return true;
	if(roll(ResistMagic))
		return false;
	switch(type) {
	case HoldPerson:
	case Sleep:
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
	if(ismonster())
		return getmonster().hd[0];
	return levels[0];
}

int	creature::getawards() const {
	if(ismonster())
		return getmonster().getexperience();
	return gethd() * 100;
}

size_s creature::getsize() const {
	if(ismonster())
		return getmonster().size;
	return Medium;
}

resource_s creature::getres() const {
	if(ismonster())
		return getmonster().rfile;
	return NONE;
}

bool combati::is(enchant_s v) const {
	return weapon && weapon->getenchant(v) != 0;
}

void creature::get(combati& result, wear_s weapon, size_s enemy_size) const {
	result.number_attacks_p2r = 1 * 2;
	result.bonus += get(AttackMelee);
	if(wears[weapon]) {
		auto& wi = bsdata<itemi>::elements[wears[weapon].gettype()].weapon;
		wears[weapon].get(result, enemy_size);
		result.weapon = const_cast<item*>(&wears[weapon]);
		if(result.weapon->is(Natural))
			result.damage.b += gethd() / 3;
	} else
		result.damage = {1, 2};
	if(is(BonusVsElfWeapon) && wears[weapon].is(UseTheifWeapon))
		result.bonus++;
	// Weapon secialist get bonus to hit (only to main hand?)
	if(weapon == RightHand && isspecialize(wears[weapon].gettype())) {
		result.number_attacks_p2r++;
		result.bonus++;
		result.damage.b += 2;
	}
	result.damage.b += get(DamageMelee);
}

bool creature::add(item value) {
	for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i + 1)) {
		if(wears[i]) {
			if(wears[i].stack(value))
				return true;
			continue;
		}
		wears[i] = value;
		return true;
	}
	return false;
}

item creature::get(wear_s id) const {
	auto pi = const_cast<creature*>(this)->getitem(id);
	if(!pi)
		return NoItem;
	return *pi;
}

bool creature::roll(ability_s id, int bonus) const {
	auto n = get(id);
	if(id >= Strenght && id <= Charisma)
		n *= 5;
	n += bonus * 5;
	if(n <= 0)
		return false;
	auto d = d100();
	return d <= n;
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
	if(is(Regeneration))
		damage(Heal, 1);
	// Every turn heroes use energy
	if(ishero())
		subenergy();
}

bool creature::isenemy(creature* target) const {
	return ishero() != target->ishero();
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
	static const wear_s slots[] = {Head, Neck, Body, RightRing, LeftRing, Elbow, Quiver, Legs, RightHand, LeftHand};
	for(auto slot : slots) {
		auto pi = getitem(slot);
		if(!pi)
			continue;
		if(!isallow(it, slot))
			continue;
		if(*pi) {
			if(pi->stack(it))
				return;
		} else {
			*pi = it;
			return;
		}
	}
	add(it);
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

void creature::satisfy() {
	food = getfoodmax();
}

void creature::exhause() {
	food = 0;
	if(!roll(Constitution, 2))
		damage(Magic, 1, 5);
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

bool creature::useammo(item_s ammo, wear_s slot, bool use_item) {
	if(ammo) {
		if(wears[Quiver].is(ammo)) {
			if(use_item)
				wears[Quiver].use();
		} else {
			if(ishero())
				say("No ammunitions!");
			if(remove(slot, false))
				usequick();
			return false;
		}
	}
	return true;
}

void creature::attack(creature* defender, wear_s slot, int bonus, int multiplier) {
	combati wi = {}; get(wi, slot, defender->getsize());
	item_s ammo = NoItem;
	if(wi.weapon)
		ammo = wi.weapon->getammo();
	if(ammo) {
		if(wears[Quiver].is(ammo)) {
			auto& awi = wears[Quiver].gete().weapon;
			auto amb = wears[Quiver].getmagic();
			wi.bonus += awi.bonus + amb;
			wi.damage.b += awi.damage.b + amb;
			wi.critical_multiplier += awi.critical_multiplier;
			wi.critical_range += awi.critical_range;
		}
	}
	auto ac = defender->get(AC);
	if(defender->is(BonusACVsLargeEnemy) && getsize() >= Large)
		ac += 4;
	if(is(BonusToHitVsGoblinoid) && defender->getrace() == Goblinoid)
		wi.bonus += 1;
	if(is(BonusDamageVsEnemy) && (defender->getrace() == Humanoid || defender->getrace() == Goblinoid))
		wi.bonus += 4;
	auto magic_bonus = 0;
	if(wi.weapon) {
		magic_bonus = wi.weapon->getmagic();
		if(defender->is(Undead)) {
			auto holyness = wi.weapon->getenchant(OfHolyness);
			wi.bonus += holyness;
			wi.damage.b += holyness * 2;
			if(wi.weapon->is(SevereDamageUndead))
				wi.damage.b += 2;
		}
	}
	if(ishero())
		subenergy();
	for(auto atn = (wi.number_attacks_p2r + (game.getrounds() % 2)) / 2; atn > 0; atn--) {
		if(!useammo(ammo, slot, true))
			return;
		auto tohit = 20 - (wi.bonus + bonus) - (10 - ac);
		auto rolls = xrand(1, 20);
		auto hits = -1;
		auto chance_critical = 20 - wi.critical_range;
		auto damage_type = wi.type;
		tohit = imax(2, imin(20, tohit));
		if(rolls >= tohit || rolls >= chance_critical) {
			auto damage = wi.damage;
			hits = damage.roll();
			hits = hits * multiplier;
			if(wi.is(OfFire)) {
				damage_type = Fire;
				hits += xrand(1, 6);
			}
			if(wi.is(OfCold)) {
				damage_type = Cold;
				hits += xrand(2, 5);
			}
			// RULE: crtitical hit can deflected
			if(rolls >= chance_critical) {
				// RULE: critical damage depends on weapon and count in dices
				if(!defender->roll(CriticalDeflect))
					hits = hits * 2;
				if(wi.weapon) {
					// RULE: Weapon with spell cast it
					auto power = wi.weapon->getpower();
					if(power.type == Spell) {
						auto spell = (spell_s)power.value;
						if(bsdata<spelli>::elements[spell].effect.type.type == Damage)
							cast(spell, Mage, wi.weapon->getmagic(), defender);
						else
							cast(spell, Mage, wi.weapon->getmagic(), this);
					}
				}
			}
			// RULE: vampiric ability allow user to drain blood and regain own HP
			if(wi.is(OfVampirism)) {
				auto hits_healed = xrand(2, 5);
				if(hits_healed > hits)
					hits_healed = hits;
				this->damage(Heal, hits_healed);
			}
		}
		// Fear attack (Not depend on attack result)
		if(wi.is(OfFear))
			defender->add(Fear, xrand(1, 3) * 10, SaveNegate);
		// When attacking sleeping creature she wake up!
		defender->remove(Sleep);
		// Show result
		draw::animation::attack(this, slot, hits);
		if(hits != -1) {
			// Poison attack
			if(wi.is(OfPoison))
				defender->add(Poison, Instant, SaveNegate);
			// Paralize attack
			if(wi.is(OfParalize))
				defender->add(HoldPerson, xrand(1, 3), SaveNegate);
			// Drain ability
			if(wi.is(OfEnergyDrain))
				attack_drain(defender, defender->drain_energy, hits);
			if(wi.is(OfStrenghtDrain))
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
			if(d100() < 30) {
				if(wi.weapon->damage(0, 0))
					usequick();
			} else
				damage(Bludgeon, 1, 3);
			return;
		}
	}
	useammo(ammo, slot, false);
}

void creature::attack(indext index, direction_s d, int bonus, bool ranged, int multiplier) {
	auto defender = game.getdefender(index, d, this);
	if(!defender) {
		// Monster can't respond to ranged attack
		if(ishero())
			return;
		if(is(Moved))
			return;
		location.move(getindex(), d);
		return;
	}
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
		attack(defender, RightHand, bonus, multiplier);
	else {
		if(wp2) {
			attack(defender, RightHand, bonus + gethitpenalty(-4), multiplier);
			attack(defender, LeftHand, bonus + gethitpenalty(-6), multiplier);
		} else
			attack(defender, RightHand, bonus, multiplier);
		if(wp3)
			attack(defender, Head, bonus, multiplier);
	}
}

void creature::addexp(int value) {
	auto b = get(BonusExperience);
	if(b)
		value += b * value / 20;
	if(value <= 0)
		value = 0;
	experience += value;
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::finish() {
	basic.hits_rolled = 0;
	memset(levels, 0, sizeof(levels));
	memset(spells, 0, sizeof(spells));
	memset(prepared, 0, sizeof(prepared));
	known_spells.clear();
	active_spells.clear();
	auto& ri = bsdata<racei>::elements[getrace()];
	basic.usability.add(ri.usability);
	basic.feats.add(ri.feats);
	for(auto i = SaveVsParalization; i <= DetectSecrets; i = (ability_s)(i + 1))
		basic.ability[i] += ri.skills[i];
	auto& ci = bsdata<classi>::elements[type];
	basic.usability.add(ci.usability);
	basic.feats.add(ci.feats);
	update_start();
	update_finish();
	if(ismonster()) {
		basic.hits_rolled = gethitdice().roll();
		if(basic.hits_rolled < 1)
			basic.hits_rolled = 1;
	} else {
		for(int i = 0; i < 3; i++) {
			auto c = getclass(getclass(), i);
			if(!c)
				continue;
			raise_level(c);
		}
	}
	update_start();
	update_finish();
	hits = gethitsmaximum();
	food = getfoodmax();
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
	basic.hits_rolled += hp;
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
	if(ismonster())
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
	return hits_rolled + hits_aid;
}

int creature::getspeed() const {
	return get(Speed);
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
			raise_level(i);
			mslog("%1 gain level of experience", getname());
		}
	}
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

char creature::racial_bonus(char* data) const {
	return data[getrace()];
}

bool creature::isspecialize(item_s type) const {
	auto cls = getclass();
	if(cls != Fighter)
		return 0;
	switch(getrace()) {
	case Dwarf: return type == AxeBattle || type == Mace || type == HammerWar;
	case HalfElf: case Elf: return type == SwordLong || type == SwordShort || type == Bow;
	case Halfling: return type == SwordShort;
	default: return type == SwordLong || type == SwordBastard || type == SwordTwoHanded;
	}
}

void creature::setframe(short* frames, short index) const {
	if(ismonster()) {
		auto po = getmonster().overlays;
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
	if(ismonster())
		return direction;
	return game.getdirection();
}

short unsigned creature::getindex() const {
	if(ismonster())
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
	return bsdata<itemi>::elements[v.gettype()].usability.allof(usability);
}

void creature::set(direction_s v) {
	if(ismonster())
		direction = v;
	else
		game.setcamera(getindex(), v);
}

void creature::setside(int value) {
	if(ismonster())
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
	auto& di = bsdata<damagei>::elements[type];
	if(di.half && is(di.half))
		hits = (hits + 1) / 2;
	if(di.reduce) {
		auto v = get(di.reduce);
		if(v) {
			if(v > 100)
				v = 100;
			hits = hits * (100 - v) / 100;
		}
	}
	if(di.immunity && is(di.immunity))
		hits = 0;
	if(hits == 0)
		return;
	remove_hits(hits_aid, hits);
	sethits(this->hits - hits);
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
	game.addexpc(value, hitd);
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
	location.stat.monsters_killed++;
	nameable::kill();
	clear();
}

bool creature::use(ability_s skill, indext index, int bonus, bool* firsttime, int exp, bool interactive) {
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
	static const char* speech[2] = {
		"I don't wear this",
		"I do not use this"
	};
	auto p1 = itm1->getowner();
	auto s1 = game.getwear(itm1);
	auto p2 = itm2->getowner();
	auto s2 = game.getwear(itm2);
	auto interactive = p1->ishero() || p2->ishero();
	if(!p1->isallowremove(*itm1, s1, interactive))
		return false;
	if(!p1->isallowremove(*itm2, s2, interactive))
		return false;
	if(!p1->isallow(*itm2, s1)) {
		if(interactive)
			p1->say(maprnd(speech));
		return false;
	}
	if(!p2->isallow(*itm1, s2)) {
		if(interactive)
			p2->say(maprnd(speech));
		return false;
	}
	if(!itm2->stack(*itm1))
		iswap(*itm1, *itm2);
	return true;
}

bool creature::isallowremove(const item i, wear_s slot, bool interactive) {
	if(slot >= Head && slot <= Legs) {
		if(i.iscursed()) {
			static const char* speech[] = {
				"It's mine!",
				"Get yours hands off!",
				"I don't leave this",
			};
			if(interactive)
				say(maprnd(speech));
			return false;
		} else if(i.isnatural()) {
			static const char* speech[] = {
				"It's part of me!",
				"No, I can't remove this!"
			};
			if(interactive)
				say(maprnd(speech));
			return false;
		}
	}
	return true;
}

static bool read_message(creature* pc, dungeoni* pd, dungeoni::overlayi* po) {
	auto language = pd->getlanguage();
	if(!pc->canspeak(language))
		return false;
	switch(po->subtype) {
	case MessageHabbits:
		pc->say("%1 and %2 dwelve this halls",
			bsdata<monsteri>::elements[pd->head.habbits[0]].name,
			bsdata<monsteri>::elements[pd->head.habbits[1]].name);
		break;
	case MessageBoss:
		if(pd->stat.boss) {
			pc->say("%1 hunt you down in locked room",
				bsdata<monsteri>::elements[pd->stat.boss].name);
		} else
			pc->say("Keep your weapons in good shape");
		break;
	case MessageSpecialItem:
		if(pd->head.wands) {
			pc->say("Find %1 somewhere on this level",
				bsdata<itemi>::elements[pd->head.wands].name);
		} else
			pc->say("Treat wounds and rest when in need");
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
		pc->say("%1 eat his friends",
			bsdata<monsteri>::elements[pd->head.habbits[0]].name);
		break;
	}
	return true;
}

void read_message(dungeoni* pd, dungeoni::overlayi* po) {
	creature* pc = 0;
	for(auto p : party) {
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

void creature::campcast(item& it) {
	auto pe = it.getenchantment();
	if(pe && pe->power.type == Spell) {
		auto& ei = bsdata<spelli>::elements[pe->power.value];
		apply((spell_s)pe->power.value, gethd() + it.getmagic());
	}
}

void creature::resting(int healed) {
	satisfy();
	// Remove additional hit points
	hits_aid = 0;
	// Remove enchanted weapon
	if(wears[RightHand].is(Charged) && wears[RightHand].is(Natural))
		wears[RightHand].clear();
	// Heal damage
	damage(Heal, healed);
	// Prepare spells
	preparespells();
	// Some magic items activate
	for(auto s : wear_slots)
		campcast(wears[s]);
	if(wears[LeftHand].getitem().usability.is(UseShield))
		campcast(wears[LeftHand]);
	// Recharge some items
	for(auto& it : wears) {
		if(!it)
			continue;
		auto type = it.gettype();
		switch(type) {
		case MagicWand:
		case Staff:
			if(it.iscursed())
				break;
			// RULE: Only mages can recharge spells
			if(get(Mage) < 5)
				break;
			if(it.getcharges() < 40)
				it.setcharges(it.getcharges() + 1);
			break;
		case MageScroll:
		case PriestScroll:
			// Autodetect scrolls by itellegence check
			if((type == MageScroll && get(Mage))
				|| (type == PriestScroll && (get(Cleric) || get(Paladin) || get(Ranger)))
				|| get(Theif) >= 3) {
				if(!it.isidentified() && roll(Intellegence)) {
					it.setidentified(1);
					say(it, "It's %1");
				}
			}
			break;
		case RedPotion:
		case GreenPotion:
		case BluePotion:
			if(!get(Mage))
				break;
			if(!it.isidentified() && roll(Intellegence)) {
				it.setidentified(1);
				say(it, "It's %1");
			}
			break;
		default:
			break;
		}
	}
}

bool creature::save(int& value, ability_s skill, save_s type, int bonus) {
	switch(type) {
	case SaveHalf:
		if(roll(skill, bonus))
			value = value / 2;
		break;
	case SaveNegate:
		if(roll(skill, bonus))
			return true;
		break;
	}
	return false;
}

void creature::poison(save_s save, char save_bonus) {
	if(save != NoSave && roll(SaveVsPoison, save_bonus))
		return;
	active_spells.set(Poison);
}

bool creature::use(item* pi) {
	unsigned short forward_index = to(game.getcamera(), game.getdirection());
	auto pc = pi->getowner();
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
			game.attack(forward_index, false, NoAmbush);
			return true;
		} else if(pi->isranged()) {
			auto original = game.getcamera();
			auto index = location.gettarget(game.getcamera(), game.getdirection());
			if(index != Blocked) {
				auto ranged = rangeto(original, index) > 1;
				game.attack(index, ranged, NoAmbush);
				if(ranged)
					location.move(index, to(game.getdirection(), Down));
			}
			return true;
		}
	}
	spell_s spell_element;
	char name[128]; stringbuilder sb(name); pi->getname(sb);
	bool consume = true;
	bool disable = false;
	auto type = pi->gettype();
	auto po = location.getoverlay(game.getcamera(), game.getdirection());
	auto magic = pi->getmagic();
	auto power = pi->getpower();
	switch(type) {
	case BluePotion:
	case GreenPotion:
	case RedPotion:
		if(power.type == Ability) {
			static int experience_amought[5 + 1] = {0, 500, 2000, 5000, 10000, 50000};
			switch(power.value) {
			case BonusExperience:
				if(magic > 0)
					pc->addexp(maptbl(experience_amought, magic));
				else
					pc->addexp(-maptbl(experience_amought, -magic));
				break;
			default:
				if(pi->iscursed()) {
					if(pc->basic.ability[power.value] > 0)
						pc->basic.ability[power.value]--;
					pc->say("I feel really bad!");
				} else {
					pc->basic.ability[power.value]++;
					pc->say("I feel greater power!");
				}
				break;
			}
		} else if(pi->iscursed()) {
			static const char* text[] = {"Shit!", "It's poisoned!", "I feel bad."};
			pc->poison(NoSave);
			pc->say(maprnd(text));
		} else if(power.type == Spell)
			pc->apply((spell_s)power.value, 5 + magic * 3);
		break;
	case Ration:
	case RationIron:
		if(pi->isbroken()) {
			mslog("%1 is not eadible!", name);
			return false;
		}
		if(location.ismonsternearby(pc->getindex(), 3)) {
			pc->say("Can't camp here. Monsters are nearby.");
			return false;
		}
		if(!draw::dlgask("Do you want make camp?"))
			return false;
		game.camp(*pi);
		break;
	case TheifTools:
		consume = false;
		if(location.get(forward_index) == CellPit) {
			if(pc->use(RemoveTraps, forward_index, 10 + magic * 2, 0, 100, true)) {
				location.set(forward_index, CellPassable);
				mslog("You remove pit");
			}
		} else if(po && po->type == CellTrapLauncher) {
			if(location.isactive(po)) {
				disable = true;
				pc->say("This trap already disabled");
			} else if(pc->use(RemoveTraps, forward_index, magic * 5, 0, 100, true)) {
				location.setactive(po, true);
				mslog("You disable trap");
			}
		} else if(po && (po->type == CellKeyHole1 || po->type == CellKeyHole2)) {
			auto bonus = 0;
			if(po->type == CellKeyHole2)
				bonus -= 10;
			if(location.isactive(po)) {
				disable = true;
				pc->say("This lock already open");
			} else if(pc->use(OpenLocks, forward_index, magic * 3, 0, 100, true)) {
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
		} else if(d100() < 15 - magic) {
			if(!disable)
				pi->damage("Your %1 is damaged", "You broke %1");
		}
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
			} else
				mslog("Nothing happened, when %1 try use wand", pc->getname());
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
			location.appear(forward_index, 2);
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
			else
				mslog("Nothing happened, when %1 try use scroll", pc->getname());
		}
		break;
	case HolySymbol:
	case HolySymbolEvil:
		pc->say("Holy god, give us your blessing!");
		game.addcity(Blessing, 1);
		game.addexp(type == HolySymbol ? Good : Evil, 100);
		break;
	case MagicBook:
		pc->say("Interesting...");
		pc->addexp(300);
		break;
	case Bones:
		pc->say("Rest in peace, unknown fellow");
		game.addcity(Blessing, 1);
		game.addexp(Good, 50);
		break;
	case KeyShelf: case KeySilver: case KeyCooper: case KeySkull: case KeySpider:
	case KeyMoon: case KeyDiamond: case KeyGreen:
		if(po && (po->type == CellKeyHole1 || po->type == CellKeyHole2)) {
			if(location.getkeytype(po->type) == type) {
				location.setactive(po, true);
				game.addexpc(100, 0);
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

bool creature::enchant(spell_s id, int level, bool run) {
	auto result = false;
	for(auto& e : wears) {
		if(e) {
			if(e.cast(id, level, run))
				result = true;
			if(!run && result)
				break;
		}
	}
	return result;
}

int	creature::getfoodmax() const {
	auto r = get(Constitution);
	return r * r;
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
	static reaction_s result_table[19] = {Friendly, Friendly,
		Indifferent, Indifferent, Indifferent, Indifferent, Indifferent, Indifferent,
		Hostile, Hostile, Hostile, Hostile, Hostile, Hostile,
		Hostile, Hostile, Hostile, Hostile, Hostile};
	bonus += party.getaverage(ReactionBonus);
	auto result = (rand() % 10) + (rand() % 10) + 2 - bonus;
	result = imax(2, imin(20, result));
	return result_table[result - 2];
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

void creature::uncurse(bool interactive) {
	for(auto i = Head; i <= LastBelt; i = (wear_s)(i + 1)) {
		if(wears[i].iscursed() && wears[i].isidentified()) {
			if(interactive) {
				char temp[260]; stringbuilder sb(temp); wears[i].getname(sb);
				mslog("%1 is turned to dust", temp);
			}
			wears[i].clear();
		}
	}
}

int creature::getpartyindex() const {
	return game.getindex(this);
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
	items.forsale(true);
	return items.getcount() > 0;
}

bool creature::ismatch(const variant v) const {
	switch(v.type) {
	case Alignment: return getalignment() == v.value;
	case Ability: return get((ability_s)v.value) > 0;
	case Class: return get((class_s)v.value) > 0;
	case Cleveress: return is((intellegence_s)v.value);
	case Condition: return is((condition_s)v.value);
	case Gender: return getgender() == v.value;
	case Race: return getrace() == v.value;
	case Item: return have((item_s)v.value);
	case Morale: return is((morale_s)v.value);
	case Reaction: return reaction == (reaction_s)v.value;
	case Spell: return isknown((spell_s)v.value);
	}
	return false;
}

bool creature::ismatch(const conditiona& v) const {
	for(auto e : v) {
		if(!e)
			break;
		if(!ismatch(e))
			return false;
	}
	return true;
}

bool creature::remove(wear_s slot, bool interactive) {
	if(!isallowremove(wears[slot], slot, interactive))
		return false;
	if(!add(wears[slot]))
		return false;
	wears[slot].clear();
	return true;
}

class_s	creature::getcaster() const {
	if(get(Cleric) || get(Paladin))
		return Cleric;
	if(get(Mage) || get(Ranger))
		return Mage;
	return NoClass;
}

void creature::removeloot() {
	for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i+1)) {
		auto& e = wears[i];
		if(!e)
			continue;
		if(e.isstarted() || e.is(Expandable))
			e.clear();
	}
}

bool creature::is(morale_s v) const {
	auto& e = bsdata<alignmenti>::elements[alignment];
	return e.morale == v || e.law == v;
}

bool creature::is(intellegence_s v) const {
	auto i = get(Intellegence);
	return i >= bsdata<intellegencei>::elements[v].v1
		&& i <= bsdata<intellegencei>::elements[v].v2;
}

bool creature::ismindless() const {
	auto i = get(Intellegence);
	return i <= bsdata<intellegencei>::elements[Semi].v2;
}

wear_s creature::getslot(const item* p) const {
	if(p >= wears && p < wears + LastInvertory)
		return (wear_s)(p - wears);
	return Backpack;
}

spell_s creature::choosespell(class_s type) const {
	variantc spells;
	spells.cspells(this, true);
	return (spell_s)spells.chooselv(type);
}

void creature::scribe(item& it) {
	auto sv = it.getpower();
	if(!sv || sv.type != Spell) {
		say("This is not magic scroll");
		return;
	}
	auto sp = (spell_s)sv.value;
	auto spn = bsdata<spelli>::elements[sp].name;
	if(roll(LearnSpell)) {
		setknown(sp);
		mslog("%1 learn %2 spell", getname(), spn);
		addexp(100);
	} else
		mslog("%1 don't learn %2 spell", getname(), spn);
	it.clear();
}

void creature::flee(bool interactive) {
	index = location.getretreat(game.getcamera(), 4);
	if(index == Blocked) {
		if(interactive)
			mslog("%1 can't flee!", getname());
	} else {
		if(interactive)
			mslog("%1 flee!", getname());
		setindex(index);
	}
}

bool creature::is(condition_s v) const {
	switch(v) {
	case BadlyWounded: return gethits() < gethitsmaximum() / 2;
	case Healed: return gethits() >= gethitsmaximum();
	case Wounded: return gethits() < gethitsmaximum();
	default: return false;
	}
}

creature* creature::get(void* focus) {
	auto i = bsdata<creature>::source.indexof(focus);
	if(i == -1)
		return 0;
	return (creature*)bsdata<creature>::source.ptr(i);
}

void creature::update_wears() {
	ability[Speed] += wears[RightHand].getitem().weapon.speed + wears[RightHand].getenchant(OfSpeed);
	ability[Speed] += wears[LeftHand].getitem().weapon.speed + wears[LeftHand].getenchant(OfSpeed);
	if(ismonster())
		ability[AC] += (10 - getmonster().ac);
	for(auto s : wear_slots) {
		ability[AC] += wears[s].getitem().armor.ac;
		ability[CriticalDeflect] += wears[s].getitem().armor.deflect * 5;
		statable::apply(wears[s], false);
	}
	statable::apply(wears[RightRing], true);
	statable::apply(wears[LeftRing], true);
	statable::apply(wears[RightHand], false);
	statable::apply(wears[LeftHand], false);
}

static void copy_value(statable& p1, statable& p2) {
	p1 = p2;
}

void creature::update_start() {
	copy_value(*this, basic);
	if(ismonster())
		hits_rolled += getmonster().hd[1];
	else {
		auto hd = gethd();
		auto rl = (int)hits_rolled / getclasscount();
		auto md = maptbl(hit_points_adjustment, ability[Constitution]);
		if(md > 2 && !is(BonusHP))
			md = 2;
		rl += md * hd;
		if(rl < hd)
			rl = hd;
		hits_rolled = rl;
	}
	update_wears();
	// Ability drain
	ability[Strenght] -= drain_strenght + disease_progress / 3;
	ability[Constitution] -= disease_progress / 2;
}

void creature::update_boost_effects() {
	// Remove unused boost
	auto rounds = game.getrounds();
	auto pb = bsdata<boosti>::begin();
	for(auto& e : bsdata<boosti>()) {
		if(e.round >= rounds)
			*pb++ = e;
	}
	// Update boost effects
	bsdata<boosti>::source.setcount(pb - bsdata<boosti>::begin());
	for(auto& e : bsdata<boosti>()) {
		auto p = e.owner.getcreature();
		if(!p)
			continue;
		p->active_spells.set(e.id);
	}
}

void creature::update_finish() {
	static char bonus_ability[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 2, 2, 3
	};
	for(auto i = SaveVsParalization; i <= SaveVsMagic; i = (ability_s)(i + 1))
		statable::add(i, type, levels);
	auto theif_class = get(Ranger) ? Ranger : Theif;
	auto theif_level = get(theif_class);
	for(auto i = ClimbWalls; i <= ReadLanguages; i = (ability_s)(i + 1))
		statable::add(i, theif_class, theif_level);
	statable::add(LearnSpell, type);
	auto main_ability = bsdata<classi>::elements[type].ability;
	auto ability_value = get(main_ability);
	ability[BonusExperience] += maptbl(bonus_ability, ability_value);
	ability[AttackAll] -= drain_energy;
	update_attacks(getbestclass(), gethd());
	update_stats();
	if(!is(Disease))
		disease_progress = 0;
	if(is(AcidArrow))
		damage(Magic, dice::roll(2, 4), 5);
	remove(Moved);
}

void creature::healing() {
	damage(Heal, xrand(2, 7));
}