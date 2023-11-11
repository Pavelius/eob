#include "main.h"

//BSDATA(varianti) = {
//	{"None"},
//	{"Ability"},
//	{"Action"},
//	{"Adventure"},
//	{"Alignment"},
//	{"Case"},
//	{"Cell"},
//	{"Class"},
//	{"Cleaveress"},
//	{"Condition"},
//	{"Creature"},
//	{"Damage"},
//	{"Enchant"},
//	{"Feat"},
//	{"Gender"},
//	{"Item"},
//	{"Morale"},
//	{"Race"},
//	{"Rarity"},
//	{"Reaction"},
//	{"Resource"},
//	{"Spell"},
//};
//assert_enum(varianti, Spell)
//BSDATAF(varianti)

const unsigned creature_players_base = 240;

variant::variant(const void* p) {
	if(!p) {
		type = NoVariant;
		value = 0;
	} else if(bsdata<creature>::source.indexof(p) != -1) {
		type = Creature;
		value = creature_players_base + bsdata<creature>::source.indexof(p);
	} else if(p >= location.monsters && p <= (location.monsters + sizeof(location.monsters) / sizeof(location.monsters[0]))) {
		type = Creature;
		value = (creature*)p - location.monsters;
	} else {
		type = NoVariant;
		value = 0;
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
	case Item: return bsdata<itemi>::elements[value].name;
	case Enchant: return bsdata<enchanti>::elements[value].name;
	case Spell: return bsdata<spelli>::elements[value].name;
	default: return "No name";
	}
}