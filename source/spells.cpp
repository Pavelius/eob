#include "main.h"

static int turn_undead_index[15] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 10, 10, 11};
static int turn_undead_chance[][12] = {{10, 7, 4, 0, 0, -1, -1, -2, -2, -2, -2, -2},
{13, 10, 7, 4, 0, 0, -1, -1, -2, -2, -2, -2},
{16, 13, 10, 7, 4, 0, 0, -1, -1, -2, -2, -2},
{19, 16, 13, 10, 7, 4, 0, 0, -1, -1, -2, -2},
{20, 19, 16, 13, 10, 7, 4, 0, 0, -1, -1, -2},
{30, 20, 19, 16, 13, 10, 7, 4, 0, 0, -1, -1},
{30, 30, 20, 19, 16, 13, 10, 7, 4, 0, 0, -1},
{30, 30, 30, 20, 19, 16, 13, 10, 7, 4, 0, 0},
{30, 30, 30, 30, 20, 19, 16, 13, 10, 7, 4, 0},
{30, 30, 30, 30, 30, 20, 19, 16, 13, 10, 7, 4},
};

static int getduration(duration_s duration, int level) {
	return bsdata<durationi>::elements[duration].get(level);
}

static void turn_undead(creature* caster, creature* want_target, const effecti& e, int level, ability_s save_skill) {
	auto ti = maptbl(turn_undead_index, level);
	auto result = rand() % 20 + 1;
	auto index = caster->getindex();
	auto dir = caster->getdirection();
	creature* targets[4];
	location.getmonsters(targets, to(index, dir), dir);
	for(int i = 0; i < 4; i++) {
		auto target = targets[i];
		if(!target)
			continue;
		if(!target->is(Undead))
			continue;
		auto hd = target->gethd();
		if(hd > 0)
			hd -= 1;
		auto tt = maptbl(turn_undead_chance, hd);
		auto value = tt[ti];
		if(value > 20)
			continue;
		if(value < 0) {
			target->damage(Magic, target->gethitsmaximum() + 10);
			continue;
		}
		if(result >= value)
			value = 0;
		if(value == 0)
			target->add(Fear, xrand(2, 6) * 10);
	}
}

static void purify_food(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	for(auto i = FirstInvertory; i <= LastInvertory; i = (wear_s)(i + 1)) {
		auto pi = target->getitem(i);
		if(!pi)
			continue;
		if(pi->gettype() != Ration && pi->gettype() != RationIron)
			continue;
		pi->setbroken(0);
	}
}

static void lay_on_hands(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->damage(Heal, 2 * player->get(Paladin));
}

static void aid_spell(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->addaid(xrand(1, 8));
	auto duration = getduration(DurationHour, level);
	target->add(Bless, duration);
}

static void create_food(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	player->add(RationIron);
}

static void cure_deafness(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->remove(Blindness);
	target->remove(Deafness);
}

static void cure_disease(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->remove(Disease);
}

//static void cast_on_items(spell_s id, int level, creature* caster, bool whole_party, int count) {
//	itema result;
//	if(whole_party) {
//		for(auto v : party) {
//			auto p = v.getcreature();
//			if(!p)
//				continue;
//			p->select(result);
//		}
//	} else
//		caster->select(result);
//	auto pb = result.data;
//	for(auto p : result) {
//		if(!p->cast(caster, id, level, false, false))
//			continue;
//		*pb++ = p;
//	}
//	result.count = pb - result.data;
//	zshuffle(result.data, result.count);
//	if(!result)
//		return;
//	caster->say(id);
//	for(auto p : result) {
//		if(count-- <= 0)
//			break;
//		p->cast(caster, id, level, true, true);
//	}
//}

static bool identify_item(creature* player, item& target, const effecti& e, int level, ability_s save_skill, bool run) {
	return true;
}

static void identify(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->identify(true);
}

static void mending(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->mending(true);
}

static void knock(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	auto index = game.getcamera();
	auto direction = game.getdirection();
	auto pe = location.getoverlay(index, direction);
	if(!pe)
		return;
	if(pe->type == CellKeyHole1 || pe->type == CellKeyHole2) {
		if(pe->is(Active))
			return;
		pe->set(Active);
		mslog("Lock is magically open");
	}
}

static void remove_curse(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->uncurse();
}

static void remove_parasizes(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	target->remove(HoldPerson);
}

static void acid_arrow(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	auto th = player->getthac0(Fighter, level);
	auto rs = rand() % 20 + 1;
	if(rs < (th - target->getac()))
		return;
	target->add(AcidArrow, 1 + level / 3, NoSave);
}

