#include "main.h"

static const char* answer_inn[] = {
	"If you want stay at night and have comfortable rest - you must go to the inn. Of course, in tavern or on the street you also may rest, but ony in inn you rest will be most effective.",
	"If you want travel you may do it by foot, just moving from one settlement to another. But this option consume time and sometimes endanger you. If luck of time, you may use stable and move with caravans faster from one place to another. Also, in coasted city, you may use harbor, to cross very long distance by short time. What can be easier?",
	"When you are boring - move to tavern. In that place you always find something funny. Try get some drink and adnventure start by itsef.",
};
static const char* answer_tavern[] = {
	"Try to play in cards or dices with those locals. If luck will be granted to you, you may raise coins. The richer taver, the greater gain would be.",
	"Beware of tough guys. They can kill you at the night.",
	"Talking with people in place like this, or maybe inn can be useful. Some people give you important piece of information or goo life hack.",
};
static const char* answer_boring[] = {
	"Not you again. Goodbuy.",
	"Excuse me, I am busy.",
	"It's all that I know. Try visit another day.",
	"Stop talking. Enought.",
};
static const char* talk_opponent[] = {
	"old dwarf", "middle aged human", "young man",
	"beautiful woman", "scarred traveller", "welldressed woman",
	"old men with a staff", "strong guy", "rotten teathed man",
	"black hair woman", "traveller in a coach"
};

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

action_s settlementi::enter(building_s id) {
	auto& ei = bsdata<buildingi>::elements[id];
	answers aw;
	aw.add(Leave, bsdata<actioni>::elements[Leave].name);
	for(auto i = action_s(0); i <= Pet; i = (action_s)(i + 1)) {
		if(!ei.is(i))
			continue;
		if(!apply(id, i, false))
			continue;
		aw.add(i, bsdata<actioni>::elements[i].name);
	}
	return (action_s)aw.choosebg(ei.description);
}

variant settlementi::enter() {
	sb.clear();
	auto n = getlevel(prosperty);
	if(image)
		image.add(sb);
	else {
		imagei im = {BUILDNGS, (unsigned short)(15 + imin((int)getrarity(), 2))};
		im.add(sb);
	}
	sb.add("You reach a");
	if(kind_n1[n])
		sb.adds(kind_n1[n]);
	if(is(Harbor))
		sb.adds("coasted");
	sb.adds(kind_n2[n]);
	sb.adds("named %1.", getname());
	answers aw;
	if(apply(Tavern, Explore, false))
		aw.add((int)variant(Explore), "Explore");
	aw.add((int)variant(Travel), "Travel");
	for(auto i = Arena; i <= WizardTower; i = (building_s)(i + 1)) {
		if(is(i))
			aw.add((int)variant(i), bsdata<buildingi>::elements[i].name);
	}
	return aw.choosebg(sb);
}

static bool isallow(item_s v, const goodf& goods) {
	auto& ei = bsdata<itemi>::elements[v];
	return goods.is(ei.goods);
}

static void create(adat<item>& pi, const goodf& goods) {
	for(auto i = item_s(1); i <= LastItem; i = (item_s)(i + 1)) {
		if(!isallow(i, goods))
			continue;
		if(bsdata<itemi>::elements[i].enchantments
			&& bsdata<itemi>::elements[i].enchantments[0].power)
			continue;
		auto p = pi.add();
		*p = i;
		p->finish();
	}
}

static void create(adat<item>& pi, const goodf& goods, rarity_s rarity) {
	for(auto i = item_s(1); i <= LastItem; i = (item_s)(i + 1)) {
		if(!isallow(i, goods))
			continue;
		auto& ei = bsdata<itemi>::elements[i].enchantments;
		for(auto& e : ei) {
			if(e.rarity > rarity)
				continue;
			auto p = pi.add(); *p = i;
			p->setenchant(ei.indexof(&e));
			p->finish();
			p->setidentified(1);
		}
	}
}

static void create(itema& result, adat<item>& source) {
	for(auto& e : source) {
		auto p = result.add();
		*p = &e;
	}
}

static bool confirm(const char* text) {
	answers aw;
	aw.add(1, "Yes");
	aw.add(0, "No");
	return aw.choosebg(text) != 0;
}

