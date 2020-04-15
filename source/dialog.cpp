#include "main.h"

static bool allow(const creature* p, const varianti v) {
	switch(v.type) {
	case Race:
		if(p->getrace() == v.value)
			return true;
		break;
	case Alignment:
		if(p->getalignment() == v.value)
			return true;
		break;
	case Class:
		if(p->get((class_s)v.value)>0)
			return true;
		break;
	case Item:
		if(p->have((item_s)v.value))
			return true;
		break;
	case Spell:
		if(p->getknown((spell_s)v.value)>0)
			return true;
		break;
	case Reaction:
		if(p->getreaction() == v.value)
			return true;
		break;
	default:
		return true;
	}
	return false;
}

static bool allowparty(const varianti v) {
	for(auto p : game::party) {
		if(!p)
			continue;
		if(allow(p, v))
			return true;
	}
	return false;
}

static bool allowpartyitem(const item_s v) {
	for(auto p : game::party) {
		if(!p)
			continue;
		if(p->find(v))
			return true;
	}
	return false;
}

bool dialogi::actioni::isallow() const {
	switch(action) {
	case HaveVariant:
	case RemoveVariant:
		if(!allowparty(variant))
			return false;
		break;
	case RessurectBones:
		if(!allowpartyitem(Bones))
			return false;
		break;
	}
	return true;
}

void dialogi::actioni::apply() {
	auto party_index = game::getcamera();
	auto party_direction = game::getdirection();
	auto monster_index = to(party_index, party_direction);
	creature* creatures[4]; location.getmonsters(creatures, monster_index, party_direction);
	switch(action) {
	case WinCombat:
		for(auto p : creatures) {
			if(p && p->gethits() > 0)
				p->damage(Magic, p->gethits(), 5);
		}
		break;
	case StartCombat:
		location.set(monster_index, Hostile);
		break;
	case GainExperience:
		creature::addexp(variant.value, 0);
		break;
	case HealParty:
		creature::apply(&creature::heal);
		break;
	case AddVariant:
		switch(variant.type) {
		case Item:
			creature::addparty((item_s)variant.value);
			break;
		case State:
			for(auto p : game::party) {
				if(p)
					p->add((state_s)variant.value, xrand(3, 10) * 5);
			}
			break;
		case Condition:
			for(auto p : game::party) {
				if(p)
					p->add((condition_s)variant.value);
			}
			break;
		case Reaction:
			location.set(monster_index, (reaction_s)variant.value);
			break;
		}
		break;
	}
}

bool dialogi::elementi::isallow() const {
	if(!text)
		return false;
	for(auto& e : actions) {
		if(!e)
			break;
		if(!e.isallow())
			return false;
	}
	return true;
}

void dialogi::elementi::apply() {
	for(auto& e : actions) {
		if(!e)
			break;
		e.apply();
	}
}

void dialogi::choose(bool border) const {
	draw::animation::update();
	auto p = find("main");
	while(p) {
		answers aw;
		for(auto& e : p->variants) {
			if(!e.isallow())
				continue;
			aw.add((int)&e, e.text);
		}
		auto horizontal = true;
		if(!aw.elements) {
			horizontal = true;
			aw.add(0, "Next");
		}
		auto pe = (elementi*)aw.choosebg(p->text, border, p->overlay, horizontal);
		if(!pe || !pe->next[0])
			break;
		pe->apply();
		p = find(pe->next[0]);
	}
}

const dialogi* dialogi::find(const char* id) const {
	if(!this || !id)
		return 0;
	for(auto p = this; *p; p++) {
		if(strcmp(p->id, id) == 0)
			return p;
	}
	return 0;
}