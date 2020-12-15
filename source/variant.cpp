#include "main.h"

#define DECL(T) &bsdata<T>::source, getnm<T>, FO(T, name), dginf<T>::meta

BSDATA(varianti) = {{"None"},
{"Ability", DECL(abilityi)},
{"Action"},
{"Alignment", DECL(alignmenti)},
{"Class", DECL(classi)},
{"Creature"},
{"Damage", DECL(damagei)},
{"Enchant", DECL(enchanti)},
{"Feat", DECL(feati)},
{"Gender", DECL(genderi)},
{"Item", DECL(itemi)},
{"Number"},
{"Race", DECL(racei)},
{"Reaction"},
{"Spell", DECL(spelli)},
};
assert_enum(variant, Spell)
INSTELEM(varianti)

const unsigned creature_players_base = 240;

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
	auto p = bsdata<varianti>::elements + type;
	if(!p->source)
		return "None";
	auto pe = p->source->ptr(value);
	return *((const char**)((char*)pe + p->uname));
}