#include "main.h"

void creaturea::match(variant id, bool remove) {
	auto ps = data;
	for(auto p : *this) {
		if(p->ismatch(id) == remove)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

creature* creaturea::getbest(ability_s v) {
	auto pc = (creature*)0;
	auto v2 = -1;
	for(auto p : *this) {
		auto v1 = p->get(v);
		if(v1 <= 0)
			continue;
		if(v2 == -1 || v1 > v2) {
			v2 = v1;
			pc = p;
		}
	}
	return pc;
}

void creaturea::match(const messagei& id, bool remove) {
	auto ps = data;
	for(auto p : *this) {
		if(p->ismatch(id) == remove)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::select(indext index) {
	if(game.getcamera() == index) {
		for(auto v : party) {
			auto p = v.getcreature();
			if(!p || !p->isready())
				continue;
			add(p);
		}
	} else {
		creature* monster_data[4];
		location.getmonsters(monster_data, index, Right);
		for(auto p : monster_data) {
			if(!p || !p->isready())
				continue;
			add(p);
		}
	}
}

static int compare_parcipants(const void* p1, const void* p2) {
	auto pc1 = *((creature**)p1);
	auto pc2 = *((creature**)p2);
	int i1 = pc1->getinitiative();
	int i2 = pc2->getinitiative();
	return i2 - i1;
}

void creaturea::rollinitiative() {
	for(auto pc : *this) {
		int value = xrand(1, 10);
		value += pc->getspeed();
		pc->setinitiative(value);
		pc->setmoved(true);
	}
	qsort(data, count, sizeof(data[0]), compare_parcipants);
}

void creaturea::set(reaction_s v) {
	for(auto pc : *this)
		pc->set(v);
}

void creaturea::leave() {
	for(auto pc : *this)
		pc->clear();
}

void creaturea::kill() {
	for(auto pc : *this)
		pc->kill();
}