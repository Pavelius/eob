#include "main.h"

#define FORM(T) {&bsdata<T>::source, getnm<T>, FO(T, name), dginf<T>::meta}

BSDATA(varianti) = {{"None"},
{"Ability", "abilities", FORM(abilityi)},
{"Action", "actions"},
{"Adventure", "adventures", FORM(adventurei), {Editable}},
{"Alignment", "alignments", FORM(alignmenti)},
{"Class", "classes", FORM(classi)},
{"Cleaveress", "cleveress"},
{"Creature", "creatures"},
{"Damage", "damages", FORM(damagei)},
{"Dialog", "dialogs", FORM(dialogi), {Editable}},
{"Enchant", "enchants", FORM(enchanti)},
{"Feat", "feats", FORM(feati)},
{"Gender", "genders", FORM(genderi)},
{"Item", "items", FORM(itemi)},
{"Morale", "morals", FORM(moralei)},
{"Number", "numbers"},
{"Race", "races", FORM(racei)},
{"Reaction", "reactions"},
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
	if(!p->form.source)
		return "None";
	auto pe = p->form.source->ptr(value);
	return *((const char**)((char*)pe + p->form.uname));
}

variant_s varianti::find(const array* source) {
	for(auto& e : bsdata<varianti>()) {
		if(e.form.source == source)
			return variant_s(&e - bsdata<varianti>::elements);
	}
	return NoVariant;
}