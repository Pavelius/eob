#include "view.h"

using namespace draw;

static int			current_portrait;
static creature*	current_player;

static void prev_portrait() {
	current_portrait--;
}

static void next_portrait() {
	current_portrait++;
}

static void roll_character() {
	current_player->roll_ability();
	current_player->finish();
}

static void new_game() {
	breakmodal(1);
}

static void delete_character() {
	auto prec = (creature**)zchr(game::party, current_player);
	if(prec)
		*prec = 0;
	current_player->clear();
	breakmodal(0);
}

static int button(int x, int y, const cmd& ev, const char* name, int key) {
	static int pressed_key;
	draw::state push;
	draw::setsmallfont();
	if(hot::key == InputKeyUp)
		pressed_key = 0;
	else if(hot::key == key) {
		pressed_key = key;
		if(key && hot::key == key)
			ev.execute();
	}
	auto pi = draw::gres(CHARGENB);
	auto si = 0;
	if(ev.proc == next_portrait || ev.proc == prev_portrait)
		si = 2;
	else if(ev.proc == new_game)
		si = 4;
	else if(ev.proc == delete_character)
		si = 6;
	if(key && pressed_key == key)
		si++;
	auto width = pi->get(si).sx;
	auto height = pi->get(si).sy;
	draw::image(x, y, pi, si, 0);
	if(ev.proc == prev_portrait)
		draw::image(x + 7, y + 5, pi, 8, 0);
	else if(ev.proc == next_portrait)
		draw::image(x + 7, y + 5, pi, 9, 0);
	if(name)
		draw::text(x + 1 + (width - draw::textw(name)) / 2, y + 1 + (height - draw::texth()) / 2, name);
	return height;
}

static void genavatar(int x, int y, const cmd& ev) {
	auto pc = *((creature**)ev.param);
	if(pc) {
		if(current_player != pc) {
			draw::state push;
			fore = colors::white;
			setsmallfont();
			pc->view_portrait(x + 1, y);
			char temp[260];	pc->getname(temp, zendof(temp));
			text(x - 14 + (58 - draw::textw(temp)) / 2, y + 43, temp);
		} else
			image(x, y, draw::gres(XSPL), (clock() / 150) % 10, 0);
	}
	if(ev.proc) {
		rect rc = {x, y, x + 32, y + 32};
		focusing(rc, ev.focus);
		if(getfocus() == ev.focus) {
			draw::rectb(rc, colors::white.mix(colors::black, draw::ciclic(200, 7)));
			if(hot::key == KeyEnter)
				ev.execute();
		}
	}
}

static void genheader(callback proc = 0) {
	draw::background(CHARGEN);
	for(int i = 0; i < 4; i++) {
		genavatar(
			16 + (i % 2) * 64,
			64 + (i / 2) * 64,
			cmd(proc, (int)&game::party[i], (int)&game::party[i]));
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
		draw::image(x + i * 32, y, ps, k, 0);
		if((i + n) == cur)
			draw::rectb({x + i * 32 - 1, y, x + i * 32 + 31, y + 31},
				colors::white.mix(colors::black, draw::ciclic(200, 8)));
	}
}

static int number(int x, int y, int w, const char* title, const char* v, bool use_bold = true) {
	if(use_bold)
		draw::textb(x, y, title);
	else
		draw::text(x, y, title);
	if(use_bold)
		draw::textb(x + w, y, v);
	else
		draw::text(x + w, y, v);
	return draw::texth() + 1;
}

static int number(int x, int y, int w, const char* title, int v, bool use_bold) {
	char temp[32];
	sznum(temp, v);
	return number(x, y, w, title, temp, use_bold);
}

static int number(int x, int y, int w, const char* title, const dice& v, bool use_bold) {
	char temp[32];
	v.range(temp, zendof(temp));
	return number(x, y, w, title, temp, use_bold);
}

static int number(int x, int y, int w, const char* title, int v1, int v2, const char* format, bool use_bold) {
	char temp[32];
	szprint(temp, zendof(temp), format, v1, v2);
	return number(x, y, w, title, temp, use_bold);
}

int creature::render_ability(int x, int y, int width, bool use_bold) const {
	auto y0 = y;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1)) {
		auto v = get(i);
		if(i == Strenght && v==18 && str_exeptional>0) {
			if(str_exeptional == 100)
				y += number(x, y, width, getstr(i), 18, 0, "%1i/00", use_bold);
			else
				y += number(x, y, width, getstr(i), 18, str_exeptional, "%1i/%2i", use_bold);
		} else
			y += number(x, y, width, getstr(i), get(i), use_bold);
	}
	return y - y0;
}

int creature::render_combat(int x, int y, int width, bool use_bold) const {
	auto y0 = y;
	combati ai = {}; get(ai);
	y += number(x, y, width, "AC", 10 - getac(), use_bold);
	y += number(x, y, width, "ATT", 20 - ai.bonus, use_bold);
	y += number(x, y, width, "DAM", ai.damage, use_bold);
	y += number(x, y, width, "HP", gethits(), gethitsmaximum(), "%1i/%2i", use_bold);
	return y - y0;
}

