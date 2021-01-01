#include "main.h"

static chati messages[] = {{Greeting, {Indifferent, Ave}, "\"Who is you? How you get there?\""},
{Greeting, {Indifferent, Low}, "\"Who is there? How you dig so deep?\""},
{Greeting, {Indifferent, Low}, "\"Whatta you doing here? And a who is you?\""},
{Greeting, {Indifferent, Low}, "\"What a ... Who is you?\""},
{Greeting, {Indifferent}, "%1 is glazing at you. But not attack, carefully watching."},
{Greeting, {Friendly, High}, "\"Welcome, my very best friends! I so glad to see you! What can I do for you?\""},
{Greeting, {Friendly, Ave}, "\"Welcome friends, I am at your service! What can I do for your?\""},
{Greeting, {Friendly, Low}, "\"Welcome strangers, me and my friends like you. Whata me can do for you?\""},
{Greeting, {Friendly}, "%1 is glazing at you. But not attack, carefully watching."},
{FailLie, {}, "\"You are liers! Prepare to die!!\""},
{TalkArtifact, {}, "\"I heard, that famous %1 lies in %2 part of this place. Find it as fast as possible.\""},
{TalkMagic, {}, "\"Wait a minute, one item from your equipment seems familiar to me. It's a %1 and it's magical.\""},
{TalkCursed, {}, "\"Oh, no. You have one cursed item %1. Be careful with it.\""},
{TalkHistory, {}, "\"This place not always be like this. Long time ago it be absolutely different and has it own secrets.\""},
{TalkRumor, {}, "\"Our fate is doomed. Keep you mind open.\""},
{TalkRumor, {}, "\"In a few days an event will come that will change the whole world.\""},
{TalkRumor, {}, "\"Dark side is strong.\""},
};
static cflags<action_s> indiferent_actions = {Lie, Bribe, Attack};
static cflags<action_s> friendly_actions = {Trade, Talk, Repair, Pet};
static item_s common_trade[] = {Ration, KeySilver, BluePotion};

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

static void prompt(const char* title) {
	answers aw;
	aw.add(1, "Next");
	aw.choosebg(title, 0, 0, true);
}

static void prompt(encounteri& scene, const char* title, const cflags<action_s>& actions) {
	answers aw;
	for(auto i = Greeting; i <= FailLie; i = (action_s)(i + 1)) {
		if(!actions.is(i))
			continue;
		if(!scene.apply(i, false))
			continue;
		aw.add(i, bsdata<actioni>::elements[i].name);
	}
	auto i = (action_s)aw.choosebg(title, 0, 0, true);
	scene.apply(i, true);
}

static void prompt(encounteri& scene, action_s id, const aref<chati>& dialogs, const cflags<action_s>& actions) {
	auto leader = scene.getleader();
	if(!leader)
		return;
	auto p = find(id, scene, dialogs);
	if(!p)
		return;
	char temp[512]; stringbuilder sb(temp);
	sb.add(p->text, leader->getname());
	prompt(scene, temp, actions);
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

static void trade_items(int cost, item_s goods[3], rarity_s rarity = Uncommon) {
	for(int i = 3; cost > 0; i--) {
		if(i == 0)
			i = 3;
		if(i > cost)
			continue;
		item it(goods[i - 1]);
		it.setpower(rarity);
		game.additem(it, true);
		cost -= i;
	}
}

static bool talk_subject(action_s id, encounteri& scene, bool run) {
	companyi::adventurei* pa;
	itema items;
	char subject_temp[260];
	const char* dungeon_part = "unknown";
	const char* subject_name = "subject";
	auto p = find(id, scene, messages);
	if(!p)
		return false;
	const char* speech_text = p->text;
	switch(id) {
	case TalkArtifact:
		if(!location.stat.wands)
			return false;
		if(!location.islying(location.stat.wands.index, location.head.wands))
			return false;
		if(run) {
			subject_name = bsdata<itemi>::elements[location.head.wands].name;
			dungeon_part = location.getnavigation(location.stat.wands.index);
		}
		break;
	case TalkMagic:
		items.select();
		items.identified(false);
		items.cursed(false);
		items.magical(true);
		if(!items)
			return false;
		if(run) {
			auto pi = items.random();
			pi->setidentified(1);
			stringbuilder sb(subject_temp); pi->getname(sb);
			subject_name = subject_temp;
		}
		break;
	case TalkCursed:
		items.select();
		items.identified(false);
		items.cursed(true);
		if(!items)
			return false;
		if(run) {
			auto pi = items.random();
			pi->setidentified(1);
			stringbuilder sb(subject_temp); pi->getname(sb);
			subject_name = subject_temp;
		}
		break;
	case TalkHistory:
		pa = game.getadventure();
		if(!pa)
			return false;
		if(pa->history_progress >= pa->gethistorymax())
			return false;
		if(run) {
			auto current_history = pa->history_progress;
			speech_text = pa->history[current_history];
			pa->history_progress = current_history + 1;
			game.addexpc(500, 0);
		}
		break;
	case TalkRumor:
		break;
	default:
		return false;
	}
	if(run) {
		char temp[512]; stringbuilder sb(temp);
		sb.add(speech_text, subject_name, dungeon_part);
		prompt(temp);
	}
	return true;
}

static bool talk_subject(encounteri& scene, bool run) {
	adat<action_s> subjects;
	for(auto& e : bsdata<actioni>()) {
		if(!e.talk)
			continue;
		auto id = (action_s)(&e - bsdata<actioni>::elements);
		if(!talk_subject(id, scene, false))
			continue;
		subjects.add(id);
	}
	if(!run)
		return subjects.getcount() != 0;
	return talk_subject(subjects.data[rand() % subjects.getcount()], scene, run);
}

bool encounteri::apply(action_s id, bool run) {
	auto interactive = true;
	auto leader = getbest(Intellegence);
	if(!leader)
		return false;
	itema items;
	next = Greeting;
	switch(id) {
	case Lie:
		if(leader->ismindless())
			return false;
		if(game.is(LawfulGood))
			return false;
		if(run) {
			if(game.roll(game.getaverage(Charisma))) {
				game.addexp(Evil, 20);
				set(Friendly);
			} else {
				set(Hostile);
				next = FailLie;
			}
		}
		break;
	case Bribe:
		if(leader->ismindless())
			return false;
		items.select();
		items.forsale(true);
		if(!items)
			return false;
		if(run) {
			auto pi = items.choose("Gift which item?", false);
			if(!pi)
				return false;
			if(interactive) {
				char temp[128]; stringbuilder sb(temp); pi->getname(sb);
				mslog("You loose %1", temp);
			}
			pi->clear();
			set(Indifferent);
		}
		break;
	case Trade:
		if(leader->ismindless())
			return false;
		items.select();
		items.forsale(true);
		if(!items)
			return false;
		if(run) {
			auto pi = items.choose("Sell which item?", false);
			if(!pi)
				return false;
			auto cost = pi->getcost();
			trade_items(cost, common_trade);
			pi->clear();
		}
		break;
	case Talk:
		if(leader->ismindless())
			return false;
		return talk_subject(*this, run);
	case Attack:
		if(run) {
			set(Hostile);
			game.addexp(Evil, 30);
		}
		break;
	case Pet:
		if(!leader->ismindless())
			return false;
		if(run) {
		}
		break;
	case Repair:
		if(leader->ismindless())
			return false;
		items.select();
		items.broken(true);
		if(!items)
			return false;
		if(run) {

		}
		break;
	default:
		return false;
	}
	return true;
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
	//encounter.set(Friendly);
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