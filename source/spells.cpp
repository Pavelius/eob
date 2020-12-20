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

static bool cast_on_items(spell_s id, int level, creature* caster, itema& result) {
	auto& si = bsdata<spelli>::elements[id];
	auto count = si.effect.damage.roll();
	if(!count)
		count = 1;
	auto say_success = si.talk;
	auto pb = result.data;
	for(auto p : result) {
		if(!p->cast(id, level, false))
			continue;
		*pb++ = p;
	}
	result.count = pb - result.data;
	zshuffle(result.data, result.count);
	if(!result)
		return false;
	for(auto p : result) {
		if(count-- <= 0)
			break;
		p->cast(id, level, true);
		if(say_success) {
			char temp[260]; stringbuilder sb(temp); p->getname(sb);
			auto pt = say_success[rand() % 3];
			caster->say(pt, temp);
		}
	}
	return true;
}

static bool cast_on_party_items(spell_s id, int level, creature* caster) {
	itema result;
	for(auto v : party) {
		auto p = v.getcreature();
		if(!p)
			continue;
		p->select(result);
	}
	return cast_on_items(id, level, caster, result);
}

static bool cast_on_ally_items(spell_s id, int level, creature* caster, creature* target) {
	itema result;
	target->select(result);
	return cast_on_items(id, level, caster, result);
}

static bool cast_on_own_items(spell_s id, int level, creature* caster) {
	itema result;
	caster->select(result);
	return cast_on_items(id, level, caster, result);
}

BSDATA(spelli) = {{"No spell", {0, 0}, TargetSelf, {}},
{"Bless", {0, 1}, TargetAlly, {Bless, DurationHour}},
{"Burning Hands", {1, 0}, TargetAllClose, {Fire, Instant, SaveHalf, 0, {1, 3}, {2}, 1, 10}, FireThrown},
{"Cure light Wounds", {0, 1}, TargetAlly, {Heal, Instant, NoSave, 0, {1, 8}}},
{"Detect Evil", {2, 1}, TargetAllAlly, {DetectEvil, DurationTurn}},
{"Detect Magic", {1, 1}, TargetAllAlly, {DetectMagic, DurationTurn}},
{"Cause Fear", {3, 1}, TargetAllClose, {Fear, Duration1PerLevel, SaveNegate}},
{"Feather Fall", {1, 0}, TargetAlly, {FeatherFall, DurationTurnPerLevel}},
{"Identify", {1, 0}, TargetAllyItems, {Identify}, {}, {"I know! It's a %1.", "Wait a minute! It's %1.", "Very well! This is %1"}},
{"Mage Armor", {1, 0}, TargetSelf, {MageArmor, Duration8Hours}},
{"Magic Missile", {1, 0}, TargetThrow, {Magic, Instant, NoSave, 0, {1, 4, 1}, {1, 4, 1}, 2, 4}, MagicThrown},
{"Mending", {1, 0}, TargetItems, {Mending, Instant, NoSave, 0, {1, 3, 1}}, {}, {"%1 as a new", "%1 is fixed", "%1 is repaired"}},
{"Prot. from Evil", {1, 1}, TargetAlly, {ProtectionFromEvil, DurationTurn}},
{"Purify food", {0, 1}, TargetAllAllyItems, {PurifyFood, Instant, NoSave, 0, {100}}, {}, {"%1 is purified", "%1 clean from poison", "%1 is safe to eat"}},
{"Read Languages", {1, 0}, TargetSelf, {ReadLanguagesSpell, DurationHour}},
{"Resist Cold", {0, 1}, TargetAlly, {ResistColdSpell, DurationTurnPerLevel}},
{"Shield", {1, 0}, TargetSelf, {ShieldSpell, Duration5PerLevel}},
{"Shoking grasp", {1, 0}, TargetSelf, {ShokingHand}},
{"Sleep", {1, 0}, TargetAllClose, {Sleep, Duration5PerLevel, SaveNegate}},

{"Acid arrow", {2}, TargetThrowHitFighter, {Pierce, Instant, NoSave, 0, {2, 4}}, Arrow},
{"Aid", {0, 2}, TargetAlly, {Aid}},
{"Blindness", {2}, TargetClose, {Blindness, Instant, SaveNegate}},
{"Blur", {2}, TargetSelf, {Blur, Duration5PerLevel}},
{"Deafness", {2}, TargetClose, {Deafness, Instant, SaveNegate}},
{"Flame Blade", {0, 2}, TargetSelf, FlameBladeHand},
{"Flaming sphere", {2}, TargetAllThrow, {Fire, Instant, SaveNegate, 0, {2, 4}}, FireThrown},
{"Goodberry", {0, 2}, TargetAllAlly, {Heal, Instant, NoSave, 0, {1, 4}}},
{"Hold Person", {0, 2}, TargetAllClose, {HoldPerson, Duration1PerLevel, SaveNegate}, MagicThrown},
{"Invisibility", {2}, TargetAlly, {Invisibility, Duration2Hours}},
{"Knock", {2}, TargetSelf, {Knock}},
{"Produce Flame", {0, 2}, TargetSelf, {FlameHand}},
{"Resist fire", {0, 2}, TargetAlly, {ResistFireSpell, DurationTurnPerLevel}},
{"Slow Poison", {0, 2}, TargetAlly, {SlowPoison, Duration8Hours}},
{"Scare", {2}, TargetAllClose, {Fear, Duration1d4P1PerLevel, NoSave}},

{"Create Food", {0, 3}, TargetSelf, {CreateFood}},
{"Cure Blindness", {0, 3}, TargetAlly, {CureBlindnessDeafness}},
{"Cause Disease", {0, 3}, TargetClose, {Disease, Instant, SaveNegate}},
{"Cure Disease", {0, 3}, TargetAlly, {CureDisease}},
{"Haste", {3, 0}, TargetAlly, {Haste, Duration5PerLevel}},
{"Protected negative", {0, 3}, TargetAlly, {NegativePlanProtection, Duration4Hours}},
{"Remove curse", {0, 3}, TargetAlly, {RemoveCurse}, {}, {"%1 cramble to dust", "%1 is uncursed", "%1 turn to ashe"}},
{"Remove paralizes", {0, 3}, TargetAllAlly, {RemoveParalizes}},

{"Cure Serious Wounds", {0, 4}, TargetAlly, {Heal, Instant, NoSave, 0, {2, 8, 1}}},
{"Cause Poison", {0, 4}, TargetClose, {Poison, Instant, SaveNegate}},

{"Lay on Hands", {0, 1}, TargetAlly, {Heal, Instant, NoSave, 0, {2}}},
{"Turn Undead", {0, 1}, TargetSelf, {TurnUndead}, MagicThrown},
};
assert_enum(spell, TurnUndead)
INSTELEM(spelli)

