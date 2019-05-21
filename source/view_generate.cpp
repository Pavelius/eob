#include "view.h"

static int button(int x, int y, int id, int key, const char* name) {
	draw::state push;
	draw::setsmallfont();
	static int pressed_key;
	if(hot::key == InputKeyUp)
		pressed_key = 0;
	else if(hot::key == key) {
		pressed_key = key;
		if(key && hot::key == key)
			draw::execute(id);
	}
	auto pi = draw::gres(CHARGENB);
	auto si = 0;
	switch(id) {
	case NextPortrait:
	case PreviousPortrait:
		si = 2;
		break;
	case NewGame:
		si = 4;
		break;
	case Delete:
		si = 6;
		break;
	}
	if(key && pressed_key == key)
		si++;
	auto width = pi->get(si).sx;
	auto height = pi->get(si).sy;
	draw::image(x, y, pi, si, 0);
	switch(id) {
	case PreviousPortrait:
		draw::image(x + 7, y + 5, pi, 8, 0);
		break;
	case NextPortrait:
		draw::image(x + 7, y + 5, pi, 9, 0);
		break;
	}
	if(name)
		draw::text(x + 1 + (width - draw::textw(name)) / 2, y + 1 + (height - draw::texth()) / 2, name);
	return height;
}

static void genavatar(int x, int y, int id, unsigned state, creature* pc) {
	if(pc) {
		if(*pc) {
			draw::state push;
			draw::fore = colors::white;
			draw::setsmallfont();
			draw::portrait(x + 1, y, pc);
			char temp[260];	pc->getname(temp, zendof(temp));
			draw::text(x - 14 + (58 - draw::textw(temp)) / 2, y + 43, temp);
		} else
			draw::image(x, y, draw::gres(XSPL), (clock() / 150) % 10, 0);
	}
	rect rc = {x, y, x + 32, y + 32};
	if(state&Focused)
		draw::rectb(rc, colors::white.mix(colors::black, draw::ciclic(200, 7)));
}

static void genheader(unsigned state, int focus) {
	draw::background(CHARGEN);
	for(int i = 0; i < 4; i++) {
		auto id = i + 1;
		genavatar(
			16 + (i % 2) * 64,
			64 + (i / 2) * 64,
			id, draw::getfstate(id, focus),
			game::party[i]);
	}
}

static void portraits(int x, int y, int& n, int cur, int count, int max_avatars, int* port) {
	auto ps = draw::gres(PORTM);
	if(cur < n)
		n = cur;
	else if(cur >= n + count)
		n = cur - count + 1;
	if(n < 0)
		n = 0;
	if(n > max_avatars - count)
		n = max_avatars - count;
	for(int i = 0; i < count; i++) {
		int k = port[i + n];
		if(!k)
			break;
		draw::image(x + i * 32, y, ps, k, 0);
		if((i + n) == cur)
			draw::rectb({x + i * 32 - 1, y, x + i * 32 + 31, y + 31},
				colors::white.mix(colors::black, draw::ciclic(200, 8)));
	}
}

static void draw_ability(creature* pc) {
	char temp[32];
	int x = 148;
	int y = 128;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1)) {
		draw::textb(x, y, getstr(i));
		sznum(temp, pc->get(i));
		draw::textb(x + 32, y, temp);
		y += draw::texth() + 1;
	}
}

static void view_ability(creature* pc, class_s type, race_s race, gender_s gender) {
	int port[64];
	auto max_avatars = game::getavatar(port, zendof(port), race, gender, type);
	const int width = 152;
	char temp[64];
	int values[8], x, y;
	draw::state push;
	draw::setbigfont();
	draw::fore = colors::white;
	hot::clear();
	int cur_portrait = 0;
	int org_portrait = 0;
	int focus = 0;
	while(true) {
		genheader(NoFocusing, focus);
		x = 143; y = 66;
		portraits(x + 33, y, org_portrait, cur_portrait, 4, max_avatars, port);
		y += button(x, y, PreviousPortrait, KeyLeft, 0);
		y += button(x, y, NextPortrait, KeyRight, 0);
		x = 148; y = 104;
		zprint(temp, "%1 %2", getstr(race), getstr(gender));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 2;
		zprint(temp, getstr(type));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 2;
		draw_ability(pc);
		y = 168; x = 223;
		button(x, y, Roll, Alpha + 'R', "Roll");
		button(x + 39, y, Keep, KeyEnter, "Keep");
		int id = draw::input();
		switch(id) {
		case Roll:
			game::getability(values, type, race);
			for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
				pc->set(i, values[i]);
			break;
		case Keep:
			if(port[cur_portrait])
				pc->setavatar(port[cur_portrait]);
			return;
		case PreviousPortrait:
			cur_portrait--;
			if(cur_portrait < 0)
				cur_portrait = 0;
			break;
		case NextPortrait:
			cur_portrait++;
			if(cur_portrait > max_avatars)
				cur_portrait = max_avatars - 1;
			break;
		}
	}
}

struct chelement {
	int			id;
	const char*	text;
};

static int choose(const char* title_string, aref<chelement> elements) {
	draw::state push;
	draw::setbigfont();
	draw::fore = colors::white;
	hot::clear();
	if(!elements)
		return 0;
	unsigned n = 0;
	while(true) {
		genheader(NoFocusing, 0);
		int x = 148;
		int y = 68;
		if(title_string)
			y += draw::header(148, 68, title_string);
		auto i = 0;
		for(auto& e : elements) {
			y += draw::linetext(x, y, 128,
				e.id,
				(n == i) ? Focused : 0,
				e.text);
			i++;
		}
		int id = draw::input();
		switch(id) {
		case KeyDown:
			if(n < elements.count - 1)
				n++;
			else
				n = 0;
			break;
		case KeyUp:
			if(n)
				n--;
			else
				n = elements.count - 1;
			break;
		case KeyEnter:
			return elements.data[n].id;
		}
	}
}

