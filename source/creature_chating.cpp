#include "main.h"

static selli dirty_sell[] = {{BlueRing, 3}, {{PotionBlue, OfHealing, 0}, 2}, {RationIron, 1}, {}};
static selli weapon_sell[] = {{ArmorPlate, 3}, {{SwordLong}, 2}, {PotionBlue, 1}, {}};

static messagei cautions_dialog[] = {{Say, 1, {}, "\"Who is there? How you dig so deep? Are you from Master?\""},
{Ask, 1, {Charisma}, "Lie", {2, 3}},
{Ask, 1, {StartCombat}, "Attack"},
{Say, 2, {StartCombat}, "\"You lier! Prepare to die!\""},
{Say, 3, {}, "\"Master send you? Ok, what do you want?\""},
{Ask, 3, {Charisma}, "Talk"},
{Ask, 3, {Trade, LeaveAway}, "Trade", {}, {}, dirty_sell},
{Ask, 3, {StartCombat}, "Attack"},
{}};
static messagei friendly_dialog[] = {{Say, 1, {}, "\"Welcome friends! Do you want something?\""},
{Ask, 1, {Charisma}, "Talk"},
{Ask, 1, {Trade}, "Trade", {}, {}, dirty_sell},
{Ask, 1, {StartCombat}, "Attack"},
{}};

void creature::encounter(reaction_s id) {
	auto ins = get(Intellegence);
	if(ins < 6)
		return;
	switch(id) {
	case Cautious:
		location.turnto(game.getcamera(), to(direction, Down));
		cautions_dialog->choose(false);
		break;
	case Friendly:
		location.turnto(game.getcamera(), to(direction, Down));
		friendly_dialog->choose(false);
		break;
	}
}