void creature::view_ability() {
	adat<int, 64> source;
	source.count = game::getavatar(source.data, source.endof(), race, gender, type);
	const int width = 152;
	char temp[64];
	int x, y;
	draw::state push;
	draw::setbigfont();
	draw::fore = colors::white;
	hot::key = 0;
	int org_portrait = 0;
	current_portrait = source.indexof(avatar);
	finish();
	while(ismodal()) {
		if(current_portrait >= (int)source.count)
			current_portrait = source.count - 1;
		if(current_portrait < 0)
			current_portrait = 0;
		if(current_portrait < (int)source.count)
			avatar = source.data[current_portrait];
		x = 143; y = 66;
		genheader();
		portraits(x + 33, y, org_portrait, current_portrait, 4, source.count, source.data);
		y += button(x, y, prev_portrait, 0, KeyLeft);
		y += button(x, y, next_portrait, 0, KeyRight);
		x = 148; y = 104;
		zprint(temp, "%1 %2", getstr(race), getstr(gender));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 2;
		zprint(temp, getstr(type));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 2;
		render_ability(148, 128, 32, true);
		render_combat(224, 128, 32, true);
		y = 168; x = 223;
		button(x, y, roll_character, "Roll", Alpha + 'R');
		button(x + 39, y, buttonok, "Keep", Alpha + 'K');
		domodal();
	}
}

static gender_s choosegender(bool interactive) {
	if(interactive) {
		answers source;
		for(auto i = Male; i <= Female; i = (gender_s)(i + 1))
			source.add(i, getstr(i));
		return (gender_s)source.choose("Select Gender:");
	} else {
		// RULE: Male are most common as adventurers
		if(d100() < 65)
			return Male;
		else
			return Female;
	}
}

static alignment_s choosealignment(bool interactive, class_s depend) {
	answers source;
	for(auto i = FirstAlignment; i <= LastAlignment; i = (alignment_s)(i + 1)) {
		if(!creature::isallow(i, depend))
			continue;
		source.add(i, getstr(i));
	}
	//source.sort();
	return (alignment_s)source.choose("Select Alignment:", interactive);
}

static race_s chooserace(bool interactive) {
	if(interactive) {
		answers source;
		for(auto i = Dwarf; i <= Human; i = (race_s)(i + 1))
			source.add(i, getstr(i));
		source.sort();
		return (race_s)source.choose("Select Race:");
	} else {
		// RULE: Humans most common in the worlds.
		if(d100() < 50)
			return Human;
		else
			return (race_s)xrand(Dwarf, Halfling);
	}
}

static class_s chooseclass(bool interactive, race_s race) {
	answers source;
	for(auto i = Cleric; i <= MageTheif; i = (class_s)(i + 1)) {
		if(!creature::isallow(i, race))
			continue;
		source.add(i, getstr(i));
	}
	source.sort();
	return (class_s)source.choose("Select Class:", interactive);
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

static void apply_change_character() {
	if(!current_player)
		return;
	char temp[260];
	draw::state push;
	setbigfont();
	fore = colors::white;
	hot::key = 0;
	auto race = current_player->getrace();
	auto gender = current_player->getgender();
	auto type = current_player->getclass();
	auto focus = 0;
	int x, y;
	const int width = 152;
	while(ismodal()) {
		x = 148; y = 98;
		genheader();
		current_player->view_portrait(205, 66);
		current_player->getname(temp, zendof(temp));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		zprint(temp, "%1 %2", getstr(race), getstr(gender));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		zprint(temp, getstr(type));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		current_player->render_ability(148, 128, 32, true);
		current_player->render_combat(224, 128, 32, true);
		y = 168; x = 223;
		button(x, y, delete_character, 0, Alpha + 'D');
		button(x + 39, y, buttonok, "OK", Alpha + 'K');
		domodal();
	}
}

static void change_character() {
	auto ptr_player = (creature**)hot::param;
	if(ptr_player >= game::party
		&& ptr_player <= game::party + sizeof(game::party) / sizeof(game::party[0])) {
		if(*ptr_player) {
			current_player = *ptr_player;
			apply_change_character();
		} else {
			(*ptr_player) = bsdata<creature>::add();
			current_player = *ptr_player;
			(*ptr_player)->create(NoGender, NoRace, NoClass, LawfulGood, true);
		}
		current_player = 0;
	}
}

int answers::choose(const char* title_string) const {
	if(!elements)
		return 0;
	draw::state push;
	setbigfont();
	fore = colors::white;
	openform();
	while(ismodal()) {
		genheader();
		auto x = 148, y = 68;
		if(title_string)
			y += draw::header(148, 68, title_string);
		for(auto& e : elements)
			y += buttont(x, y, 128, cmd(buttonparam, e.id, (int)&e), e.text);
		domodal();
		navigate();
	}
	closeform();
	return getresult();
}

void creature::view_party() {
	draw::state push;
	fore = colors::white;
	setbigfont();
	openform();
	while(ismodal()) {
		genheader(change_character);
		rect rc = {150, 74, 296, 184};
		char temp[1024];
		zprint(temp, "Select the box of the character you wish to create or view.");
		if(is_party_created()) {
			zcat(temp, "\n\n");
			zprint(temp, "Your party is complete. Select PLAY button or press 'P' to start the game.");
			::button(25, 181, new_game, 0, Alpha + 'P');
		}
		textb(rc, temp);
		domodal();
		navigate();
	}
	closeform();
}

void creature::create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive) {
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
	roll_ability();
	setavatar(game::getavatar(race, gender, getclass(type, 0)));
	if(interactive)
		view_ability();
	else
		finish();
	random_equipment();
	random_name();
}