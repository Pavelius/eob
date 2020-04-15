#include "main.h"

const unsigned		creature_players_base = 240;
extern creature		hero_data[];

variant::variant(const creature* p) {
	if(!p) {
		type = NoVariant;
		value = 0;
	} else if(p->ishero()) {
		type = Creature;
		value = creature_players_base + (p - hero_data);
	} else {
		type = Creature;
		value = p - location.monsters;
	}
}

creature* variant::getcreature() const {
	if(type != Creature)
		return 0;
	if(value >= creature_players_base)
		return hero_data + (value - creature_players_base);
	return location.monsters + value;
}