static const char* buy_panel(const void* object, stringbuilder& sb) {
	auto p = (item*)object;
	sb.addn("Item price: %1i gp", p->getcostgp());
	sb.addn("Party have: %1i gp", game.getgold());
	return sb;
}

static bool buy_items(itema& items) {
	items.sort();
	auto p = items.choose("Which item to buy?", true, buy_panel);
	if(!p)
		return false;
	auto cost = p->getcostgp();
	sb.clear();
	sb.add("Do you really want to buy ");
	p->getname(sb);
	sb.adds("for %1i gold coins?", cost);
	if(!confirm(sb))
		return false;
	game.addgold(-cost);
	game.additem(*p, false);
	return true;
}

static bool sell_items(itema& items) {
	items.sort();
	auto p = items.choose("Which item to sell?", true, buy_panel);
	if(!p)
		return false;
	auto cost = p->getcostgp();
	sb.clear();
	sb.add("Do you really want to sell ");
	p->getname(sb);
	sb.adds("for %1i gold coins as trade in?", cost);
	if(!confirm(sb))
		return false;
	game.addgold(-cost);
	p->clear();
	return true;
}

static void showmessage() {
	answers aw;
	aw.add(1, "Continue");
	aw.choosebg(sb);
	sb.clear();
}

static bool confirm_pay() {
	if(!confirm(sb)) {
		sb.clear();
		sb.add("\"Very well. So get out of here, and not waste my time!\"");
		showmessage();
		return false;
	}
	return true;
}

static void gambling(creaturea& creatures) {
	static imagei ei = {BUILDNGS, 1};
	static int bits[] = {10, 20, 50, 100, 300, 500, 1000, 2000};
	sb.clear();
	ei.add(sb);
	sb.adds("Do you want to play cards, dice or thimblerig? If you do, take your bid and try to win.");
	sb.adds("How match you bet?");
	answers aw;
	aw.add(0, "Stop");
	for(auto b : bits) {
		if(game.getgold() >= b)
			aw.add(b, "%1i", b);
	}
	auto b = aw.choosebg(sb);
	auto p = creatures.getbest(Charisma);
	auto gv = p->get(Charisma);
	sb.clear();
	ei.add(sb);
	if(game.roll(gv)) {
		p->addexp(50);
		sb.add("%1 win game and gain %2i coins.", p->getname(), b);
		game.addgold(b);
		showmessage();
	} else {
		sb.add("%1 lose game and lose %2i coins. Luck is not on you side today.", p->getname(), b);
		game.addgold(-b);
		showmessage();
	}
}

static bool resting(building_s b, creaturea& creatures) {
	auto cost = party.getcount();
	auto healed = 1;
	sb.clear();
	if(b == Tavern) {
		sb.add("Bartender looked for you and sad: \"Get food and drink for this gentlemens! And this will be cost %1i gold coins. Do you pay?\"", cost);
		if(!confirm_pay())
			return false;
	} else if(b == Inn) {
		healed += 4;
		sb.add("Inn's owner looked for you and sad: \"You may stay. It will be cost %1i gold coins. Do you pay?\"", cost);
		if(!confirm_pay())
			return false;
		for(auto p : creatures)
			p->autocast(creatures);
	}
	for(auto p : creatures)
		p->resting(healed);
	game.passtime(8 * 60);
	return true;
}

static variant current_action;

static bool journey(variantc& locations) {
	sb.clear();
	sb.adds("You can make journay to another settlement. Choose settlement you want to travel.");
	answers aw;
	aw.add(0, "Cancel");
	locations.sort();
	for(auto v : locations)
		aw.add((int)v, v.getname());
	variant r = aw.choosebg(sb);
	if(!r)
		return false;
	game.rideto(r);
	return true;
}

static bool explore(variantc& locations) {
	static imagei shop_image = {BUILDNGS, 18};
	sb.clear();
	sb.adds("Wich location in this settlement or nearbe you want to explore?");
	answers aw;
	for(auto v : locations)
		aw.add((int)v, v.getname());
	aw.add(0, "Cancel");
	variant r = aw.choosebg(sb);
	if(!r)
		return false;
	auto cost = 5;
	sb.clear();
	shop_image.add(sb);
	sb.adds("You must equip party before journey. Equiping include buy some useful items, like torches, provision, ropes and other adventure equipment. Totaly you must spend %1i gold coins. Do you really want to spent money and go to adventure?", cost);
	if(!confirm(sb))
		return false;
	game.pay(cost);
	game.equiping();
	game.rideto(r);
	draw::setnext(draw::adventure);
	return true;
}

