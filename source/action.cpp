#include "main.h"

BSDATA(actioni) = {{"Creeting"},
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
{"Trade"},
{"Travel"},
{"Pet"},
{"Experience"},
{"Gold"},
{"Prosperty"},
{"Reputation"},
{"Discard"},
{"Fail lie roll"},
{"Talk about artifact", true},
{"Talk about cursed item", true},
{"Talk about magic item", true},
{"Talk about magic loot on the ground", true},
{"Talk about magic loot in cellar", true},
{"Talk about history this place", true},
{"Rumor", true},
};
assert_enum(action, TalkRumor)
INSTELEM(actioni)

BSDATA(actionseti) = {{"Attack 1-3", Attack, 1, 3},
{"Attack 1-6", Attack, 1, 6},
{"Attack 2-8", Attack, 2, 4},
{"Attack 2-12", Attack, 2, 6},
{"Attack 3-18", Attack, 3, 6},
{"Gain 5 gold coins", Gold, 5},
{"Gain 10 gold coins", Gold, 10},
{"Gain 20 gold coins", Gold, 20},
{"Gain 50 experiences", Experience, 50},
{"Gain 100 experiences", Experience, 100},
{"Gain Prosperty", Prosperty, 1},
{"Gain Reputation", Reputation, 1},
{"Lose 5 gold coins", Gold, -5},
{"Lose 10 gold coins", Gold, -10},
{"Lose 20 gold coins", Gold, -20},
{"Lose 20 gold coins or reputation", Gold, -20},
{"Lose Prosperty", Prosperty, 1},
{"Loose Reputation", Reputation, 1},
{"Pay 1 gold coin", Pay, 1, 0, true},
{"Pay 2 gold coins", Pay, 2, 0, true},
{"Pay 5 gold coins", Pay, 5, 0, true},
{"Pay 10 gold coins", Pay, 10, 0, true},
};
assert_enum(actionset, Pay10)
INSTELEM(actionseti)

int	actionseti::roll() const {
	if(max)
		return xrand(count, max);
	return count;
}