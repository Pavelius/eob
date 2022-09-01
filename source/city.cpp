#include "main.h"

campaigni campaign;
static adventurei* last_quest;

void cityi::clear() {
	memset(this, 0, sizeof(*this));
}

static int get_time_left() {
	auto p = eventi::findtimer();
	if(!p)
		return -1;
	return p->param2 - game.getrounds();
}

bool cityi::askmiracle() {
	auto r = d100();
	auto n = getcity(Blessing);
	if(r >= n)
		return false;
	addcity(Blessing, -1);
	return true;
}

void cityi::addcity(const cityi& e) {
	for(auto i = (city_ability_s)0; i <= Gold; i = (city_ability_s)(i + 1)) {
		if(i == ExperienceReward)
			game.addexpc(e.data[i], 0);
		else
			data[i] += e.data[i];
	}
}

static void choose_quest() {
	answers aw;
	for(auto& e : bsdata<adventurei>()) {
		if(e.stage == 1 || e.stage == 2)
			aw.add((int)&e, e.getname());
	}
	last_quest = (adventurei*)aw.choosemb("Which way to go?");
}

static void enter_quest() {
	choose_quest();
	if(!last_quest)
		return;
	if(last_quest->stage == 1) {
		if(!answers::confirm(last_quest->summary))
			return;
		answers::message(last_quest->agree);
		last_quest->stage = 2;
	}
	last_quest->enter();
}

static int getdiscounted(int cost) {
	auto count = (game.getcity(Reputation) - 50) / 5;
	cost -= count;
	if(cost < 1)
		cost = 1;
	return cost;
}

static bool pay(int cost) {
	if(game.getcity(Gold) < cost) {
		draw::dlgmsgsm("You don't have enought gold piece!");
		return false;
	}
	game.addcity(Gold, -cost);
	return true;
}

static void rent_inn() {
	auto cost = getdiscounted(30);
	char temp[260]; stringbuilder sb(temp);
	sb.add("Do you really want to rent inn for %1i gold pieces?", cost);
	if(!draw::dlgask(temp))
		return;
	if(!pay(cost))
		return;
	enter_inn();
}

static void play_dialog() {
	answers an;
	for(auto& e : last_menu) {
		if(e.test && !e.test())
			continue;
		an.add((int)&e, e.name);
	}
	auto pa = (actioni*)an.choosebg(last_menu_header);
	if(pa) {
		if(pa->test && !pa->test())
			return;
		draw::setnext(pa->proc);
	}
}

static void make_donation() {
	auto cost = getdiscounted(500);
	char temp[260]; stringbuilder sb(temp);
	sb.add("Do you really want donate %1i gold pieces?", cost);
	if(!draw::dlgask(temp))
		return;
	if(pay(cost)) {
		game.addcity(Blessing, 1);
		if(game.askmiracle())
			add_small_miracle();
	}
	draw::setnext(play_city);
}

static void enter_temple() {
	static actioni actions[] = {
		{"Donate", make_donation},
		{"Leave temple", enter_city},
	};
	last_image.res = BUILDNGS;
	last_image.frame = campaign.temple_frame;
	last_name = campaign.temple;
	last_menu = actions;
	last_menu_header = "Temple options";
	draw::setnext(play_city);
}

static void eat_and_drink() {
	if(game.getcity(Gold) >= 2)
		game.addcity(Gold, -2);
	game.passtime(xrand(30, 60));
	answers::message(campaign.feast);
	for(auto p : party)
		p->satisfy();
	draw::setnext(play_city);
}

static void enter_tavern() {
	static actioni actions[] = {
		{"Eat and drink", eat_and_drink},
		{"Leave tavern", enter_city},
	};
	last_image.res = BUILDNGS;
	last_image.frame = campaign.tavern_frame;
	last_name = campaign.tavern;
	last_menu = actions;
	last_menu_header = "Tavern options";
	draw::setnext(play_city);
}

void enter_city() {
	static actioni actions[] = {
		{"Go on adventure", enter_quest},
		{"Rent inn", rent_inn},
		{"Visit temple", enter_temple},
		{"Visit tavern", enter_tavern},
		{"Game options", game_options},
	};
	if(location) {
		if(campaign.city)
			mslog("Party return to %1", campaign.city);
	}
	location.clear();
	location_above.clear();
	last_image.res = BUILDNGS;
	last_image.frame = campaign.city_frame;
	last_name = campaign.city;
	last_menu = actions;
	last_menu_header = "City options";
	draw::setnext(play_city);
}

static void addval(stringbuilder& sb, const char* pb, int value, const char* name) {
	if(!value)
		return;
	if(pb[0])
		sb.add(", ");
	sb.add("%1i %2", value, name);
}

static void gain_loot() {
	for(auto p : party) {
		if(p)
			p->removeloot();
	}
}

static void gain_reward() {
	last_adventure->stage = 0xFF;
	answers::message(last_adventure->finish);
	game.addcity(Reputation, 1);
	for(auto i = 0; i < 128; i++) {
		if(last_adventure->unlock.is(i)) {
			if(bsdata<adventurei>::elements[i].stage == 0)
				bsdata<adventurei>::elements[i].stage = 1;
		}
	}
}

void return_to_city() {
	gain_loot();
	if(last_adventure) {
		if(last_adventure->iscomplete())
			gain_reward();
		else
			game.addcity(Reputation, -1);
	}
	game.write();
	draw::setnext(enter_city);
}

static void rest_party() {
	game.camp(RationIron, false, 4);
	draw::setnext(enter_city);
}

void enter_inn() {
	static actioni actions[] = {
		{"Pray for spells", pray_for_spells},
		{"Memorize spells", memorize_spells},
		{"Scrible scrolls", scrible_scrolls},
		{"Rest party", rest_party},
		{"Leave inn", enter_city},
	};
	last_image.res = BUILDNGS;
	last_image.frame = campaign.inn_frame;
	last_name = campaign.inn;
	last_menu = actions;
	last_menu_header = "Inn options";
	draw::setnext(play_city);
}

void item::sell() {
	if(!type)
		return;
	char temp[512]; stringbuilder sb(temp);
	auto cost = getdiscounted(getcostgp() / 2);
	if(cost <= 3)
		cost = 0;
	if(!cost)
		sb.add("Do you really want to drop this item away?");
	else
		sb.add("Do you really want to sell item for %1i gold pieces?", cost);
	if(!draw::dlgask(temp))
		return;
	game.addcity(Gold, cost);
	clear();
}