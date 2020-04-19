#include "main.h"

INSTDATAC(boosti, 256)

void boosti::clear() {
	memset(this, 0, sizeof(*this));
}

static boosti* find_boost(variant source, variant id, variant owner) {
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(; pb < pe; pb++) {
		if(!*pb)
			break;
		if(pb->owner == owner && pb->source == source && pb->id == id)
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
	if(!duration)
		return;
	auto pb = find_boost(source, id, this);
	pb->value = value;
	pb->round = game.getrounds() + duration;
}

void creature::removeboost(variant v) const {
	variant owner = this;
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(auto& e : bsdata<boosti>::elements) {
		if(!e)
			break;
		if(e.id == v && e.owner == owner)
			continue;
		*pb++ = e;
	}
	if(pb != pe)
		pb->clear();
}

void creature::clearboost() {
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(auto& e : bsdata<boosti>::elements) {
		if(!e)
			break;
		auto p = e.owner.getcreature();
		if(!p)
			continue;
		if(p->ishero())
			continue;
		switch(e.id.type) {
		case Ability: p->ability[e.id.value] += e.value; break;
		default: break;
		}
		*pb++ = e;
	}
	if(pb != pe)
		pb->clear();
}

void creature::update_boost() {
	auto rounds = game.getrounds();
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(auto& e : bsdata<boosti>::elements) {
		if(!e)
			break;
		if(e.round < rounds) {
			// TODO: apply boost
			auto p = e.owner.getcreature();
			if(!p)
				continue;
			switch(e.id.type) {
			case Ability: p->ability[e.id.value] += e.value; break;
			default: break;
			}
		} else
			*pb++ = e;
	}
	if(pb != pe)
		pb->clear();
}