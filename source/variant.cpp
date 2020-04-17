#include "main.h"

const unsigned		creature_players_base = 240;

variant::variant(const creature* p) {
	if(!p) {
		type = NoVariant;
		value = 0;
	} else if(p->ishero()) {
		type = Creature;
		value = creature_players_base + bsdata<creature>::source.indexof(p);
	} else {
		type = Creature;
		value = p - location.monsters;
	}
}

creature* variant::getcreature() const {
	if(type != Creature)
		return 0;
	if(value >= creature_players_base)
		return bsdata<creature>::elements + (value - creature_players_base);
	return location.monsters + value;
}