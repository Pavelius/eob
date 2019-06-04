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

bool allowparty(const varianti v) {
	for(auto p : game::party) {
		if(!p)
			continue;
		if(allow(p, v))
			return true;
	}
	return false;
}

bool dialogi::elementi::isallow() const {
	switch(action) {
	case HaveVariant:
	case RemoveVariant:
		if(!allowparty(variant))
			return false;
		break;
	}
	return true;
}

void dialogi::choose(bool border) const {
	auto p = find("main");
	while(p) {
		answers aw;
		for(auto& e : p->variants) {
			if(!e.isallow())
				continue;
			aw.add((int)&e, e.text);
		}
		auto pe = (elementi*)aw.choosebg(p->text, border, p->overlay);
		if(!pe || !pe->success)
			break;
		p = find(pe->success);
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