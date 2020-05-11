#include "main.h"

static selli dirty_sell[3] = {BlueRing, BluePotion, RationIron};
static selli weapon_sell[3] = {{ArmorPlate, Uncommon}, {SwordLong, Rare}, BluePotion};

static messagei standart_dialog[] = {{Say, 1, {Friendly}, "\"Welcome friends!\"", {3}},
{Say, 1, {}, "\"Who is there? How you dig so deep? Are you from Master?\""},
{Ask, 1, {Charisma}, "Lie", {4, 3}},
{Ask, 1, {StartCombat}, "Attack"},
{Say, 3, {}, "\"Do you want something?\""},
{Ask, 3, {Trade, LeaveAway}, "Trade", {}, {}, dirty_sell},
{Ask, 3, {LeaveAway}, "Leave"},
{Ask, 3, {StartCombat}, "Attack"},
{Say, 4, {StartCombat}, "\"You lier! Prepare to die!\""},
{}};

void creature::encounter(reaction_s id) {
	auto ins = get(Intellegence);
	if(ins < 6)
		return;
	switch(id) {
	case Cautious:
	case Friendly:
		location.turnto(game.getcamera(), to(direction, Down));
		standart_dialog->choose(false, 1, id);
		break;
	}
}