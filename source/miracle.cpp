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

static bool healing(int param, bool run) {
	if(game.is(Healed))
		return true;
	if(run)
		game.each(&creature::healing);
	return true;
}

static bool cold_resistance(int param, bool run) {
	if(game.iseffect(ResistColdSpell))
		return false;
	game.addspell(ResistColdSpell, 60 * 24 * 3);
	return true;
}

static bool remove_poison(int param, bool run) {
	if(!game.iseffect(Poison))
		return false;
	if(run)
		remove_spell(Poison);
	return true;
}

static miraclei small_miracles[] = {
	{"ColdResistance", cold_resistance},
	{"Healing", healing},
	{"RemovePoison", remove_poison},
};

static const miraclei* random_miracle(const aref<miraclei>& source) {
	adat<const miraclei*> collections;
	for(auto& e : source) {
		if(!e.proc(0, false))
			continue;
		collections.add(&e);
	}
	if(!collections)
		return 0;
	return collections.data[rand() % collections.getcount()];
}

void add_small_miracle() {
	auto p = random_miracle(small_miracles);
	if(p)
		p->proc(0, true);
}