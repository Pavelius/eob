#include "main.h"

BSDATA(actioni) = {
	{"Reshufle"},
	{"Attack"},
	{"Bribe"},
	{"Buy"},
	{"Donate"},
	{"Drink"},
	{"Fun"},
	{"Heal"},
	{"Gamble"},
	{"Leave"},
	{"Lie"},
	{"Quest"},
	{"Pay"},
	{"Repair"},
	{"Rest"},
	{"Sacrifice"},
	{"Sell"},
	{"Talk"},
	{"Take"},
	{"Trade"},
	{"Travel"},
	{"Pet"},
	{"Work"},
	{"Experience"},
};
assert_enum(action, Experience)
BSDATAF(actioni)

BSDATA(talki) = {
	{"Greeting"},
	{"Fail lie roll"},
	{"Talk about artifact", {Friendly}},
	{"Talk about cursed item", {Friendly}},
	{"Talk about magic item", {Friendly}},
	{"Talk about magic loot on the ground", {Friendly}},
	{"Talk about magic loot in cellar", {Friendly}},
	{"Talk about history this place", {Friendly}},
	{"Rumor", {Friendly}},
};
BSDATAF(talki)

BSDATA(actionseti) = {
	{"Attack 1-3", Attack, 1, 3},
	{"Attack 1-6", Attack, 1, 6},
	{"Attack 2-8", Attack, 2, 4},
	{"Attack 2-12", Attack, 2, 6},
	{"Attack 3-18", Attack, 3, 6},
	{"Exhause party", Work},
	{"Gain Rare Armor", Take, Armors, Rare},
	{"Gain Rare Weapon", Take, Weapons, Rare},
	{"Pay 1 gold coin", Pay, 1, 0, {CheckCondition}},
	{"Pay 2 gold coins", Pay, 2, 0, {CheckCondition}},
	{"Pay 5 gold coins", Pay, 5, 0, {CheckCondition}},
	{"Pay 10 gold coins", Pay, 10, 0, {CheckCondition}},
};
BSDATAF(actionseti)

int	actionseti::roll() const {
	if(count2)
		return xrand(count1, count2);
	return count1;
}

bool resultable::have(variant v) const {
	for(auto e : actions) {
		if(!e)
			break;
		if(e == v)
			return true;
	}
	return false;
}

bool resultable::isallow() const {
	for(auto e : actions) {
		if(!e)
			break;
		if(e.type == Case)
			continue;
		else if(e.type == ActionSet) {
			auto ps = (actionseti*)e.getpointer(ActionSet);
			if(ps->is(CheckCondition)) {
				auto v = game.get(ps->action);
				auto vm = ps->roll();
				if(v < vm)
					return false;
			}
		} else if(!party.have(e))
			return false;
	}
	return true;
}

void eventi::clear() {
	memset(this, 0, sizeof(*this));
}

void eventi::shufle() const {
}

void eventi::discard() const {
}

void eventi::apply(case_s v, bool interactive) const {
	for(auto& e : results) {
		if(!e)
			break;
		if(!e.have(v))
			continue;
		if(!e.isallow())
			continue;
		if(interactive)
			answers::message(e.getname());
		auto need_reshufle = false;
		for(auto a : e.actions) {
			if(a == variant(Reshufle))
				need_reshufle = true;
			else
				game.apply(a);
		}
		discard();
		if(need_reshufle)
			shufle();
		break;
	}
}

void eventi::play() const {
	answers aw;
	aw.add(Case1, ask[0]);
	aw.add(Case2, ask[1]);
	auto i = (case_s)aw.choosebg(*this);
	apply(i, true);
}

static item random(good_s good, rarity_s rarity) {
	adat<item> items;
	item::select(items, good, rarity);
	return items.random();
}

void gamei::apply(variant v) {
}