spelli bsdata<spelli>::elements[] = {{"No spell", {0, 0}, TargetSelf, {0}},
// 1 - level
{"Bless", {0, 1}, TargetAllAlly, {Bless, DurationHour}},
{"Burning Hands", {1, 0}, TargetAllClose, {Fire, {1, 3}, {2}, 1, 10, SaveHalf}, FireThrown},
{"Cure light Wounds", {0, 1}, TargetAlly, {Heal, {1, 8}, {0}, 0, 0, NoSave}},
{"Detect Evil", {2, 1}, TargetAllAlly, {DetectEvil, DurationTurn}},
{"Detect Magic", {1, 1}, TargetAllAlly, {DetectMagic, DurationTurn}},
{"Cause Fear", {3, 1}, TargetAllClose, {Fear, Duration1PerLevel, SaveNegate}},
{"Feather Fall", {1, 0}, TargetAlly, {FeatherFall, DurationTurnPerLevel}},
{"Identify", {1, 0}, TargetSelf, identify},
{"Mage Armor", {1, 0}, TargetSelf, {MageArmor, Duration4Hours}},
{"Magic Missile", {1, 0}, TargetThrow, {Magic, {1, 4, 1}, {1, 4, 1}, 2, 4}, MagicThrown},
{"Mending", {1, 0}, TargetAllAlly, mending},
{"Prot. from Evil", {1, 1}, TargetAlly, {ProtectionFromEvil, DurationTurn}},
{"Purify food", {0, 1}, TargetAllAlly, purify_food},
{"Read Languages", {1, 0}, TargetSelf, {ReadLanguagesSpell, DurationHour}},
{"Resist Cold", {0, 1}, TargetAlly, {ResistColdSpell, DurationTurnPerLevel}},
{"Shield", {1, 0}, TargetSelf, {ShieldSpell, Duration5PerLevel}},
{"Shoking grasp", {1, 0}, TargetSelf, ShokingHand},
{"Sleep", {1, 0}, TargetAllClose, {Sleep, Duration5PerLevel, SaveNegate}},
// 2 - level
{"Acid arrow", {2}, TargetThrow, acid_arrow, Arrow},
{"Aid", {0, 2}, TargetAlly, aid_spell},
{"Blindness", {2}, TargetClose, {Blindness, Instant, SaveNegate}},
{"Blur", {2}, TargetSelf, {Blur, Duration5PerLevel}},
{"Deafness", {2}, TargetClose, {Deafness, Instant, SaveNegate}},
{"Flame Blade", {0, 2}, TargetSelf, FlameBladeHand},
{"Flaming sphere", {2}, TargetAllThrow, {Fire, {2, 4}, {}, 0, 0, SaveNegate}, FireThrown},
{"Goodberry", {0, 2}, TargetAllAlly, {Heal, {1, 4}, {0}, 0, 0, NoSave}},
{"Hold Person", {0, 2}, TargetAllClose, {HoldPerson, Duration1PerLevel, SaveNegate}, MagicThrown},
{"Invisibility", {2}, TargetAlly, {Invisibility, Duration2Hours}},
{"Knock", {2}, TargetSpecial, knock},
{"Produce Flame", {0, 2}, TargetSelf, FlameHand},
{"Resist fire", {0, 2}, TargetAlly, {ResistFireSpell, DurationTurnPerLevel}},
{"Slow Poison", {0, 2}, TargetAlly, {SlowPoison, Duration8Hours}},
// 3 - level
{"Create Food", {0, 3}, TargetSpecial, create_food},
{"Cure Blindness", {0, 3}, TargetAlly, cure_deafness},
{"Cause Disease", {0, 3}, TargetClose, {Disease, Instant, SaveNegate}},
{"Cure Disease", {0, 3}, TargetAlly, cure_disease},
{"Haste", {3, 0}, TargetAlly, {Haste, Duration5PerLevel}},
{"Protected negative", {0, 3}, TargetAlly, {NegativePlanProtection, Duration4Hours}},
{"Remove curse", {0, 3}, TargetAlly, remove_curse},
{"Remove paralizes", {0, 3}, TargetAllAlly, remove_parasizes},
// 4 - level
{"Cause Poison", {0, 3}, TargetClose, {Poison, Instant, SaveNegate}},
// Special ability
{"Lay on Hands", {0, 1}, TargetAlly, lay_on_hands},
{"Turn Undead", {0, 1}, TargetSpecial, turn_undead, MagicThrown},
};
assert_enum(spell, TurnUndead);

int	creature::getlevel(spell_s id, class_s type) {
	switch(type) {
	case Cleric: return bsdata<spelli>::elements[id].levels[1];
	default: return bsdata<spelli>::elements[id].levels[0];
	}
}

