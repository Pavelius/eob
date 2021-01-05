#include "main.h"

static int compare(const void* v1, const void* v2) {
	auto p1 = (variant*)v1;
	auto p2 = (variant*)v2;
	return strcmp(p1->getname(), p2->getname());
}

void variantc::cspells(const creature* player, bool expand) {
	for(auto i = Bless; i <= LastSpellAbility; i = (spell_s)(i + 1)) {
		auto j = player->get(i);
		if(!j)
			continue;
		if(expand) {
			while(j > 0) {
				add(i);
				j--;
			}
		} else
			add(i);
	}
}

static int getlevel(spell_s v, class_s c) {
	int i;
	auto& ei = bsdata<spelli>::elements[v];
	switch(c) {
	case NoClass:
		i = ei.levels[1];
		if(!i && ei.levels[0] && i > ei.levels[0])
			i = ei.levels[0];
		return i;
	case Cleric: return ei.levels[1];
	default: return ei.levels[0];
	}
}

void variantc::match(point start, int radius, bool keep) {
	auto p = data;
	for(auto& e : *this) {
		if(e.type == Settlement) {
			auto& ei = bsdata<settlementi>::elements[e.value];
			auto v = ei.position.range(start) <= radius;
			if(v != keep)
				continue;
		} else
			continue;
		*p++ = e;
	}
	count = p - data;
}

void variantc::match(variant v, bool keep) {
	auto p = data;
	for(auto& e : *this) {
		if(e.type == Adventure) {
			auto r = e.getadventure()->match(v);
			if(r != keep)
				continue;
		} else
			continue;
		*p++ = e;
	}
	count = p - data;
}

void variantc::matchsl(class_s c, int level) {
	auto p = data;
	for(auto& e : *this) {
		if(e.type != Spell)
			continue;
		auto& ei = bsdata<spelli>::elements[e.value];
		if(getlevel((spell_s)e.value, c) != level)
			continue;
		*p++ = e;
	}
	count = p - data;
}

void variantc::exclude(variant v) {
	auto p = data;
	for(auto& e : *this) {
		if(e == v)
			continue;
		*p++ = e;
	}
	count = p - data;
}

void variantc::sort() {
	qsort(data, count, sizeof(data[0]), compare);
}

void variantc::select(variant_s type) {
	auto& ei = bsdata<varianti>::elements[type];
	auto p = ei.form.source;
	if(!p)
		return;
	variant v;
	v.type = type;
	for(v.value = 0; v.value <= p->getcount(); v.value++) {
		auto pn = v.getname();
		if(!pn || pn[0] == 0)
			continue;
		add(v);
	}
}

bool adventurei::match(variant v) const {
	switch(v.type) {
	case Settlement: return settlement == v.value;
	default: return false;
	}
}