#include "main.h"

static messagei dragon_text[] = {{Say, 1, {Indifferent}, " - Are you from master?"},
{Say, 1, {Indifferent}, " - Wait a minute! How you can dig so deep?"},
{Ask, 1, {StartCombat}, "Attack"},
{Ask, 1, {Charisma}, "Lie", {12, 13}},
{Say, 3, {}, " - Who is you? You are not from this place."},
{Say, 12, {}, " - Hello, friends. Glad to see you. What can we do for you?"},
{Ask, 12, {Trading}, "Trade"},
{Say, 13, {}, " - You liers! Prepare to die!", StartCombat},
{}};

static void apply_variant(variant id) {
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
			creature::addexp(1000, 0);
			break;
		case HealParty:
			creature::apply(&creature::heal);
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
	for(auto v : variants) {
		if(!v)
			break;
	}
	return true;
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
		apply_variant(v);
	}
}

void messagei::choose(bool border) const {
	draw::animation::update();
	auto p = find(1);
	while(p) {
		answers aw;
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
		auto next_id = pe->next[0];
		if(pe->next[1] && make_test(pe, false))
			next_id = pe->next[1];
		p = find(next_id);
	}
}