#include "main.h"

bool creaturea::have(variant v) const {
	for(auto p : *this) {
		if(p->ismatch(v))
			return true;
	}
	return false;
}

void creaturea::match(variant id, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(p->ismatch(id) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::match(const conditiona& id, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(p->ismatch(id) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

creature* creaturea::getbest(ability_s v) const {
	auto pc = (creature*)0;
	auto v2 = -1;
	for(auto p : *this) {
		auto v1 = p->get(v);
		if(v1 <= 0)
			continue;
		if(v2 == -1 || v1 > v2) {
			v2 = v1;
			pc = const_cast<creature*>(p);
		}
	}
	return pc;
}

int	creaturea::getaverage(ability_s v) const {
	auto total = 0, count = 0;
	for(auto p : *this) {
		if(!p->isready())
			continue;
		total += p->get(v);
		count++;
	}
	if(!count)
		return 0;
	return total / count;
}

void creaturea::select() {
	for(auto p : party) {
		if(!p->isready())
			continue;
		add(p);
	}
}

void creaturea::select(indext index) {
	if(game.getcamera() == index)
		select();
	else {
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
	for(auto p : *this)
		p->set(v);
}

void creaturea::leave() {
	for(auto p : *this)
		p->clear();
}

void creaturea::resolve() {
	auto award = 0;
	for(auto p : *this) {
		award += p->getawards();
		p->clear();
	}
	game.addexpc(award, 0);
}

void creaturea::kill() {
	for(auto pc : *this)
		pc->kill();
}

creature* creaturea::getmostdamaged() const {
	creature* result = 0;
	int difference = 0;
	for(auto p : *this) {
		int hp = p->gethits();
		int mhp = p->gethitsmaximum();
		if(hp == mhp)
			continue;
		auto n = mhp - hp;
		if(n > difference) {
			result = p;
			difference = n;
		}
	}
	return result;
}

creature* creaturea::choose() const {
	answers elements;
	for(auto p : *this) {
		if(p)
			elements.add((int)p, p->getname());
	}
	return (creature*)elements.choosesm("On which hero?", true);
}

void creaturea::apply(creaturea::fnapply proc) {
	for(auto p : *this) {
		(p->*proc)();
	}
}

bool creaturea::additem(item v, bool interactive) {
	for(auto p : *this) {
		if(p->add(v)) {
			if(interactive) {
				char t1[260]; stringbuilder s1(t1); v.getname(s1);
				mslog("%1 gain %2", p->getname(), t1);
			}
			return true;
		}
	}
	if(location) {
		location.dropitem(game.getcamera(), v, game.getside(0, game.getdirection()));
		return true;
	}
	return false;
}