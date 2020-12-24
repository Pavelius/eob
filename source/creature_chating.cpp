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

void creature::interract() {
	auto new_reaction = reaction;
	auto party_index = game.getcamera();
	auto party_direction = game.getdirection();
	bool party_ambush = false;
	location.turnto(index, direction);
	location.formation(index, direction);
	location.turnto(party_index, to(direction, Down), &party_ambush);
	encounteri encounter;
	encounter.set(reaction);
	if(encounter.reaction == Indifferent)
		encounter.set(rollreaction(0));
	encounter.select(index);
	if(encounter.reaction == Indifferent || encounter.reaction == Friendly) {
		party_ambush = false;
		standart_dialog->choose(false, 1, encounter.reaction);
	}
	switch(encounter.reaction) {
	case Friendly:
		encounter.leave();
		game.addexp(Good, 50);
		break;
	case Indifferent:
		encounter.leave();
		break;
	default:
		game.attack(index, false, party_ambush ? PartyAmbush : NoAmbush);
		break;
	}
}