#include "main.h"

#define FORM(T) {&bsdata<T>::source, getnm<T>, FO(T, name), dginf<T>::meta}

BSDATA(varianti) = {{"None"},
{"Ability", "abilities", FORM(abilityi)},
{"Action", "actions"},
{"Adventure", "adventures", FORM(adventurei), {VarTextable}},
{"Alignment", "alignments", FORM(alignmenti)},
{"Building", "buildings", FORM(buildingi)},
{"Case", "cases"},
{"Class", "classes", FORM(classi)},
{"Cleaveress", "cleveress"},
{"Condition", "conditions"},
{"Creature", "creatures"},
{"Damage", "damages", FORM(damagei)},
{"Enchant", "enchants", FORM(enchanti)},
{"Event", "events", FORM(eventi)},
{"Feat", "feats", FORM(feati)},
{"Gender", "genders", FORM(genderi)},
{"Item", "items", FORM(itemi)},
{"Morale", "morals", FORM(moralei)},
{"Race", "races", FORM(racei)},
{"Reaction", "reactions"},
{"Settlement", "settlements", FORM(settlementi), {VarTextable}},
{"Spell", "spells", FORM(spelli)},
};
assert_enum(variant, Spell)
INSTELEM(varianti)

const unsigned creature_players_base = 240;

variant::variant(variant_s v, const void* p) {
	if(!p) {
		type = NoVariant;
		value = 0;
	} else {
		type = v;
		value = bsdata<varianti>::elements[v].form.source->indexof(p);
	}
}

variant::variant(const void* p) {
	if(!p) {
		type = NoVariant;
		value = 0;
	} else if(bsdata<creature>::source.indexof(p)!=-1) {
		type = Creature;
		value = creature_players_base + bsdata<creature>::source.indexof(p);
	} else if(p>=location.monsters && p<=(location.monsters + sizeof(location.monsters)/sizeof(location.monsters[0]))) {
		type = Creature;
		value = (creature*)p - location.monsters;
	} else {
		type = NoVariant;
		value = 0;
		for(auto i = (variant_s)1; i <= Spell; i = (variant_s)(i + 1)) {
			if(!bsdata<varianti>::elements[i].form.source)
				continue;
			if(bsdata<varianti>::elements[i].form.source->indexof(p) != -1) {
				type = i;
				value = bsdata<varianti>::elements[i].form.source->indexof(p);
				break;
			}
		}
	}
}

void* variant::getpointer(variant_s t) const {
	if(type != t)
		return 0;
	return bsdata<varianti>::elements[t].form.source->ptr(value);
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
	if(!p->form.source)
		return "None";
	auto pe = p->form.source->ptr(value);
	if(p->flags.is(VarTextable))
		return ((textable*)((char*)pe + p->form.uname))->getname();
	return *((const char**)((char*)pe + p->form.uname));
}

point variant::getposition() const {
	switch(type) {
	case Settlement: return getsettlement()->position;
	case Adventure: return getadventure()->position;
	default: return {0, 0};
	}
}

variant_s varianti::find(const array* source) {
	for(auto& e : bsdata<varianti>()) {
		if(e.form.source == source)
			return variant_s(&e - bsdata<varianti>::elements);
	}
	return NoVariant;
}