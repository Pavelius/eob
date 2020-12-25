#include "main.h"

struct chati {
	action_s			action;
	conditiona			conditions;
	const char*			text;
};

static chati messages[] = {{Greeting, {Indifferent}, "\"Who is you? How you get there?\""},
{Greeting, {Indifferent}, "\"Who is there? How you dig so deep?\""},
{Greeting, {Indifferent}, "\"What a ... Who is you?\""},
{Greeting, {Friendly, High}, "\"Welcome, my very best friends! I so glad to see you! What can I do for you?\""},
{Greeting, {Friendly}, "\"Welcome friends, I am at your service! What can I do for your?\""},
{FailLie, {}, "\"You are liers! Prepare to die!!\""},
};
static cflags<action_s> indiferent_actions = {Lie, Bribe, Attack};
static cflags<action_s> friendly_actions = {Trade, Talk, Smithing};

static const chati* find(action_s id, encounteri& scene, const aref<chati>& source) {
	adat<const chati*> result;
	for(auto& e : source) {
		if(e.action != id)
			continue;
		if(!scene.match(e.conditions))
			continue;
		result.add(&e);
	}
	if(!result)
		return 0;
	return result.data[rand() % result.count];
}

static void prompt(encounteri& scene, const char* title, const cflags<action_s>& actions) {
	answers aw;
	for(auto i = Greeting; i <= FailLie; i = (action_s)(i + 1)) {
		if(!actions.is(i))
			continue;
		if(!game.apply(i, scene, false))
			continue;
		aw.add(i, bsdata<actioni>::elements[i].name);
	}
	auto i = (action_s)aw.choosebg(title, 0, 0, true);
	game.apply(i, scene, true);
}

static void prompt(encounteri& scene, action_s id, const aref<chati>& dialogs, const cflags<action_s>& actions) {
	auto p = find(id, scene, dialogs);
	if(!p)
		return;
	prompt(scene, p->text, actions);
}

static void chatting(encounteri& scene, const aref<chati>& dialogs) {
	draw::animation::update();
	if(scene.reaction == Indifferent) {
		prompt(scene, Greeting, dialogs, indiferent_actions);
		if(scene.reaction != Friendly)
			return;
	}
	prompt(scene, Greeting, dialogs, friendly_actions);
}

creature* encounteri::getleader() const {
	return getbest(Intellegence);
}

bool encounteri::match(const conditiona& source) const {
	auto leader = getleader();
	if(!leader)
		return false;
	for(auto& e : source) {
		if(!e)
			break;
		if(e.type == Reaction) {
			if(reaction != e.value)
				return false;
		} else if(!leader->ismatch(e))
			return false;
	}
	return true;
}

void encounteri::dialog() {
	chatting(*this, messages);
}

void encounteri::set(reaction_s v) {
	reaction = v;
	creaturea::set(v);
}

void gamei::interract(indext monster_index) {
	encounteri encounter;
	encounter.select(monster_index);
	auto leader = encounter.getbest(Intellegence);
	if(!leader)
		return;
	auto party_index = game.getcamera();
	auto party_direction = game.getdirection();
	bool party_ambush = false;
	auto direction = pointto(monster_index, party_index);
	location.turnto(monster_index, direction);
	location.formation(monster_index, direction);
	location.turnto(party_index, to(direction, Down), &party_ambush);
	encounter.set(leader->getreaction());
	if(encounter.reaction == Indifferent)
		encounter.set(leader->rollreaction(0));
	if(encounter.reaction == Indifferent || encounter.reaction == Friendly) {
		party_ambush = false;
		encounter.dialog();
	}
	switch(encounter.reaction) {
	case Friendly:
		encounter.resolve();
		game.addexp(Good, 50);
		break;
	case Indifferent:
		encounter.leave();
		break;
	default:
		game.attack(monster_index, false, party_ambush ? PartyAmbush : NoAmbush);
		break;
	}
}

bool gamei::apply(action_s id, encounteri& scene, bool run) {
	auto interactive = true;
	auto leader = scene.getbest(Intellegence);
	if(!leader)
		return false;
	itema items;
	scene.next = Greeting;
	switch(id) {
	case Lie:
		if(leader->ismindless())
			return false;
		if(is(LawfulGood))
			return false;
		if(run) {
			if(roll(getaverage(Charisma))) {
				addexp(Evil, 20);
				scene.set(Friendly);
			} else {
				scene.set(Hostile);
				scene.next = FailLie;
			}
		}
		break;
	case Bribe:
		if(leader->ismindless())
			return false;
		items.select();
		items.forsale(false);
		if(!items)
			return false;
		if(run) {
			auto p = items.choose("Gift wich item?", false);
			if(interactive) {
				char temp[128]; stringbuilder sb(temp); p->getname(sb);
				mslog("You loose %1", temp);
			}
			p->clear();
			scene.set(Indifferent);
		}
		break;
	case Trade:
		if(leader->ismindless())
			return false;
		break;
	case Talk:
		if(leader->ismindless())
			return false;
		if(run) {

		}
		break;
	case Attack:
		if(run) {
			scene.set(Hostile);
			addexp(Evil, 30);
		}
		break;
	default:
		return false;
	}
	return true;
}