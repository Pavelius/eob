#include "main.h"

boosti bsdata<boosti>::elements[128];

void boosti::clear() {
	memset(this, 0, sizeof(*this));
}

static boosti* find_boost(variant source, variant id, variant owner) {
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(; pb < pe; pb++) {
		if(!*pb)
			break;
		if(pb->owner == owner && pb->source == source && pb->id==id)
			return pb;
	}
	if(pb == pe)
		return 0;
	pb->source = source;
	pb->owner = owner;
	pb->id = id;
	return pb;
}

bool creature::isaffect(variant id) const {
	variant owner = this;
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(; pb < pe; pb++) {
		if(!*pb)
			break;
		if(pb->owner == owner && pb->source == id)
			return true;
	}
	return false;
}

void creature::addboost(variant source, variant id, char value, unsigned duration) const {
	auto pb = find_boost(source, id, this);
	pb->value = value;
	pb->round = game::rounds + duration;
}

void creature::update_boost() {
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(auto& e : bsdata<boosti>::elements) {
		if(!e)
			break;
		if(e.round < game::rounds)
			continue;
		*pb = e;
	}
	if(pb != pe)
		pb->clear();
}

void creature::apply_boost(int m) {
	for(auto& e : bsdata<boosti>::elements) {
		if(!e)
			break;
		auto p = e.owner.getcreature();
		if(!p)
			continue;
		// TODO: apply boost
		switch(e.id.type) {
		case Ability: p->ability[e.id.value] += e.value*m; break;
		default: break;
		}
	}
}