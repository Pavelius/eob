#include "main.h"

static void remove_spell(spell_s v) {
	for(auto& e : bsdata<boosti>()) {
		if(e.id != v)
			continue;
		auto p = e.owner.getcreature();
		if(!p || !p->ishero())
			continue;
		e.clear();
	}
}

static bool healing(bool run) {
	if(game.is(Healed))
		return true;
	if(run) {
		for(auto p : party)
			p->healing();
	}
	return true;
}

static bool cold_resistance(bool run) {
	if(game.iseffect(ResistColdSpell))
		return false;
	if(run)
		game.addspell(ResistColdSpell, 60 * 24 * 3);
	return true;
}

static bool remove_poison(bool run) {
	if(!game.iseffect(Poison))
		return false;
	if(run)
		remove_spell(Poison);
	return true;
}

static bool identify_items(bool run) {
	if(!game.enchant(Identify, 1, false))
		return false;
	if(run)
		game.enchant(Identify, 1, true);
	return true;
}

static miraclei small_miracles[] = {
	{"ColdResistance", cold_resistance, "You feel winter breath"},
	{"Identifying", identify_items, "Sundelly you known all items power in your backpacks!"},
	{"Healing", healing, "You wound is healing magical means"},
	{"RemovePoison", remove_poison, "Poison stop run in your veins"},
};

static const miraclei* random_miracle(const aref<miraclei>& source) {
	adat<const miraclei*> collections;
	for(auto& e : source) {
		if(!e.proc(false))
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
		p->proc(true);
		if(p->text)
			mslog(p->text);
	}
}