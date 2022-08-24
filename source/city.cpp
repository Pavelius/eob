#include "main.h"

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
	enter_inn();
}

void enter_city() {
	static actioni actions[] = {
		{"Enter quest", enter_quest},
		{"Rent inn", rent_inn},
		{"Game options", game_options},
	};
	last_image.res = BUILDNGS;
	last_image.frame = game.city_frame;
	last_name = game.city;
	last_menu = actions;
	last_menu_header = "City options";
	draw::setnext(play_city);
}

void enter_inn() {
	static actioni actions[] = {
		{"Pray for spells", pray_for_spells},
		{"Memorize spells", memorize_spells},
		{"Scrible scrolls", scrible_scrolls},
		{"Leave inn", enter_city},
		{"Game options", game_options},
	};
	last_image.res = BUILDNGS;
	last_image.frame = game.inn_frame;
	last_name = game.inn;
	last_menu = actions;
	last_menu_header = "Inn options";
	draw::setnext(play_city);
}