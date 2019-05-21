#include "main.h"

spell_info bsmeta<spell_info>::elements[] = {{"No spell"},
// 1 - level
{"Bless", {0, 1}, TargetAllAlly, DurationHour, StateBlessed},
{"Burning Hands", {1}, TargetAllClose, Instant, NoState, SaveHalf, {{1, 3}, 1, {2}, 10}, FireThrown},
{"Cure light Wounds", {0, 1}, TargetAlly, Instant, NoState, NoSave, {{1, 8, 1}}},
{"Detect Evil", {2, 1}, TargetAllAlly, DurationTurn, StateDetectedEvil},
{"Detect Magic", {1, 1}, TargetAllAlly, DurationTurn, StateDetectedMagic},
{"Mage Armor", {1}, TargetSelf, Duration4Hours, StateArmored},
{"Magic Missile", {1}, TargetThrow, Instant, NoState, NoSave, {{1, 4, 1}, 2, {1, 4, 1}, 4}, MagicThrown},
{"Prot. from Evil", {1, 1}, TargetAlly, DurationTurn, StateProtectedVsEvil},
{"Purify food", {0, 1}, TargetAllAlly, Instant},
{"Read Languages", {1}, TargetSelf, DurationTurn, StateSpeakable},
{"Shield", {1}, TargetSelf, Duration5PerLevel, StateShielded},
{"Sleep", {1}, TargetAllClose, Duration5PerLevel, StateSleeped},
// Special ability
{"Lay on Hands", {0, 1}, TargetAlly, Instant},
{"Turn Undead", {0, 1}, TargetAllClose, DurationTurn, StateTurned},
};
assert_enum(spell, TurnUndead);

int game::get(duration_s duration, int level) {
	const int hour = 60;
	switch(duration) {
	case Duration5PerLevel: return 5 * level;
	case DurationTurn: return 10;
	case DurationTurnPerLevel: return 10 * level;
	case DurationHour: return 1 * hour;
	case Duration2Hours: return 2 * hour;
	case Duration4Hours: return 4 * hour;
	case Duration8Hours: return 8 * hour;
	default: return 0;
	}
}

int game::getspelllevel(spell_s spell, class_s type) {
	switch(type) {
	case Cleric: return bsmeta<spell_info>::elements[spell].levels[1];
	default:
		return bsmeta<spell_info>::elements[spell].levels[0];
	}
}

static int get_spell_value(spell_s spell, int level, creature* target, save_s save, int wand_magic) {
	auto& e = bsmeta<spell_info>::elements[spell].number;
	auto value = e.base.roll();
	if(e.level) {
		auto mi = level / e.level;
		for(int i = 1; i < mi; i++)
			value += e.perlevel.roll();
	}
	auto save_skill = SaveVsMagic;
	if(wand_magic)
		save_skill = SaveVsTraps;
	switch(save) {
	case SaveHalf:
		if(target->roll(save_skill))
			value = value / 2;
		break;
	case SaveNegate:
		if(target->roll(save_skill))
			value = -1;
		break;
	}
	return value;
}

static void apply_spell_effect(creature* caster, creature* target, spell_s spell, class_s cls, int level, int wand_magic) {
	auto& si = bsmeta<spell_info>::elements[spell];
	auto duration = si.duration;
	auto save = si.save;
	auto effect = si.effect;
	auto spell_level = game::getspelllevel(spell, cls);
	auto value = get_spell_value(spell, level, target, save, wand_magic);
	if(value < 0)
		return;
	// Apply effect if any
	if(effect)
		target->set(effect, game::get(duration, level));
	// Some spell has special cases
	switch(spell) {
	case SpellCureLightWounds:
		target->damage(-value);
		break;
	case SpellPurifyFood:
		for(auto i = FirstInvertory; i <= LastInvertory; i = (wear_s)(i + 1)) {
			auto pi = target->getitem(i);
			if(!pi)
				continue;
			if(pi->gettype() != Ration && pi->gettype() != RationIron)
				continue;
			pi->setbroken(0);
		}
		break;
	default:
		if(value)
			target->damage(value);
		break;
	}
}

static void mscast(creature* caster, spell_s spell) {
	char temp[260];
	mslog("%1 cast %2", caster->getname(temp, zendof(temp)), getstr(spell));
}

short unsigned get_enemy_distance(short unsigned index, direction_s dir, item_s throw_effect) {
	int distance = 0;
	while(distance < 3) {
		if(throw_effect)
			draw::animation::thrownstep(index, dir, throw_effect, Center, 70);
		index = moveto(index, dir);
		if(location.isblocked(index))
			return Blocked;
		if(location.ismonster(index))
			return index;
		distance++;
	}
	return 0;
}

bool game::action::cast(creature* caster, spell_s spell, class_s cls, creature* want_target, int wand_magic) {
	auto range = bsmeta<spell_info>::elements[spell].range;
	auto spell_level = getspelllevel(spell, cls);
	if(!spell_level)
		spell_level = getspelllevel(spell, Mage);
	if(!spell_level)
		spell_level = getspelllevel(spell, Cleric);
	auto level = caster->get(cls);
	if(wand_magic)
		level = (spell_level + wand_magic - 1) * 2 - 1;
	if(!level || !spell_level)
		return false;
	auto index = caster->getindex();
	auto dir = caster->getdirection();
	creature* target = 0;
	creature* targets[4];
	switch(range) {
	case TargetSelf:
		mscast(caster, spell);
		apply_spell_effect(caster, caster, spell, cls, level, wand_magic);
		break;
	case TargetAllAlly:
		mscast(caster, spell);
		for(auto e : game::party) {
			if(!e)
				continue;
			apply_spell_effect(caster, e, spell, cls, level, wand_magic);
		}
		break;
	case TargetAlly:
		if(want_target)
			target = want_target;
		else
			target = game::action::choosehero();
		if(!target)
			return false;
		mscast(caster, spell);
		apply_spell_effect(caster, target, spell, cls, level, wand_magic);
		break;
	case TargetAllThrow:
		index = get_enemy_distance(index, dir, bsmeta<spell_info>::elements[spell].throw_effect);
		if(index == Blocked)
			return false;
		index = moveto(index, rotateto(dir, Down));
		// Continue this case
	case TargetAllClose:
		//if(range == TargetAllClose && spell_data[spell].throw_effect)
		//	draw::animation::thrownstep(index, dir, spell_data[spell].throw_effect);
		location.getmonsters(targets, moveto(index, dir), dir);
		mscast(caster, spell);
		for(auto e : targets) {
			if(!e)
				continue;
			apply_spell_effect(caster, e, spell, cls, level, wand_magic);
		}
		break;
	case TargetThrow:
		index = get_enemy_distance(index, dir, bsmeta<spell_info>::elements[spell].throw_effect);
		if(index == Blocked)
			return false;
		index = moveto(index, rotateto(dir, Down));
		// Continue this case
	case TargetClose:
		target = game::getdefender(moveto(index, dir), dir, caster);
		if(target) {
			mscast(caster, spell);
			apply_spell_effect(caster, target, spell, cls, level, wand_magic);
		}
		break;
	}
	if(wand_magic == 0) {
		int cv = caster->get(spell);
		if(cv)
			caster->set(spell, cv - 1);
	}
	// RULE: When casting you gain experience
	int exp = 20 * spell_level;
	if(wand_magic)
		exp /= 2;
	else if(cls == Mage)
		exp *= 3;
	caster->addexp(exp);
	return true;
}