static int getmaximumdistance(building_s b) {
	switch(b) {
	case Stable:
		return imax(game.getmapheight(), game.getmapwidth()) / 3;
	case Harbor:
		return imax(game.getmapheight(), game.getmapwidth());
	default:
		return game.pixels_per_day * 3;
	}
}

static const char* talk_boring(building_s b) {
	return maprnd(answer_boring);
}

static const char* talk_rumor(building_s b) {
	switch(b) {
	case Inn:
		return maprnd(answer_inn);
	case Tavern:
		if(d100()<30)
			return maprnd(answer_inn);
		return maprnd(answer_tavern);
	default:
		return talk_boring(b);
	}
}

static const char* random_opponent() {
	return maprnd(talk_opponent);
}

bool talk(building_s b, creaturea& party, char& informations) {
	sb.clear();
	static imagei im = {BUILDNGS, 20};
	const char* ps = 0;
	auto po = random_opponent();
	if(informations <= -4) {
		sb.add("There is no one who want to talk with you. Try talk another day.");
		showmessage();
		return false;
	} else if(informations <= 0)
		ps = talk_boring(b);
	else
		ps = talk_rumor(b);
	if(!game.roll(party.getaverage(Charisma)))
		informations--;
	im.add(sb);
	if(b==Tavern)
		sb.add("An %+1 drink with you and sad", po);
	else
		sb.add("You find an %+1, who sad", po);
	sb.add(": \"");
	sb.add(ps);
	sb.add("\"");
	showmessage();
	return true;
}

static bool drink_and_seat(building_s b, action_s a, int coins, creaturea& party, char& informations) {
	static imagei ei = {BUILDNGS, 6};
	sb.clear();
	ei.add(sb);
	sb.add("\"Good, day!\" - bartender sad - \"Cost for drinking would be %1i gold coins. Do you want pay?\"", coins);
	if(!confirm_pay())
		false;
	return talk(b, party, informations);
}

bool settlementi::apply(building_s b, action_s a, bool run) {
	auto& ei = bsdata<buildingi>::elements[b];
	adat<item> genitems;
	creaturea creatures;
	variantc vars;
	itema items;
	auto ismagicitems = ei.goods.is(Devices) || ei.goods.is(Potions);
	switch(a) {
	case Buy:
		if(ismagicitems)
			create(genitems, ei.goods, getrarity());
		else
			create(genitems, ei.goods);
		create(items, genitems);
		items.costgp(true);
		if(!ismagicitems)
			items.match(getrarity(), true);
		items.maxcost(game.getgold(), true);
		if(!items)
			return false;
		if(run)
			return buy_items(items);
		break;
	case Sell:
		items.select();
		items.costgp(true);
		items.match(ei.goods, true);
		if(!items)
			return false;
		if(run)
			return sell_items(items);
		break;
	case Drink:
		if(run)
			return drink_and_seat(b, a, 3, party, talk_tavern);
		break;
	case Talk:
		if(run)
			return talk(b, party, talk_inn);
		break;
	case Travel:
		vars.clear();
		vars.select(Settlement);
		vars.exclude(this);
		vars.match(position, getmaximumdistance(b), true);
		if(run)
			return journey(vars);
		break;
	case Explore:
		vars.clear();
		vars.select(Adventure);
		vars.match(this, true);
		if(!vars)
			return false;
		if(run)
			return explore(vars);
		break;
	case Gambling:
		creatures.select();
		creatures.match(Chaotic, true);
		if(!creatures)
			return false;
		if(run)
			gambling(creatures);
		break;
	case Leave:
		break;
	case Rest:
		if(run)
			return resting(b, party);
		break;
	default:
		return false;
	}
	return true;
}

void settlementi::adventure() {
	talk_inn = 3;
	talk_tavern = 3;
	auto v = enter();
	switch(v.type) {
	case Action:
		apply(Tavern, (action_s)v.value, true);
		break;
	case Building:
		while(game.getsettlement() == this) {
			auto b = (building_s)v.value;
			auto a = enter(b);
			if(a == Leave)
				break;
			apply(b, a, true);
			game.passtime(60);
		}
		break;
	}
}