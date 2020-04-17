#include "main.h"

void creaturea::select(short unsigned index) {
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