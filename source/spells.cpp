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
	return bsmeta<durationi>::elements[duration].get(level);
}

static void turn_undead(creature* caster, creature* want_target, const effecti& e, int level, int wand_magic) {
	auto ti = maptbl(turn_undead_index, level);
	auto result = d20();
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
			target->set(Scared, xrand(2, 6) * 10);
	}
}

static void purify_food(creature* player, creature* target, const effecti& e, int level, int wand_magic) {
	for(auto i = FirstInvertory; i <= LastInvertory; i = (wear_s)(i + 1)) {
		auto pi = target->getitem(i);
		if(!pi)
			continue;
		if(pi->gettype() != Ration && pi->gettype() != RationIron)
			continue;
		pi->setbroken(0);
	}
}

static void lay_on_hands(creature* player, creature* target, const effecti& e, int level, int wand_magic) {
	target->damage(Heal, 2 * player->get(Paladin));
}

static void aid_spell(creature* player, creature* target, const effecti& e, int level, int wand_magic) {
	target->addaid(xrand(1, 8));
	auto duration = getduration(DurationHour, level);
	target->add(Blessed, duration, NoSave);
}

static void slow_poison(creature* player, creature* target, const effecti& e, int level, int wand_magic) {
	player->slowpoison();
}

spelli bsmeta<spelli>::elements[] = {{"No spell", {0, 0}, TargetSelf, {0}},
// 1 - level
{"Bless", {0, 1}, TargetAllAlly, {DurationHour, Blessed, NoSave}},
{"Burning Hands", {1, 0}, TargetAllClose, {Fire, {1, 3}, {2}, 1, 10, SaveHalf}, FireThrown},
{"Cure light Wounds", {0, 1}, TargetAlly, {Heal, {1, 8}, {0}, 0, 0, NoSave}},
{"Detect Evil", {2, 1}, TargetAllAlly, {DurationTurn, DetectedEvil, NoSave}},
{"Detect Magic", {1, 1}, TargetAllAlly, {DurationTurn, DetectedMagic, NoSave}},
{"Feather Fall", {1, 0}, TargetAlly, {DurationTurnPerLevel, Climbed, NoSave}},
{"Mage Armor", {1, 0}, TargetSelf, {Duration4Hours, Armored, NoSave}},
{"Magic Missile", {1, 0}, TargetThrow, {Magic, {1, 4, 1}, {1, 4, 1}, 2, 4}, MagicThrown},
{"Prot. from Evil", {1, 1}, TargetAlly, {DurationTurn, ProtectedFromEvil, NoSave}},
{"Purify food", {0, 1}, TargetAllAlly, {purify_food}},
{"Read Languages", {1, 0}, TargetSelf, {DurationTurn, StateSpeakable, NoSave}},
{"Shield", {1, 0}, TargetSelf, {Duration5PerLevel, Shielded, NoSave}},
{"Shoking grasp", {1, 0}, TargetSelf, ShokingHand},
{"Sleep", {1, 0}, TargetAllClose, {Duration5PerLevel, Sleeped, NoSave}},
// 2 - level
{"Aid", {0, 2}, TargetAlly, aid_spell},
{"Flame Blade", {0, 2}, TargetSelf, FlameBladeHand},
{"Hold Person", {0, 2}, TargetAllClose, {Duration1PerLevel, Paralized}},
{"Slow Poison", {0, 2}, TargetAlly, slow_poison},
// Special ability
{"Lay on Hands", {0, 1}, TargetAlly, {lay_on_hands}},
{"Turn Undead", {0, 1}, TargetSpecial, {turn_undead}, MagicThrown},
};
assert_enum(spell, TurnUndead);

int	creature::getlevel(spell_s id, class_s type) {
	switch(type) {
	case Cleric: return bsmeta<spelli>::elements[id].levels[1];
	default: return bsmeta<spelli>::elements[id].levels[0];
	}
}

void effecti::apply_effect(creature* player, creature* target, const effecti& e, int level, int wand_magic) {
	auto duration = getduration(e.duration, level);
	target->add(e.state, duration, e.save, e.save_bonus);
}

static bool test_save(creature* target, int& value, skill_s skill, save_s type, int bonus) {
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

void effecti::apply_weapon(creature* player, creature* target, const effecti& e, int level, int wand_magic) {
	//target->setweapon(e.item_weapon, level + xrand(4, 8));
	target->setweapon(e.item_weapon, 2);
}

void effecti::apply_damage(creature* player, creature* target, const effecti& e, int level, int wand_magic) {
	auto value = e.damage.roll();
	if(e.damage_increment) {
		auto mi = level / e.damage_increment;
		if(mi > e.damage_maximum)
			mi = e.damage_maximum;
		for(int i = 0; i < mi; i++)
			value += e.damage_per.roll();
	}
	auto save_skill = SaveVsMagic;
	if(wand_magic)
		save_skill = SaveVsTraps;
	if(!test_save(target, value, save_skill, e.damage_save, 0))
		target->damage(e.damage_type, value);
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
	auto& si = bsmeta<spelli>::elements[id];
	auto index = getindex();
	auto dir = getdirection();
	auto range = si.range;
	auto spell_level = getlevel(id, type);
	if(!spell_level)
		spell_level = getlevel(id, Mage);
	if(!spell_level)
		spell_level = getlevel(id, Cleric);
	auto level = get(type);
	if(wand_magic)
		level = (spell_level + wand_magic - 1) * 2 - 1;
	if(!level || !spell_level)
		return false;
	switch(range) {
	case TargetSelf:
		say(id);
		si.effect.proc(this, this, si.effect, level, wand_magic);
		break;
	case TargetAllAlly:
		say(id);
		for(auto e : game::party) {
			if(!e)
				continue;
			si.effect.proc(this, e, si.effect, level, wand_magic);
		}
		break;
	case TargetAlly:
		if(!target)
			target = game::action::choosehero();
		if(!target)
			return false;
		say(id);
		si.effect.proc(this, target, si.effect, level, wand_magic);
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
			si.effect.proc(this, e, si.effect, level, wand_magic);
		}
		break;
	case TargetThrow:
		index = get_enemy_distance(index, dir, si.throw_effect);
		if(index == Blocked)
			return false;
		index = to(index, to(dir, Down));
		// Continue this case
	case TargetClose:
		target = game::getdefender(to(index, dir), dir, this);
		if(!target)
			return false;
		say(id);
		si.effect.proc(this, target, si.effect, level, wand_magic);
		break;
	case TargetSpecial:
		say(id);
		si.effect.proc(this, target, si.effect, level, wand_magic);
		break;
	}
	if(wand_magic == 0) {
		if(get(id)>0)
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