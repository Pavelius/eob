#include "main.h"

INSTDATAC(boosti, 256)

void boosti::clear() {
	memset(this, 0, sizeof(*this));
}

static boosti* find_boost(variant id, variant owner) {
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(; pb < pe; pb++) {
		if(!*pb)
			break;
		if(pb->owner == owner && pb->id == id)
			return pb;
	}
	if(pb == pe)
		return 0;
	pb->owner = owner;
	pb->id = id;
	if(pb + 1 != pe)
		pb[1].clear();
	return pb;
}

bool creature::isaffect(variant id) const {
	variant owner = this;
	for(auto& e : bsdata<boosti>()) {
		if(!e)
			break;
		if(e.owner == owner && e.id == id)
			return true;
	}
	return false;
}

void creature::addboost(variant id, unsigned duration, char value) const {
	if(!duration)
		return;
	auto pb = find_boost(id, this);
	pb->round = game.getrounds() + duration;
	pb->value;
}

void creature::removeboost(variant v) {
	variant owner = this;
	auto pb = bsdata<boosti>::elements;
	auto pe = bsdata<boosti>::elements + (sizeof(bsdata<boosti>::elements) / sizeof(bsdata<boosti>::elements[0]));
	for(auto& e : bsdata<boosti>::elements) {
		if(!e)
			break;
		if(e.id == v && e.owner == owner) {
			update(e);
			continue;
		}
		*pb++ = e;
	}
	if(pb != pe)
		pb->clear();
}

void creature::update(const boosti& e) {
	switch(e.id.type) {
	case Spell: active_spells.remove((spell_s)e.id.value); break;
	case Ability: ability[e.id.value] += e.value; break;
	default: break;
	}
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
			*pb++ = e;
		else
			p->update(e);
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
			p->update(e);
		} else
			*pb++ = e;
	}
	if(pb != pe)
		pb->clear();
}