static gender_s choosegender(bool interactive) {
	if(interactive) {
		adat<chelement, 32> source;
		for(auto i = Male; i <= Female; i = (gender_s)(i + 1)) {
			source.add({i, getstr(i)});
		}
		return (gender_s)choose("Select Gender:", source);
	} else {
		// RULE: Male are most common as adventurers
		if(d100() < 65)
			return Male;
		else
			return Female;
	}
}

static alignment_s choosealignment(bool interactive, class_s depend) {
	adat<chelement, 32> source;
	for(auto i = FirstAlignment; i <= LastAlignment; i = (alignment_s)(i + 1)) {
		if(!creature::isallow(i, depend))
			continue;
		source.add({i, getstr(i)});
	}
	if(interactive)
		return (alignment_s)choose("Select Alignment:", source);
	return (alignment_s)source.data[rand() % source.count].id;
}

static race_s chooserace(bool interactive) {
	if(interactive) {
		adat<chelement, 32> source;
		for(auto i = Dwarf; i <= Human; i = (race_s)(i + 1))
			source.add({i, getstr(i)});
		return (race_s)choose("Select Race:", source);
	}
	else {
		// RULE: Humans most common in the worlds.
		if(d100() < 50)
			return Human;
		else
			return (race_s)xrand(Dwarf, Halfling);
	}
}

static class_s chooseclass(bool interactive, race_s race) {
	adat<chelement, 32> source;
	for(auto i = Cleric; i <= MageTheif; i = (class_s)(i + 1)) {
		if(!creature::isallow(i, race))
			continue;
		source.add({i, getstr(i)});
	}
	if(interactive)
		return (class_s)choose("Select Class:", source);
	return (class_s)source.data[rand() % source.count].id;
}

static bool is_party_created() {
	for(int i = 0; i < 4; i++) {
		if(!game::party[i])
			return false;
		if(!(*game::party[i]))
			return false;
	}
	return true;
}

static void change(creature* pc) {
	char temp[260];
	draw::state push;
	draw::setbigfont();
	draw::fore = colors::white;
	hot::clear();
	auto race = pc->getrace();
	auto gender = pc->getgender();
	auto type = pc->getclass();
	auto focus = 0;
	int x, y;
	const int width = 152;
	creature** prec;
	while(true) {
		genheader(NoFocusing, focus);
		draw::portrait(205, 66, pc);
		x = 148; y = 98;
		pc->getname(temp, zendof(temp));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		zprint(temp, "%1 %2", getstr(race), getstr(gender));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		zprint(temp, getstr(type));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		draw_ability(pc);
		y = 168; x = 223;
		button(x, y, Delete, Alpha + 'D', 0);
		button(x + 39, y, OK, KeyEnter, "OK");
		button(x + 39, y - 16, Rename, Alpha + 'N', "Name");
		int id = draw::input();
		switch(id) {
		case Delete:
			prec = (creature**)zchr(game::party, pc);
			if(prec)
				*prec = 0;
			pc->clear();
			return;
		case KeyEscape:
		case OK:
			return;
		case Rename:
			pc->setname();
			break;
		}
	}
}

void draw::generation() {
	draw::state push;
	draw::fore = colors::white;
	draw::setbigfont();
	auto focus = 1;
	while(true) {
		genheader(0, focus);
		auto nid = (focus >= 1 && focus <= 4) ? focus - 1 : -1;
		rect rc = {150, 74, 296, 184};
		char temp[1024];
		zprint(temp, "Select the box of the character you wish to create or view.");
		if(is_party_created()) {
			zcat(temp, "\n\n");
			zprint(temp, "Your party is complete. Select PLAY button or press 'P' to start the game.");
			button(25, 181, NewGame, Alpha + 'P', 0);
		}
		draw::textb(rc, temp);
		auto id = draw::input();
		switch(id) {
		case KeyLeft:
		case KeyRight:
		case KeyUp:
		case KeyDown:
			if(focus >= 1 && focus <= 4) {
				int inc = -1;
				if(id == KeyRight)
					inc = 1;
				else if(id == KeyUp)
					inc = -2;
				else if(id == KeyDown)
					inc = 2;
				focus += inc;
				if(focus <= 0)
					focus += 4;
				else if(focus > 4)
					focus -= 4;
			}
			break;
		case KeyEnter:
			if(game::party[nid])
				change(game::party[nid]);
			else {
				game::party[nid] = creature::newhero();
				game::party[nid]->create(NoGender, NoRace, NoClass, LawfulGood, true);
			}
			break;
		case NewGame:
			return;
		}
	}
}

void creature::create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive) {
	int temp[6];
	if(!gender)
		gender = choosegender(interactive);
	if(!race)
		race = chooserace(interactive);
	if(!type)
		type = chooseclass(interactive, race);
	if(!alignment)
		alignment = choosealignment(interactive, type);
	// Basic
	set(gender);
	set(race);
	set(type);
	set(alignment);
	// Abilities
	game::getability(temp, type, race);
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		set(i, temp[i]);
	// Portrait
	setavatar(game::getavatar(race, gender, getclass(type, 0)));
	if(interactive)
		view_ability(this, type, race, gender);
	finish();
	sethits(gethitsmaximum());
}