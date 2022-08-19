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
	if(!answers::confirm(last_quest->summary))
		return;
	answers::message(last_quest->agree);
	last_quest->enter();
}

void gamei::playcity() {
	answers aw;
	while(draw::isallowmodal()) {
		aw.clear();
		aw.add((int)enter_quest, "Quest");
		auto p = (fnevent)aw.choosebg("#BUILDNGS 17\nYou are on the street of city Waterdeep. Spend some money and rest for a moment.");
		if(p)
			p();
	}
}