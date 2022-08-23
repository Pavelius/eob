#include "main.h"

void pray_for_spells();
void memorize_spells();
void scrible_scrolls();
void game_options();

static adventurei* last_quest;

static void choose_quest() {
	answers aw;
	for(auto& e : bsdata<adventurei>())
		aw.add((int)&e, e.getname());
	last_quest = (adventurei*)aw.choosemb("Which way to go?");
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

static void rent_inn() {
	if(!draw::dlgask("Do you really want to rent inn for 10 gold pieces?"))
		return;
	static actioni actions[] = {
		{"Pray for spells", pray_for_spells},
		{"Memorize spells", memorize_spells},
		{"Scrible scrolls", scrible_scrolls},
		{"Game options", game_options},
	};
	draw::options("Inn options", actions);
}

void city_options() {
	static actioni actions[] = {
		{"Enter quest", enter_quest},
		{"Rent inn", rent_inn},
		{"Game options", game_options},
	};
	draw::options("Camp options", actions);
}