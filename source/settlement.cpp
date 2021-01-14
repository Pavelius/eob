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
	"black hair woman", "black hair lady", "traveller in a coach",
	"halfnaked woman",
};
static const char* local_rumor[] = {
	"Locals tells interesting tales.",
	"I heard some local news.",
	"I have one interesting fact about this settlement."
};
static const char* distant_rumor[] = {
	"I heard rumor from %1.",
	"Bad news from %1.",
	"Some interesting news about %1.",
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
	for(auto i = action_s(0); i <= Work; i = (action_s)(i + 1)) {
		if(!ei.is(i))
			continue;
		if(!apply(id, i, false))
			continue;
		aw.add(i, bsdata<actioni>::elements[i].name);
	}
	return (action_s)aw.choosebg(ei.description);
}

void settlementi::getdescriptor(stringbuilder& sb) const {
	auto n = getlevel(prosperty);
	if(kind_n1[n])
		sb.adds(kind_n1[n]);
	if(is(Harbor))
		sb.adds("coasted");
	sb.adds(kind_n2[n]);
}

variant settlementi::enter() {
	sb.clear();
	auto n = getlevel(prosperty);
	if(game.isnight()) {
		static imagei im = {BUILDNGS, 13};
		im.add(sb);
	} else if(image)
		image.add(sb);
	else {
		imagei im = {BUILDNGS, (unsigned short)(15 + imin((int)getrarity(), 2))};
		im.add(sb);
	}
	sb.add("You reach a");
	getdescriptor(sb);
	sb.adds("named %1.", getname());
	if(game.isnight())
		sb.adds("There is night on street. Most buildings here were closed.");
	answers aw;
	if(apply(Tavern, Quest, false))
		aw.add((int)variant(Quest), "Quest");
	aw.add((int)variant(Travel), "Travel");
	if(game.isnight())
		aw.add((int)variant(Rest), "Rest");
	for(auto i = Arena; i <= WizardTower; i = (building_s)(i + 1)) {
		if(!is(i))
			continue;
		if(game.isnight() && !bsdata<buildingi>::elements[i].actions.is(Rest))
			continue;
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

static bool buy_items(building_s b, rarity_s rarity, bool run) {
	auto& ei = bsdata<buildingi>::elements[b];
	adat<item> genitems;
	itema items;
	auto ismagicitems = ei.goods.is(Devices) || ei.goods.is(Potions);
	if(ismagicitems)
		create(genitems, ei.goods, rarity);
	else
		create(genitems, ei.goods);
	items.select(genitems);
	items.costgp(true);
	if(!ismagicitems)
		items.match(rarity, true);
	items.maxcost(game.getgold(), true);
	if(!items)
		return false;
	if(run) {
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
		party.additem(*p, false);
	}
	return true;
}

static bool sell_items(building_s b, bool run) {
	auto& ei = bsdata<buildingi>::elements[b];
	itema items;
	items.select();
	items.costgp(true);
	items.match(ei.goods, true);
	if(!items)
		return false;
	if(run) {
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
	}
	return true;
}

static void showmessage() {
	answers aw;
	aw.add(1, "Continue");
	aw.choosebg(sb);
	sb.clear();
}

static bool confirm_pay() {
	if(!confirm(sb))
		return false;
	return true;
}

static bool gamble(settlementi& e, bool run) {
	static imagei ei = {BUILDNGS, 1};
	static int bits[] = {10, 20, 50, 100, 300, 500, 1000, 2000};
	creaturea creatures;
	creatures.select();
	creatures.match(Chaotic, true);
	if(!creatures)
		return false;
	if(run) {
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
		game.passtime(xrand(60, 120));
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
	return true;
}

static bool resting(const char* format, int cost, bool fullrest, bool healing) {
	auto healed = 1;
	sb.clear();
	sb.add(format, cost);
	if(!confirm_pay())
		return false;
	if(fullrest) {
		healed += 4;
		for(auto p : party)
			p->autocast(party);
	}
	if(healing) {
		for(auto p : party)
			p->resting(healed);
	}
	game.passtime(8 * 60);
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

static bool journey(settlementi& e, building_s b, bool run) {
	variantc locations;
	locations.clear();
	locations.select(Settlement);
	locations.exclude(&e);
	locations.match(e.position, getmaximumdistance(b), true);
	if(!locations)
		return false;
	if(run) {
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
	}
	return true;
}

static bool explore(settlementi& e, bool run) {
	static imagei im = {BUILDNGS, 18};
	variantc locations;
	locations.clear();
	locations.select(Adventure);
	locations.match(&e, true);
	locations.matchac(true);
	if(!locations)
		return false;
	if(run) {
		sb.clear();
		sb.adds("Wich location in this settlement or nearbe you want to visit?");
		answers aw;
		aw.add(0, "Cancel");
		for(auto v : locations)
			aw.add((int)v, v.getname());
		variant r = aw.choosebg(sb);
		if(!r)
			return false;
		auto pa = r.getadventure();
		if(!pa)
			return false;
		if(pa->message_before) {
			answers aw;
			aw.add(1, "Accept");
			aw.add(0, "Decline");
			auto i = aw.choosebg(pa->message_before);
			if(!i)
				return false;
		}
		auto cost = e.getequipmentcost(*pa);
		sb.clear();
		im.add(sb);
		sb.adds("You must equip party before journey. Equiping include buy some useful items, like torches, provision, ropes and other adventure equipment. Totaly you must spend %1i gold coins. Do you really want to spent money and go to adventure?", cost);
		if(!confirm(sb))
			return false;
		game.pay(cost);
		game.equiping();
		if(pa->message_agree)
			answers::message(pa->message_agree);
		game.rideto(r);
	}
	return true;
}

static const char* talk_settlement() {
	return 0;
}

static const char* talk_boring() {
	return maprnd(answer_boring);
}

static const char* talk_rumor(building_s b) {
	switch(b) {
	case Inn:
		return maprnd(answer_inn);
	case Tavern:
		if(d100() < 30)
			return maprnd(answer_inn);
		return maprnd(answer_tavern);
	default:
		return talk_boring();
	}
}

static const char* random_opponent() {
	return maprnd(talk_opponent);
}

adventurei* allowed_rumor() {
	variantc var;
	var.select(Adventure);
	var.matchrm(true);
	if(!var)
		return 0;
	return var.random().getadventure();
}

bool talk(const char* prompt, const char* text, char& mood) {
	sb.clear();
	static imagei im = {BUILDNGS, 20};
	auto po = random_opponent();
	auto chance_heard_true = 25 + mood * 5;
	adventurei* rumor_quest = 0;
	if(mood <= -4) {
		sb.add("There is no one who want to talk with you. Try talk another day.");
		showmessage();
		return false;
	} else if(mood <= 0)
		text = talk_boring();
	else if(d100() < chance_heard_true)
		rumor_quest = allowed_rumor();
	if(!game.roll(party.getaverage(Charisma)))
		mood--;
	im.add(sb);
	sb.add(prompt, po);
	sb.adds("\"");
	if(rumor_quest) {
		sb.add(maprnd(local_rumor));
		sb.adds(rumor_quest->rumor_activate.getname());
		rumor_quest->activate();
	} else
		sb.add(text);
	sb.add("\"");
	showmessage();
	return true;
}

static bool drink_and_seat(building_s b, int coins, char& informations, bool run) {
	static imagei ei = {BUILDNGS, 6};
	if(game.getgold() < coins)
		return false;
	if(run) {
		sb.clear();
		ei.add(sb);
		sb.add("You pay %1i for drinking and food. Then seat to feast.", coins);
		showmessage();
		game.passtime(xrand(30, 60));
		party.satisfy();
		return talk("A %+1 sit to drink with you and say", talk_rumor(b), informations);
	}
	return true;
}

static bool sacrifice(bool run) {
	itema items;
	items.select();
	items.match({HolySymbol, HolySymbolEvil}, true);
	if(!items)
		return false;
	if(run) {
		sb.clear();
		sb.add("You have some holy symbols. If you sacrifice it to holy god, you got experience and holiness. Do you really want sacrifice all symbols?");
		if(!confirm(sb))
			return false;
		auto exp = 0;
		auto sacrifice_score = 0;
		for(auto p : items) {
			p->clear();
			exp += 100;
			sacrifice_score++;
		}
		game.addsacrifice(sacrifice_score);
		game.addexpc(exp, 0);
	}
	return true;
}

static bool healing(int cost, bool run) {
	if(game.getgold() < cost)
		return false;
	if(!party.have(Wounded))
		return false;
	if(run) {
		if(cost > 0) {
			sb.clear();
			sb.add("\"Healing all of you will be cost %1i gold coins. Do you agree?\"", cost);
			if(!confirm(sb))
				return false;
		}
		for(auto p : party)
			p->heal(true);
		sb.clear();
		sb.add("\"With god blessing all of you has been healed. Pray our god and you will be rewarded!\"");
		showmessage();
	}
	return true;
}

static bool donate(bool run) {
	static int level[][2] = {{50, 3}, {100, 7}, {200, 16}, {300, 30}, {400, 50}};
	if(game.getgold() < level[0][0])
		return false;
	if(run) {
		sb.clear();
		sb.add("\"For glory of mighty god you must donate gold coins. How much it will be?\"");
		answers aw;
		aw.add(-1, "Nothing");
		for(unsigned i = 0; i < sizeof(level) / sizeof(level[0]); i++) {
			if(game.getgold()>=level[i][0])
				aw.add(i, "%1i", level[i][0]);
		}
		auto i = aw.choosebg(sb);
		if(i == -1)
			return false;
		game.pay(level[i][0]);
		game.adddonation(level[i][0]);
		auto chance_increase_luck = level[i][1];
		sb.clear();
		if(d100() < chance_increase_luck) {
			static const char* t[] = {"The god has been pleased!", "Mighty god smiles and accept your donation!", "Oh, holy god! I feel a the power! Your donation is accept!"};
			sb.add("\"%1\"", maprnd(t));
			game.addluck();
		} else {
			static const char* t[] = {"Your donation is not enought. You must donate more.", "Donation is made. God is watching you."};
			sb.add("\"%1\"", maprnd(t));
		}
		showmessage();
	}
	return true;
}

static bool havefun(int cost, bool run) {
	if(game.getgold() < cost)
		return false;
	if(run) {
		sb.clear();
		sb.adds("Fun cost money. You must pay for all pleasures %1i gold coins. Do you pay?", cost);
		if(!confirm_pay())
			return false;
		sb.clear();
		for(auto p : party) {
			auto cha_ok = p->roll(Charisma);
			auto con_ok = p->roll(Constitution);
			if(cha_ok && con_ok) {
				sb.adds("%1 have full pleasure and get some new experience.", p->getname());
				p->addexp(50);
				if(p->is(Chaotic))
					p->addexp(25);
			} else
				sb.adds("%1 will be very shy.", p->getname());
		}
		game.passtime(xrand(3*60, 5*60));
		showmessage();
	}
	return true;
}

bool settlementi::apply(building_s b, action_s a, bool run) {
	if(game.isnight()) {
		switch(a) {
		case Leave: break;
		case Travel: return journey(*this, b, run);
		case Quest: return explore(*this, run);
		case Rest:
			if(run)
				return resting("You may stay for night in this place, but not for free. It will cost you %1i gold coins. Do you pay?",
					getrestcost(b), false, false);
			break;
		default: return false;
		}
	} else {
		switch(a) {
		case Buy: return buy_items(b, getrarity(), run);
		case Sell: return sell_items(b, run);
		case Drink: return drink_and_seat(b, getdrinkcost(), mood_tavern, run);
		case Talk:
			if(run)
				return talk("You find a %+1, who sad", talk_rumor(b), mood_inn);
			break;
		case Fun: return havefun(20, run);
		case Travel: return journey(*this, b, run);
		case Quest: return explore(*this, run);
		case Gambling: return gamble(*this, run);
		case Leave: break;
		case Rest:
			if(run)
				return resting("You may stay for night in this place, but not for free. It will cost you %1i gold coins. Do you pay?",
					getrestcost(b), false, true);
			break;
		case Sacrifice: return sacrifice(run);
		case HealAction: return healing(gethealingcost(), run);;
		case Donate: return donate(run);;
		default: return false;
		}
	}
	return true;
}

int	settlementi::gethealingcost() const {
	auto r = 60;
	if(party.have(Paladin))
		r -= 15;
	if(party.have(Cleric))
		r -= 15;
	return r;
}

int	settlementi::getequipmentcost(adventurei& e) const {
	return 2 + position.range(e.position) / game.pixels_per_day;
}

static void correct_talk(char& v, int maximum) {
	if(v < maximum)
		v++;
}

void settlementi::update() {
	auto chat_rang = 2;
	correct_talk(mood_tavern, chat_rang);
	correct_talk(mood_inn, chat_rang);
	correct_talk(mood_other, chat_rang);
}

void settlementi::play() {
	auto v = enter();
	switch(v.type) {
	case Action:
		if(v.value==Rest)
			resting("You can rest right here. But you can't sleep comfortable, so you can't restore spells and have a healing sleep.",
				0, false, false);
		else
			apply(Tavern, (action_s)v.value, true);
		break;
	case Building:
		while(game.getsettlement() == this) {
			auto b = (building_s)v.value;
			auto a = enter(b);
			if(a == Leave) {
				draw::setnext(game.play);
				break;
			}
			apply(b, a, true);
			game.passtime(60);
		}
		break;
	}
}

int	settlementi::getrestcost(building_s b) const {
	auto cost = party.getcount();
	return cost;
}

void settlementi::addprosperty(int v) {
	prosperty += v;
	if(prosperty < 0)
		prosperty = 0;
}