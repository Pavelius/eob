#include "main.h"

BSDATAC(boosti, 256)

void boosti::clear() {
	memset(this, 0, sizeof(*this));
}

static boosti* find_boost(spell_s id, variant owner) {
	for(auto& e : bsdata<boosti>()) {
		if(e.owner == owner && e.id == id)
			return &e;
	}
	auto pb = bsdata<boosti>::add();
	pb->clear();
	pb->owner = owner;
	pb->id = id;
	return pb;
}

void creature::addboost(spell_s id, unsigned duration) {
	if(!duration)
		return;
	auto pb = find_boost(id, this);
	pb->round = game.getrounds() + duration;
	active_spells.set(id);
}

void creature::remove(spell_s v) {
	if(!active_spells.is(v))
		return;
	variant owner = this;
	auto pb = bsdata<boosti>::begin();
	for(auto& e : bsdata<boosti>()) {
		if(e.id == v && e.owner == owner)
			continue;
		*pb++ = e;
	}
	bsdata<boosti>::source.setcount(pb - bsdata<boosti>::begin());
}