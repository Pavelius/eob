#include "main.h"

#define FORM(T) &bsdata<T>::source, getnm<T>, dginf<T>::meta

static array game_source(&game, sizeof(companyi), 1);

BSDATA(varianti) = {
	{"None"},
	{"Ability", "abilities", {Action}, FORM(abilityi)},
	{"Action", "actions", {Action}},
	{"Adventure", "adventures", {}},
	{"Alignment", "alignments", {Action}, FORM(alignmenti)},
	{"Case", "cases", {Action}, FORM(casei)},
	{"Cell", "cells"},
	{"Class", "classes", {Action}, FORM(classi)},
	{"Cleaveress", "cleveress"},
	{"Condition", "conditions", {Action}},
	{"Creature", "creatures", {}, FORM(creature)},
	{"Damage", "damages", {}, FORM(damagei)},
	{"Enchant", "enchants", {}, FORM(enchanti)},
	{"Feat", "feats", {Action}, FORM(feati)},
	{"Gender", "genders", {Action}, FORM(genderi)},
	{"Item", "items", {Action}, FORM(itemi)},
	{"Morale", "morals", {}, FORM(moralei)},
	{"Race", "races", {Action}, FORM(racei)},
	{"Rarity", "rarities"},
	{"Reaction", "reactions", {Action}},
	{"Resource", "resources"},
	{"Spell", "spells", {}, FORM(spelli)},
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

const char* variant::getname() const {
	static char strings[2][128];
	static stringbuilder sbs[] = {strings[0], strings[1]};
	static unsigned char counter;
	auto p = bsdata<varianti>::elements + type;
	if(!p->source)
		return "None";
	auto pe = p->source->ptr(value);
	auto& sb = sbs[(counter++) % 1];
	sb.clear();
	return p->pgetname(pe, sb);
}

point variant::getposition() const {
	return {0, 0};
}

varianti* varianti::find(const markup* v) {
	for(auto& e : bsdata<varianti>()) {
		if(e.form == v)
			return &e;
	}
	return 0;
}

variant_s varianti::find(const array* source) {
	for(auto& e : bsdata<varianti>()) {
		if(e.source == source)
			return variant_s(&e - bsdata<varianti>::elements);
	}
	return NoVariant;
}

variant variant::find(const char* name) {
	if(!name || name[0] == 0)
		return variant();
	char temp[260]; stringbuilder sb(temp);
	for(auto& ei : bsdata<varianti>()) {
		if(!ei.source || !ei.pgetname)
			continue;
		auto m = ei.source->getcount();
		for(unsigned v = 0; v < m; v++) {
			sb.clear();
			auto object = ei.source->ptr(v);
			auto pn = ei.pgetname(object, sb);
			if(pn && pn[0] && strcmp(name, pn) == 0) {
				auto t = (variant_s)(&ei - bsdata<varianti>::elements);
				return {t, (unsigned char)v};
			}
		}
		sb.clear();
	}
	return variant();
}

variant varianti::find(const char* name) const {
	if(!name || name[0] == 0 || !source || !pgetname)
		return variant();
	char temp[260]; stringbuilder sb(temp);
	auto m = source->getcount();
	for(unsigned v = 0; v < m; v++) {
		sb.clear();
		auto object = source->ptr(v);
		auto pn = pgetname(object, sb);
		if(pn && pn[0] && strcmp(name, pn) == 0) {
			auto t = (variant_s)(this - bsdata<varianti>::elements);
			return {t, (unsigned char)v};
		}
	}
	sb.clear();
	return variant();
}

void varianti::getmetadata(stringbuilder& sb) {
	sb.addn("{");
	auto result = 0;
	for(auto& e : bsdata<varianti>()) {
		if(result)
			sb.add(",");
		sb.addn("\t\"%1\" : {", e.name);
		if(e.form) {
			auto requisit_count = 0;
			for(auto pm = e.form; *pm; pm++) {
				if(!pm->title)
					continue;
				if(requisit_count)
					sb.add(",");
				if(pm->value.type)
					sb.addn("\t\t\"%1\" : \"%2\"", pm->title, pm->value.type->title);
				else
					sb.addn("\t\t\"%1\" : \"complex\"", pm->title);
				requisit_count++;
			}
		}
		sb.addn("}");
		result++;
	}
	sb.addn("}");
}