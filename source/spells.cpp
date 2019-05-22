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

static void turn_undead(creature* caster, creature** targets, const effecti& e, int level) {
	auto ti = maptbl(turn_undead_index, level);
	auto result = d20();
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
		if(value = 0)
			target->set(Scared, xrand(2, 6) * 10);
	}
}

static void purify_food(creature* player, creature* target, const effecti& e, int level) {
	for(auto i = FirstInvertory; i <= LastInvertory; i = (wear_s)(i + 1)) {
		auto pi = target->getitem(i);
		if(!pi)
			continue;
		if(pi->gettype() != Ration && pi->gettype() != RationIron)
			continue;
		pi->setbroken(0);
	}
}

static void lay_on_hands(creature* player, creature* target, const effecti& e, int level) {
}

spelli bsmeta<spelli>::elements[] = {{"No spell", {0, 1}, TargetSelf, {0, 0}},
// 1 - level
{"Bless", {0, 1}, TargetAllAlly, {DurationHour, Blessed}},
{"Burning Hands", {1, 0}, TargetAllClose, {Fire, {1, 3}, {2}, 1, 10, SaveHalf}, FireThrown},
{"Cure light Wounds", {0, 1}, TargetAlly, {Heal, {1, 8}, {0}}},
{"Detect Evil", {2, 1}, TargetAllAlly, {DurationTurn, DetectedEvil}},
{"Detect Magic", {1, 1}, TargetAllAlly, {DurationTurn, DetectedMagic}},
{"Feather Fall", {1, 0}, TargetAlly, {DurationTurnPerLevel, Climbed}},
{"Mage Armor", {1, 0}, TargetSelf, {Duration4Hours, Armored}},
{"Magic Missile", {1, 0}, TargetThrow, {Magic, {1, 4, 1}, {1, 4, 1}, 2, 4}, MagicThrown},
{"Prot. from Evil", {1, 1}, TargetAlly, {DurationTurn, ProtectedFromEvil}},
{"Purify food", {0, 1}, TargetAllAlly, {purify_food}},
{"Read Languages", {1, 0}, TargetSelf, {DurationTurn, StateSpeakable}},
{"Shield", {1, 0}, TargetSelf, {Duration5PerLevel, Shielded}},
{"Sleep", {1, 0}, TargetAllClose, {Duration5PerLevel, Sleeped}},
// Special ability
{"Lay on Hands", {0, 1}, TargetAlly, {lay_on_hands}},
{"Turn Undead", {0, 1}, TargetAllClose, {0, turn_undead}, MagicThrown},
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
	case Cleric: return bsmeta<spelli>::elements[spell].levels[1];
	default:
		return bsmeta<spelli>::elements[spell].levels[0];
	}
}

static int get_spell_value(spell_s spell, int level, creature* target, save_s save, int wand_magic) {
	//auto& e = bsmeta<spelli>::elements[spell].number;
	//auto value = e.base.roll();
	//if(e.level) {
	//	auto mi = level / e.level;
	//	for(int i = 1; i < mi; i++)
	//		value += e.perlevel.roll();
	//}
	//auto save_skill = SaveVsMagic;
	//if(wand_magic)
	//	save_skill = SaveVsTraps;
	//switch(save) {
	//case SaveHalf:
	//	if(target->roll(save_skill))
	//		value = value / 2;
	//	break;
	//case SaveNegate:
	//	if(target->roll(save_skill))
	//		value = -1;
	//	break;
	//}
	return 0;
}

static void apply_spell_effect(creature* caster, creature* target, spell_s spell, class_s cls, int level, int wand_magic) {
	auto& si = bsmeta<spelli>::elements[spell];
	//auto duration = si.duration;
	//auto save = si.save;
	//auto effect = si.effect;
	//auto spell_level = game::getspelllevel(spell, cls);
	//auto value = get_spell_value(spell, level, target, save, wand_magic);
	//if(value < 0)
	//	return;
	//// Apply effect if any
	//if(effect)
	//	target->set(effect, game::get(duration, level));
	//// Some spell has special cases
	//switch(spell) {
	//case SpellCureLightWounds:
	//	target->damage(Magic, -value);
	//	break;
	//case LayOnHands:
	//	target->damage(Magic, -caster->gethd()*2);
	//	break;
	//case SpellPurifyFood:
	//	break;
	//default:
	//	if(value)
	//		target->damage(si.damage_type, value);
	//	break;
	//}
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
	auto range = bsmeta<spelli>::elements[spell].range;
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
	auto& si = bsmeta<spelli>::elements[spell];
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
		index = get_enemy_distance(index, dir, si.throw_effect);
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
		index = get_enemy_distance(index, dir, si.throw_effect);
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