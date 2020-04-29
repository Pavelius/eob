#include "main.h"

static messagei dragon_text[] = {{1, {Indifferent}, " - Are you from master?"},
{1, {Indifferent}, " - Wait a minute! How you can dig so deep?"},
{3, {}, " - Who is you? You are not from this place."},
{12, {}, " - Hello, friends. Glad to see you. What can we do for you?"},
{13, {}, " - You liers! Prepare to die!", StartCombat},
};
static messagei dragon_quest[] = {{1, {StartCombat}, "Attack"},
{1, {Charisma}, "Lie", {12, 13}},
{12, {Trading}, "Trade"},
};

static bool allowed(creature& player, const variant& v) {
	switch(v.type) {
	case Alignment: return player.getalignment() == v.value;
	case Class: return player.get((class_s)v.value) > 0;
	case Gender: return player.getgender() == v.value;
	case Race: return player.getrace() == v.value;
	}
	return false;
}

static bool allowed(const variant& v) {
	for(auto vc : party) {
		auto p = vc.getcreature();
		if(!p)
			continue;
		if(allowed(*p, v))
			return true;
	}
	return false;
}

bool messagei::isallow() const {
	for(auto v : variants) {
		if(!v)
			break;
	}
	return true;
}