int	creature::getlevel(spell_s id, class_s type) {
	switch(type) {
	case Cleric: return bsdata<spelli>::elements[id].levels[1];
	default: return bsdata<spelli>::elements[id].levels[0];
	}
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
			mslog("Spell failed due to deafness!");
	}
	auto save_skill = SaveVsMagic;
	switch(range) {
	case TargetSelf:
		say(id);
		apply(id, level, 0);
		break;
	case TargetAllAlly:
		say(id);
		for(auto v : party) {
			auto p = v.getcreature();
			if(!p)
				continue;
			p->apply(id, level, 0);
		}
		break;
	case TargetAlly:
		if(!target)
			target = choosehero();
		if(!target)
			return false;
		say(id);
		target->apply(id, level, 0);
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
		for(auto p : targets) {
			if(!p)
				continue;
			p->apply(id, level, 0);
		}
		break;
	case TargetThrow:
	case TargetThrowHitFighter:
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
		if(range == TargetThrowHitFighter) {
			auto th = getthac0(Fighter, level);
			auto rs = rand() % 20 + 1;
			if(rs < (th - target->getac()))
				return false;
		}
		target->apply(id, level, 0);
		break;
	case TargetItems:
		say(id);
		cast_on_own_items(id, level, this);
		break;
	case TargetAllAllyItems:
		say(id);
		cast_on_party_items(id, level, this);
		break;
	case TargetAllyItems:
		if(!target)
			target = choosehero();
		if(!target)
			return false;
		say(id);
		cast_on_ally_items(id, level, this, target);
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

void creature::apply(spell_s id, int level, unsigned duration) {
	auto& ei = bsdata<spelli>::elements[id];
	switch(id) {
	case AcidArrow:
		add(AcidArrow, 1 + level / 3, NoSave);
		break;
	case Aid:
		addaid(xrand(1, 8));
		add(Bless, 60);
		break;
	case CureDisease:
		remove(Disease);
		break;
	case CureBlindnessDeafness:
		remove(Blindness);
		remove(Deafness);
		break;
	case CreateFood:
		add(RationIron);
		break;
	case Fear:
	case Scare:
		if(is(Undead) || gethd() >= 6)
			return;
		ei.effect.apply(this, level);
		break;
	case Knock:
		if(true) {
			auto index = getindex();
			auto dir = getdirection();
			auto pe = location.getoverlay(index, direction);
			if(!pe)
				return;
			if(pe->type == CellKeyHole1 || pe->type == CellKeyHole2) {
				if(pe->is(Active))
					return;
				pe->set(Active);
				say("Lock is open now!");
			}
		}
		break;
	case LayOnHands:
		damage(Heal, 2 * level);
		break;
	case RemoveCurse:
		uncurse(true);
		break;
	case RemoveParalizes:
		remove(HoldPerson);
		break;
	case TurnUndead:
		if(true) {
			if(!is(Undead))
				return;
			auto hd = gethd();
			if(hd > 0)
				hd -= 1;
			auto tt = maptbl(turn_undead_chance, hd);
			auto ti = maptbl(turn_undead_index, level);
			auto value = tt[ti];
			if(value > 20)
				return;
			if(value < 0) {
				kill();
				return;
			}
			auto result = rand() % 20 + 1;
			if(result >= value)
				value = 0;
			if(value == 0)
				add(Fear, xrand(2, 6) * 10);
		}
		break;
	default:
		ei.effect.apply(this, level);
		break;
	}
}

bool item::cast(spell_s id, int level, bool run) {
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
		if(!isbroken() || type == Ration || type == RationIron)
			return false;
		if(run)
			broken = false;
		break;
	case PurifyFood:
		if(!isbroken() || !(type == Ration || type == RationIron))
			return false;
		if(run)
			broken = false;
		break;
	case RemoveCurse:
		if(!iscursed())
			return false;
		if(run)
			cursed = false;
		break;
	default:
		return false;
	}
	return true;
}