#include "main.h"

static void instant_creature_spell(spell_s spell) {
	for(auto p : party)
		p->apply(spell, 20);
}

static bool duration_creature_spell(spell_s spell, bool run) {
	if(game.iseffect(spell))
		return false;
	if(run) {
		for(auto p : party)
			p->add(spell, 3 * 24 * 60, NoSave, 0);
	}
	return true;
}

static bool instant_item_spell(spell_s spell, bool run) {
	return game.enchant(spell, 20, run);
}

static bool instant_creature_spell(spell_s spell, spell_s dispell, bool run) {
	if(!game.iseffect(dispell))
		return false;
	if(run) {
		for(auto p : party)
			p->apply(spell, 20);
	}
	return true;
}

static bool apply_miracle(spell_s spell, bool run) {
	switch(spell) {
	case Bless:
	case DetectEvil:
	case DetectMagic:
	case ResistColdSpell:
	case Haste:
	case FeatherFall:
	case MageArmor:
		return duration_creature_spell(spell, run);
	case Identify:
	case Mending:
		return instant_item_spell(spell, run);
	case CureLightWounds:
	case CureSeriousWounds:
		if(!game.is(Healed))
			return false;
		if(run)
			instant_creature_spell(spell);
		break;
	case NeutralizePoison:
		return instant_creature_spell(spell, Poison, run);
	case RemoveParalizes:
		return instant_creature_spell(spell, HoldPerson, run);
	case RaiseStrenght: case RaiseConstitution: case RaiseDexterity:
	case RaiseIntellect: case RaiseWisdow: case RaiseCharisma:
		if(run)
			instant_creature_spell(spell);
		break;
	case GainExperience:
		if(run)
			instant_creature_spell(spell);
		break;
	default:
		return false;
	}
	return true;
}

struct miraclei {
	spell_s		spell;
	const char*	text;
};
static miraclei small_miracles[] = {
	{ResistColdSpell, "You feel winter breath"},
	{Identify, "Sundelly you known all items power in your backpacks!"},
	{CureSeriousWounds, "You wound is healing magical means"},
	{NeutralizePoison, "Poison stop run in your veins"},
	{GainExperience, "You feel powerfull knowledge insight"},
};

static const miraclei* random_miracle(const aref<miraclei>& source) {
	adat<const miraclei*> collections;
	for(auto& e : source) {
		if(!apply_miracle(e.spell, false))
			continue;
		collections.add(&e);
	}
	if(!collections)
		return 0;
	return collections.data[rand() % collections.getcount()];
}

void add_small_miracle() {
	auto p = random_miracle(small_miracles);
	if(p) {
		apply_miracle(p->spell, true);
		if(p->text)
			mslog(p->text);
	}
}