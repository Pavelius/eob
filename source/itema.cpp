#include "main.h"

void itema::select() {
	for(auto p : party) {
		if(!p)
			continue;
		p->select(*this);
	}
}

void itema::select(adat<item>& source) {
	for(auto& e : source)
		add(&e);
}

void itema::is(good_s v, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(p->is(v) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::match(const item::typea& list, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(p->match(list) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::havespell(const creature* pc, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto pe = p->getenchantment();
		auto r = pe && pe->power.type == Spell && pc->isknown((spell_s)pe->power.value);
		if(r != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::match(rarity_s v, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto result = (p->getrarity() <= v);
		if(result != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::match(const goodf& e, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto result = e.is(p->gete().goods);
		if(result != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::maxcost(int v, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto result = (p->getcostgp() <= v);
		if(result != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::select(pitem proc, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if((p->*proc)() != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::forsale(bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto value = p->getcost() > 0;
		if(p->isidentified() && p->ismagical())
			value = false;
		if(value != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

item* itema::random() {
	if(!count)
		return 0;
	return data[rand() % count];
}

static int compare(const void* v1, const void* v2) {
	auto p1 = (item**)v1;
	auto p2 = (item**)v2;
	char t1[260], t2[260];
	stringbuilder s1(t1), s2(t2);
	(*p1)->getname(s1);
	(*p2)->getname(s2);
	return strcmp(t1, t2);
}

void itema::sort() {
	qsort(data, count, sizeof(data[0]), compare);
}