#include "main.h"

static array game_source(&game, sizeof(companyi), 1);

BSDATA(varianti) = {
	{"None"},
	{"Ability", "abilities", {Action}},
	{"Action", "actions", {Action}},
	{"Adventure", "adventures", {}},
	{"Alignment", "alignments", {Action}},
	{"Case", "cases", {Action}},
	{"Cell", "cells"},
	{"Class", "classes", {Action}},
	{"Cleaveress", "cleveress"},
	{"Condition", "conditions", {Action}},
	{"Creature", "creatures", {}},
	{"Damage", "damages", {}},
	{"Enchant", "enchants", {}},
	{"Feat", "feats", {Action}},
	{"Gender", "genders", {Action}},
	{"Item", "items", {Action}},
	{"Morale", "morals", {}},
	{"Race", "races", {Action}},
	{"Rarity", "rarities"},
	{"Reaction", "reactions", {Action}},
	{"Resource", "resources"},
	{"Spell", "spells", {}},
};
assert_enum(variant, Spell)
BSDATAF(varianti)

const unsigned creature_players_base = 240;

variant::variant(variant_s v, const void* p) {
	if(!p) {
		type = NoVariant;
		value = 0;
	} else {
		type = v;
		value = bsdata<varianti>::elements[v].source->indexof(p);
	}
}

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
		for(auto i = (variant_s)1; i <= Spell; i = (variant_s)(i + 1)) {
			if(!bsdata<varianti>::elements[i].source)
				continue;
			if(bsdata<varianti>::elements[i].source->indexof(p) != -1) {
				type = i;
				value = bsdata<varianti>::elements[i].source->indexof(p);
				break;
			}
		}
	}
}

void* variant::getpointer(variant_s t) const {
	if(type != t)
		return 0;
	return bsdata<varianti>::elements[t].source->ptr(value);
}

creature* variant::getcreature() const {
	if(type != Creature)
		return 0;
	if(value >= creature_players_base)
		return bsdata<creature>::elements + (value - creature_players_base);
	return location.monsters + value;
}

variant_s varianti::find(const array* source) {
	for(auto& e : bsdata<varianti>()) {
		if(e.source == source)
			return variant_s(&e - bsdata<varianti>::elements);
	}
	return NoVariant;
}

const char* variant::getname() const {
	switch(type) {
	case Item: return bsdata<itemi>::elements[value].name;
	case Enchant: return bsdata<enchanti>::elements[value].name;
	case Spell: return bsdata<spelli>::elements[value].name;
	default: return "No name";
	}
}