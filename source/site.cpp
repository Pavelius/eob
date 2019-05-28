#include "main.h"

static sitei::headi elements[] = {{BRICK, {Kobold, Leech}, {KeySilver, KeyCooper}, Human},
{BLUE, {Goblin, Orc}, {KeySilver, KeyMoon}, Dwarf},
{SILVER, {Goblin, Orc}, {KeyDiamond, KeySilver}, Human},
};

const sitei::headi& sitei::get(resource_s type) {
	for(auto& e : elements) {
		if(e.type == type)
			return e;
	}
	return elements[0];
}

unsigned sitei::getleveltotal() const {
	unsigned r = 0;
	for(auto p = this; *p; p++)
		r += p->levels;
	return r;
}

void sitei::headi::apply(sitei::headi& ev) const {
	auto& def = get(type);
	ev.type = type;
	for(int i = 0; i < sizeof(habbits) / sizeof(habbits[0]); i++) {
		if(habbits[i])
			ev.habbits[i] = habbits[i];
		else
			ev.habbits[i] = def.habbits[i];
	}
	for(int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
		if(keys[i])
			ev.keys[i] = keys[i];
		else
			ev.keys[i] = def.keys[i];
	}
	ev.language = language;
	if(!ev.language)
		ev.language = def.language;
}