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

action_s settlementi::enter(building_s id) {
	auto& ei = bsdata<buildingi>::elements[id];
	sb.clear();
	sb.add(ei.description);
	answers aw;
	for(auto i = action_s(0); i <= Pet; i = (action_s)(i + 1)) {
		if(!ei.is(i))
			continue;
		if(!apply(id, i, false))
			continue;
		aw.add(i, bsdata<actioni>::elements[i].name);
	}
	aw.sort();
	aw.add(Leave, bsdata<actioni>::elements[Leave].name);
	return (action_s)aw.choosebg(prompt_text, "What you want to do?", ei.image);
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
	return (building_s)aw.choosebg(prompt_text, "Witch way you want to go?", image);
}

void settlementi::adventure() {
	makeitems();
	while(true) {
		auto b = enter();
		auto a = enter(b);
		apply(b, a, true);
	}
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
	return aw.choosebg(text, 0) != 0;
}

static const char* buy_panel(void* object, stringbuilder& sb) {
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
	aw.add(1, "Next");
	aw.choosebg(sb, 0);
	sb.clear();
}

static void drink_and_seat(building_s b, action_s a, int coins) {
	sb.clear();
	sb.add("\"Good, day!\" - bartender sad - \"Cost for drinking would be %1i gold coins. Do you want pay?\"", coins);
	if(!confirm(sb)) {
		sb.clear();
		sb.add("\"Very well. So get out of here, and not waste my time!\"");
		showmessage();
	}
}

static void gambling(creaturea& creatures) {
	static int bits[] = {10, 20, 50, 100, 300, 500, 1000, 2000};
	sb.clear();
	sb.add("Do you want to play cards, dice or thimblerig? If you do, take your bid and try to win.");
	answers aw;
	aw.add(0, "Stop");
	for(auto b : bits) {
		if(game.getgold()>=b)
			aw.add(b, "%1i", b);
	}
	draw::imagestate push_img("gambling1");
	auto b = aw.choosebg(sb, "How match you bet?", 0, true); sb.clear();
	auto p = creatures.getbest(Charisma);
	auto gv = p->get(Charisma);
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

bool settlementi::apply(building_s b, action_s a, bool run) {
	auto& ei = bsdata<buildingi>::elements[b];
	adat<item> genitems;
	creaturea creatures;
	variantc vars;
	itema items;
	switch(a) {
	case Buy:
		create(genitems, ei.goods);
		create(items, genitems);
		if(ei.goods.is(Devices)) {
			for(auto& e : wands) {
				if(e)
					items.add(&e);
			}
		}
		items.costgp(true);
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
			drink_and_seat(b, a, 3);
		break;
	case Talk:
		break;
	case Travel:
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
	default:
		return false;
	}
	return true;
}

void settlementi::makeitems() {
	adat<rarity_s, 16> source;
	auto m = getrarity();
	for(auto i = Common; i <= m; i = (rarity_s)(i + 1))
		source.add(i);
	if(!source)
		return;
	for(auto& e : wands) {
		if(e)
			continue;
		e = item(MagicWand);
		auto i = source.data[rand() % source.getcount()];
		e.setpower(i);
		e.setidentified(1);
	}
}