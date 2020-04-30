#include "main.h"

static messagei cautions_dialog[] = {{Say, 1, {}, "\"Who is there? How you dig so deep? Are you from Master?\""},
{Ask, 1, {Charisma}, "Lie"},
{Ask, 1, {StartCombat}, "Attack"},
{}};
static messagei friendly_dialog[] = {{Say, 1, {}, "\"Welcome friends! Do you want something?\""},
{Ask, 1, {Charisma}, "Talk"},
{Ask, 1, {Trade}, "Trade"},
{Ask, 1, {StartCombat}, "Attack"},
{Ask, 1, {LeaveAway}, "Leave"},
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