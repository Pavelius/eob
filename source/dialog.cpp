#include "main.h"

static bool allow(const creature* p, const varianti v) {
	switch(v.type) {
	case Race:
		if(p->getrace() == v.race)
			return true;
		break;
	case Alignment:
		if(p->getalignment() == v.alignment)
			return true;
		break;
	case Class:
		if(p->get(v.cls)>0)
			return true;
		break;
	case Item:
		if(p->have(v.item))
			return true;
		break;
	case Spell:
		if(p->getknown(v.spell)>0)
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
		auto pe = (elementi*)aw.choosebg(p->text, border, p->overlay);
		if(!pe || !pe->next[0])
			break;
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