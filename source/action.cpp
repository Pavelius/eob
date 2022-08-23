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
assert_enum(actioni, Experience)
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

static item random(good_s good, rarity_s rarity) {
	adat<item> items;
	item::select(items, good, rarity);
	return items.random();
}

void gamei::apply(variant v) {
}