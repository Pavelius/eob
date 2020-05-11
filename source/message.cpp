#include "main.h"

static reaction_s current_reaction;

static void trade_creatures(const selli* goods) {
	itema items;
	items.select();
	items.forsale(false);
	auto pi = items.choose("Sell which item?", true);
	if(!pi)
		return;
	auto cost = pi->getcost();
	for(int i = 3; cost > 0; i--) {
		if(i == 0)
			i = 3;
		if(i > cost)
			continue;
		item it(goods[i].object);
		it.setpower(goods[i].rarity);
		creature::addparty(it, true);
		cost -= i;
	}
	pi->clear();
}

static void apply_variant(variant id, const selli* trade) {
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
				p->kill();
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
			trade_creatures(trade);
			break;
		case DeathSave:
			for(auto p : party) {
				if(!p->roll(SaveVsParalization))
					p->damage(Magic, p->gethits() + 10, 5);
			}
			break;
		case TrapDamage:
			for(auto p : party) {
				if(!p->roll(SaveVsTraps))
					p->damage(Pierce, dice::roll(2, 6), 5);
			}
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

const messagei* messagei::find(int id, bool test_allow) const {
	if(!this || !id)
		return 0;
	for(auto p = this; *p; p++) {
		if(p->id == id) {
			if(test_allow) {
				while(*p && p->id==id) {
					if(p->isallow())
						return p;
					p++;
				}
				return 0;
			}
			return p;
		}
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
		apply_variant(v, trade);
	}
}

void messagei::choose(bool border, int next_id, reaction_s reaction) const {
	creature::setcom(reaction);
	draw::animation::update();
	while(next_id) {
		auto p = find(next_id, true);
		if(!p)
			break;
		answers aw;
		p->apply();
		for(auto pe = p; *pe; pe++) {
			if(pe->type != Ask || pe->id != p->id)
				continue;
			if(!pe->isallow())
				continue;
			aw.add((int)pe, pe->text);
		}
		auto need_apply = true;
		if(!aw.elements) {
			need_apply = false;
			aw.add((int)p, "Next");
		}
		auto pe = (messagei*)aw.choosebg(p->text, need_apply ? "What do you do?" : 0, p->overlay, true);
		if(!pe)
			break;
		if(need_apply)
			pe->apply();
		next_id = pe->next[0];
		if(pe->next[1] && make_test(pe, false))
			next_id = pe->next[1];
	}
}