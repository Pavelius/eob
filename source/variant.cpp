#include "main.h"

BSDATA(varianti) = {{""},
{"Ability"},
{"Action"},
{"Alignment", &bsdata<alignmenti>::source, getnm<alignmenti>},
{"Class"},
{"Creature"},
{"Damage", &bsdata<damagei>::source, getnm<damagei>},
{"Enchant", &bsdata<enchanti>::source, getnm<enchanti>},
{"Gender", &bsdata<genderi>::source, getnm<genderi>},
{"Item", &bsdata<itemi>::source, getnm<itemi>},
{"Number"},
{"Race", &bsdata<racei>::source, getnm<racei>},
{"Reaction"},
{"Spell"},
};
assert_enum(variant, Spell)
INSTELEM(varianti)

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

const char* variant::getname() const {
	switch(type) {
	case Ability: return bsdata<abilityi>::elements[value].name;
	case Spell: return bsdata<spelli>::elements[value].name;
	default: return "None";
	}
}