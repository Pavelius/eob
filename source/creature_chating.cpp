#include "main.h"

static dialogi cautions_dialog[] = {{{}, "main", "\"Who is there? How you dig so deep? Are you from Master?\"", {{"Lie"}, {"Attack"}}},
{}};
static dialogi friendly_dialog[] = {{{}, "main", "\"Welcome friends! Do you want something?\"", {{"Talk"}, {"Trade"}, {"Attack"}, {"Leave"}}},
{}};

void creature::encounter(reaction_s id) {
	auto ins = get(Intellegence);
	switch(id) {
	case Cautious:
		location.turnto(game::getcamera(), to(direction, Down));
		cautions_dialog->choose(false);
		break;
	case Friendly:
		location.turnto(game::getcamera(), to(direction, Down));
		friendly_dialog->choose(false);
		break;
	}
}