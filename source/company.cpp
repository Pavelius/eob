#include "main.h"

BSDATAC(adventurei, 128)
BSDATAC(settlementi, 64)
BSDATAC(eventi, 256)

unsigned historyi::gethistorymax() const {
	for(unsigned i = 0; i < history_max; i++)
		if(!history[i])
			return i;
	return history_max;
}

settlementi* resultable::getsettlement() const {
	return game.getsettlement();
}

bool resultable::have(variant v) const {
	for(auto e : actions) {
		if(!e)
			break;
		if(e == v)
			return true;
	}
	return false;
}

bool resultable::isallow() const {
	for(auto e : actions) {
		if(!e)
			break;
		if(e.type == Case)
			continue;
		else if(e.type == ActionSet) {
			auto ps = (actionseti*)e.getpointer(ActionSet);
			if(ps->check) {
				auto v = game.get(ps->action);
				auto vm = ps->roll();
				if(v < vm)
					return false;
			}
		} else if(!party.have(e))
			return false;
	}
	return true;
}

void eventi::clear() {
	memset(this, 0, sizeof(*this));
}

void eventi::shufle() const {
	auto& source = game.getevents();
	source.addbottom(this);
}

void eventi::discard() const {
	auto& source = game.getevents();
	source.discard(this);
}

void eventi::apply(case_s v, bool interactive) const {
	for(auto& e : results) {
		if(!e)
			break;
		if(!e.have(v))
			continue;
		if(!e.isallow())
			continue;
		if(interactive)
			answers::message(e.getname());
		auto need_reshufle = false;
		for(auto a : e.actions) {
			if(a == variant(Reshufle))
				need_reshufle = true;
			else
				game.apply(a);
		}
		discard();
		if(need_reshufle)
			shufle();
		break;
	}
}

void eventi::play() const {
	answers aw;
	aw.add(Case1, ask[0]);
	aw.add(Case2, ask[1]);
	auto i = (case_s)aw.choosebg(*this, false);
	apply(i, true);
}