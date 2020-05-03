#include "main.h"

static selli sell_low[] = {{BlueRing, 3}, {{PotionBlue, OfHealing, 0}, 2}, {RationIron, 1}, {}};

static void trade_creatures(selli* goods) {
	itema items;
	items.select();
	items.forsale(false);
	auto pi = items.choose("Sell which item?", true);
	auto cost = pi->getcost();
	auto pg = goods;
	while(cost > 0) {
		if(!pg->cost)
			pg = goods;
		if(pg->cost > cost) {
			if(pg->cost > 1)
				break;
			pg++;
			continue;
		}
		//creature::add(pg->object);
		cost -= pg->cost;
	}
	pi->clear();
}

static void apply_variant(variant id, const messagei& em) {
	creaturea party, opponents;
	auto party_index = game.getcamera();
	auto party_direction = game.getdirection();
	auto monster_index = to(party_index, party_direction);
	party.select(party_index);
	opponents.select(monster_index);
	if(id.type == Action) {
		switch(id.value) {
		case WinCombat:
			for(auto p : opponents)
				p->damage(Magic, p->gethits(), 5);
			break;
		case LeaveAway:
			for(auto p : opponents)
				p->clear();
			break;
		case StartCombat:
			for(auto p : opponents)
				p->set(Hostile);
			break;
		case GainExperience:
			creature::addexp(500, 0);
			break;
		case HealParty:
			creature::apply(&creature::heal);
			break;
		case Trade:
			trade_creatures(em.trade);
			break;
		}
	}
}

static bool allowed(const variant& v) {
	for(auto vc : party) {
		auto p = vc.getcreature();
		if(!p)
			continue;
		if(p->ismatch(v))
			return true;
	}
	return false;
}

static bool allowed(indext index, const variant& v) {
	creature* creatures[4]; location.getmonsters(creatures, index, Up);
	for(auto p : creatures) {
		if(!p)
			continue;
		if(p->ismatch(v))
			return true;
	}
	return false;
}

bool messagei::isallow() const {
	creaturea parcipants;
	parcipants.select(game.getcamera());
	parcipants.match(*this, false);
	return parcipants.getcount() > 0;
}

const messagei* messagei::find(int id) const {
	if(!this || !id)
		return 0;
	for(auto p = this; *p; p++) {
		if(p->id == id)
			return p;
	}
	return 0;
}

static bool make_test(const messagei* p, bool can_help) {
	creaturea parcipants;
	parcipants.select(game.getcamera());
	parcipants.match(*p, false);
	for(auto& e : p->variants) {
		switch(e.type) {
		case Ability:
			if(can_help) {
				for(auto pc : parcipants) {
					if(pc->roll((ability_s)e.value))
						return true;
				}
			} else {
				auto pc = parcipants.getbest((ability_s)e.value);
				if(pc->roll((ability_s)e.value))
					return true;
			}
			break;
		}
	}
	return false;
}

void messagei::apply() const {
	for(auto v : variants) {
		if(!v)
			break;
		apply_variant(v, *this);
	}
}

void messagei::choose(bool border) const {
	draw::animation::update();
	auto next_id = 1;
	while(next_id) {
		auto p = find(next_id);
		if(!p)
			break;
		answers aw;
		p->apply();
		if(p->next[0]) {
			next_id = p->next[0];
			continue;
		}
		for(auto pe = p; *pe; pe++) {
			if(pe->type != Ask || pe->id != p->id)
				continue;
			if(!pe->isallow())
				continue;
			aw.add((int)pe, pe->text);
		}
		if(!aw.elements)
			aw.add(0, "Next");
		auto pe = (messagei*)aw.choosebg(p->text, border, p->overlay, true);
		if(!pe)
			break;
		pe->apply();
		next_id = pe->next[0];
		if(pe->next[1] && make_test(pe, false))
			next_id = pe->next[1];
	}
}