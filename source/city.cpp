#include "main.h"

static adventurei* last_quest;

static void choose_quest() {
	answers aw;
	for(auto& e : bsdata<adventurei>())
		aw.add((int)&e, e.getname());
	last_quest = (adventurei*)aw.choosems("Which way to go?");
}

static void enter_quest() {
	choose_quest();
	if(!last_quest)
		return;
#ifdef _DEBUG
#else
	if(!answers::confirm(last_quest->summary))
		return;
	answers::message(last_quest->agree);
#endif
	last_quest->enter();
}

static void enter_inn() {
}

static actioni actions[] = {
	{"Quest", enter_quest},
	{"Inn", enter_inn},
};

static void play_actions(const char* header, aref<actioni> actions) {
	answers aw;
	while(draw::isallowmodal()) {
		aw.clear();
		for(auto& e : actions)
			aw.add((int)e.proc, e.name);
		auto p = (fnevent)aw.choosebg(header);
		if(p)
			p();
	}
}

void companyi::playcity() {
	play_actions(game.city, actions);
}