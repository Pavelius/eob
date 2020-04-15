#include "main.h"

boosti bsdata<boosti>::elements[128];

void boosti::clear() {
	memset(this, 0, sizeof(*this));
}

boosti* find_boost(variant source, variant owner, bool add_if_not_exist) {
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(; pb < pe; pb++) {
		if(!*pb)
			break;
		if(pb->owner == owner && pb->source == source)
			return pb;
	}
	if(!add_if_not_exist || pb == pe)
		return 0;
	pb->source = source;
	pb->owner = owner;
	return pb;
}

bool creature::isaffect(variant id) const {
	return find_boost(id, this, false) != 0;
}

void creature::addboost(variant source, variant id, char value, unsigned duration) const {
	auto pb = find_boost(source, this, true);
	pb->id = id;
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