void effecti::apply_effect(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	auto duration = getduration(e.duration, level);
	target->add((spell_s)e.type.value, duration, e.save, e.save_bonus, save_skill);
}

static bool test_save(creature* target, int& value, ability_s skill, save_s type, int bonus) {
	switch(type) {
	case SaveHalf:
		if(target->roll(skill, bonus))
			value = value / 2;
		break;
	case SaveNegate:
		if(target->roll(skill, bonus))
			return true;
		break;
	}
	return false;
}

void effecti::apply_weapon(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	switch(e.type.type) {
	case Item:
		target->setweapon((item_s)e.type.value, xrand(1, 4) + level);
		break;
	}
}

void effecti::apply_damage(creature* player, creature* target, const effecti& e, int level, ability_s save_skill) {
	auto value = e.damage.roll();
	if(e.damage_increment) {
		auto mi = level / e.damage_increment;
		if(mi > e.damage_maximum)
			mi = e.damage_maximum;
		for(int i = 0; i < mi; i++)
			value += e.damage_per.roll();
	}
	if(!test_save(target, value, save_skill, e.save, 0))
		target->damage((damage_s)e.type.value, value);
}

short unsigned get_enemy_distance(short unsigned index, direction_s dir, item_s throw_effect) {
	int distance = 0;
	while(distance < 3) {
		if(throw_effect)
			draw::animation::thrownstep(index, dir, throw_effect, Center, 70);
		index = to(index, dir);
		if(location.isblocked(index))
			return Blocked;
		if(location.ismonster(index))
			return index;
		distance++;
	}
	return 0;
}

void creature::say(spell_s id) const {
	char temp[260];
	mslog("%1 cast %2", getname(temp, zendof(temp)), getstr(id));
}

bool creature::cast(spell_s id, class_s type, int wand_magic, creature* target) {
	creature* targets[4];
	auto& si = bsdata<spelli>::elements[id];
	auto index = getindex();
	auto dir = getdirection();
	auto range = si.range;
	auto spell_level = getlevel(id, type);
	if(!spell_level)
		spell_level = getlevel(id, Mage);
	if(!spell_level)
		spell_level = getlevel(id, Cleric);
	auto level = getcasterlevel(type);
	if(!level)
		level = gethd();
	if(wand_magic)
		level = (spell_level + wand_magic - 1) * 2 - 1;
	if(!spell_level)
		return false;
	if(is(Deafness) && (d100() < 20)) {
		if(ishero())
			mslog("Spell failed!");
	}
	auto save_skill = SaveVsMagic;
	if(wand_magic)
		save_skill = SaveVsTraps;
	switch(range) {
	case TargetSelf:
		say(id);
		si.effect.proc(this, this, si.effect, level, save_skill);
		break;
	case TargetAllAlly:
		say(id);
		for(auto v : party) {
			auto p = v.getcreature();
			if(!p)
				continue;
			si.effect.proc(this, p, si.effect, level, save_skill);
		}
		break;
	case TargetAlly:
		if(!target)
			target = choosehero();
		if(!target)
			return false;
		say(id);
		si.effect.proc(this, target, si.effect, level, save_skill);
		break;
	case TargetAllThrow:
		index = get_enemy_distance(index, dir, si.throw_effect);
		if(index == Blocked)
			return false;
		index = to(index, to(dir, Down));
		// Continue this case
	case TargetAllClose:
		//if(range == TargetAllClose && spell_data[spell].throw_effect)
		//	draw::animation::thrownstep(index, dir, spell_data[spell].throw_effect);
		location.getmonsters(targets, to(index, dir), dir);
		say(id);
		for(auto e : targets) {
			if(!e)
				continue;
			si.effect.proc(this, e, si.effect, level, save_skill);
		}
		break;
	case TargetThrow:
		index = get_enemy_distance(index, dir, si.throw_effect);
		if(index == Blocked)
			return false;
		index = to(index, to(dir, Down));
		// Continue this case
	case TargetClose:
		target = game.getdefender(to(index, dir), dir, this);
		if(!target)
			return false;
		say(id);
		si.effect.proc(this, target, si.effect, level, save_skill);
		break;
	case TargetSpecial:
		say(id);
		si.effect.proc(this, target, si.effect, level, save_skill);
		break;
	}
	if(wand_magic == 0) {
		if(get(id) > 0)
			set(id, get(id) - 1);
	}
	// RULE: When casting you gain experience
	int exp = 20 * spell_level;
	if(wand_magic)
		exp /= 2;
	else if(type == Mage)
		exp *= 3;
	addexp(exp);
	return true;
}