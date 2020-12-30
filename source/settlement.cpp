#include "main.h"

static char				prompt_text[512];
static stringbuilder	sb(prompt_text);
static unsigned char	prosperty_progress[10] = {0, 5, 10, 15, 20, 30, 40, 55, 80, 100};
static rarity_s			rarity_items[10] = {Common, Common, Common, Uncommon, Uncommon, Uncommon, Rare, Rare, VeryRare, Artifact};
static const char*		kind_n1[10] = {"", "", "", "small", "", "small", "", "small", "large", ""};
static const char*		kind_n2[10] = {"community", "outpost", "hamlet", "village", "village", "town", "town", "city", "city", "megapolis"};

static int getlevel(int v) {
	auto r = 0;
	for(auto e : prosperty_progress) {
		if(v <= e)
			break;
		r++;
	}
	return r;
}

rarity_s settlementi::getrarity() const {
	return rarity_items[getlevel(prosperty)];
}

action_s settlementi::enter(building_s id) const {
	auto& ei = bsdata<buildingi>::elements[id];
	sb.clear();
	sb.add(ei.description);
	answers aw;
	for(auto i = action_s(0); i <= Pet; i = (action_s)(i + 1)) {
		if(ei.is(i))
			aw.add(i, bsdata<actioni>::elements[i].name);
	}
	aw.sort();
	aw.add(Leave, bsdata<actioni>::elements[Leave].name);
	return (action_s)aw.choosebg(prompt_text, "What you want to do?", &ei.image);
}

building_s settlementi::enter() const {
	sb.clear();
	auto n = getlevel(prosperty);
	sb.add("You reach a");
	if(kind_n1[n])
		sb.adds(kind_n1[n]);
	if(is(Harbor))
		sb.adds("coasted");
	sb.adds(kind_n2[n]);
	sb.adds("named %1.", getname());
	answers aw;
	for(auto i = Arena; i <= WizardTower; i = (building_s)(i + 1)) {
		if(is(i))
			aw.add(i, bsdata<buildingi>::elements[i].name);
	}
	aw.sort();
	return (building_s)aw.choosebg(prompt_text, "Witch way you want to go?", &image);
}

void settlementi::adventure() {
	while(true) {
		auto b = enter();
		auto a = enter(b);
		switch(a) {
		case Buy:
			break;
		case Sell:
			break;
		}
	}
}

static bool isallow(item_s v, const goodf& goods) {
	auto& ei = bsdata<itemi>::elements[v];
	return ei.goods.oneof(goods);
}

static void create(item* pi, const goodf& goods) {
	for(auto i = item_s(1); i <= LastItem; i = (item_s)(i + 1)) {
		if(!isallow(i, goods))
			continue;
		*pi = i;
		pi->finish();
		pi++;
	}
}

bool settlementi::apply(building_s b, action_s a, bool run) {
	auto& ei = bsdata<buildingi>::elements[b];
	item genitems[LastItem + 1];
	itema items;
	switch(a) {
	case Buy:
		create(genitems, ei.goods);
		break;
	case Sell:
		items.select();
		break;
	case Drink:
	case Talk:
		break;
	case Leave:
		break;
	default:
		return false;
	}
	return true;
}