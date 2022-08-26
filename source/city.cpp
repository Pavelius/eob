#include "main.h"

companyi campaign;
static adventurei* last_quest;
static int last_value;

bool cityi::askmiracle() {
	auto r = d100();
	auto n = getcity(Blessing);
	if(r >= n)
		return false;
	addcity(Blessing, -1);
	return true;
}

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

static int getdiscounted(int cost) {
	auto count = (game.get(Reputation) - 50) / 5;
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
		if(e.test && !e.test(e.param))
			continue;
		an.add((int)&e, e.name);
	}
	auto pa = (actioni*)an.choosebg(last_menu_header);
	if(pa) {
		if(pa->test && !pa->test(pa->param))
			return;
		last_value = pa->param;
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
		{"Game options", game_options},
	};
	last_image.res = BUILDNGS;
	last_image.frame = campaign.temple_frame;
	last_name = campaign.temple;
	last_menu = actions;
	last_menu_header = "Temple options";
	draw::setnext(play_city);
}

void enter_city() {
	static actioni actions[] = {
		{"Enter quest", enter_quest},
		{"Rent inn", rent_inn},
		{"Visit temple", enter_temple},
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
	last_loot.clear();
	for(auto p : party) {
		if(p)
			p->removeloot();
	}
	if(!last_loot)
		return;
	char temp[512]; stringbuilder sb(temp);
	sb.addn("/BUILDNGS 18");
	sb.addn("After return to settlement you visit a shop and sell all items, which you get in adventure. After all you gain: ");
	auto pb = sb.get();
	addval(sb, pb, last_loot.gold, "gold pieces");
	addval(sb, pb, last_loot.experience, "epxerience points");
	addval(sb, pb, last_loot.reputation, "reputation");
	addval(sb, pb, last_loot.blessing, "god blessing");
	answers::message(temp);
	game.addcity(Gold, last_loot.gold);
	game.addcity(Reputation, last_loot.reputation);
	game.addcity(Blessing, last_loot.blessing);
	game.addexpc(last_loot.experience, 0);
}

void gamei::returntobase() {
	gain_loot();
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
		{"Game options", game_options},
	};
	last_image.res = BUILDNGS;
	last_image.frame = campaign.inn_frame;
	last_name = campaign.inn;
	last_menu = actions;
	last_menu_header = "Inn options";
	draw::setnext(play_city);
}