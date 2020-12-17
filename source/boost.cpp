#include "main.h"

BSDATAC(boosti, 256)

void boosti::clear() {
	memset(this, 0, sizeof(*this));
}

static boosti* find_boost(variant id, variant owner) {
	for(auto& e : bsdata<boosti>()) {
		if(e.owner == owner && e.id == id)
			return &e;
	}
	auto pb = bsdata<boosti>::add();
	memset(pb, 0, sizeof(*pb));
	pb->owner = owner;
	pb->id = id;
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
	auto pb = bsdata<boosti>::begin();
	for(auto& e : bsdata<boosti>::elements) {
		if(e.id == v && e.owner == owner) {
			update(e);
			continue;
		}
		*pb++ = e;
	}
	bsdata<boosti>::source.setcount(pb - bsdata<boosti>::begin());
}

void creature::update(const boosti& e) {
	switch(e.id.type) {
	case Spell: active_spells.remove((spell_s)e.id.value); break;
	case Ability: ability[e.id.value] += e.value; break;
	default: break;
	}
}

void creature::update_boost() {
	auto rounds = game.getrounds();
	auto pb = bsdata<boosti>::begin();
	for(auto& e : bsdata<boosti>()) {
		if(e.round < rounds) {
			// TODO: apply boost
			auto p = e.owner.getcreature();
			if(!p)
				continue;
			p->update(e);
		} else
			*pb++ = e;
	}
	bsdata<boosti>::source.setcount(pb - bsdata<boosti>::begin());
}