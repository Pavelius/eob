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
	current_player->random_ability();
	current_player->finish();
}

static void new_game() {
	breakmodal(1);
}

static void delete_character() {
	current_player->clear();
	breakmodal(0);
}

static int buttonx(int x, int y, const char* name, int key, callback proc, int param) {
	static int pressed_key;
	draw::state push;
	draw::setsmallfont();
	if(hot::key == InputKeyUp)
		pressed_key = 0;
	else if(hot::key == key) {
		pressed_key = key;
		if(key && hot::key == key)
			draw::execute(proc, param);
	}
	auto pi = draw::gres(CHARGENB);
	auto si = 0;
	if(proc == next_portrait || proc == prev_portrait)
		si = 2;
	else if(proc == new_game)
		si = 4;
	else if(proc == delete_character)
		si = 6;
	if(key && pressed_key == key)
		si++;
	auto width = pi->get(si).sx;
	auto height = pi->get(si).sy;
	draw::image(x, y, pi, si, 0);
	if(proc == prev_portrait)
		draw::image(x + 7, y + 5, pi, 8, 0);
	else if(proc == next_portrait)
		draw::image(x + 7, y + 5, pi, 9, 0);
	if(name)
		draw::text(x + 1 + (width - draw::textw(name)) / 2, y + 1 + (height - draw::texth()) / 2, name);
	return height;
}

static void genavatar(int x, int y, creature* pc, callback proc) {
	if(current_player == pc)
		image(x, y, draw::gres(XSPL), (clock() / 150) % 10, 0);
	else if(*pc) {
		draw::state push;
		fore = colors::white;
		setsmallfont();
		pc->view_portrait(x + 1, y);
		auto pn = pc->getname();
		text(x - 14 + (58 - draw::textw(pn)) / 2, y + 43, pn);
	}
	if(proc) {
		rect rc = {x, y, x + 32, y + 32};
		focusing(rc, pc);
		if(isfocus(pc)) {
			draw::rectb(rc, colors::white.mix(colors::black, draw::ciclic(200, 7)));
			if(hot::key == KeyEnter)
				execute(proc, (int)pc);
		}
	}
}

static void genheader(callback proc = 0) {
	draw::background(CHARGEN);
	for(int i = 0; i < 4; i++) {
		auto p = &bsdata<creature>::elements[i];
		genavatar(
			16 + (i % 2) * 64,
			64 + (i / 2) * 64,
			p, proc);
	}
}

static void portraits(int x, int y, int& n, int cur, int count, int max_avatars, short unsigned* port) {
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
		if(i == Strenght && v == 18 && str_exeptional > 0) {
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
	adat<short unsigned, 256> source;
	source.count = game.getavatar(source.data, race, gender, type);
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
			avatar = (char)source.data[current_portrait];
		x = 143; y = 66;
		genheader();
		portraits(x + 33, y, org_portrait, current_portrait, 4, source.count, source.data);
		y += buttonx(x, y, 0, KeyLeft, prev_portrait, 0);
		y += buttonx(x, y, 0, KeyRight, next_portrait, 0);
		x = 148; y = 104;
		zprint(temp, "%1 %2", getstr(race), getstr(gender));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 2;
		zprint(temp, getstr(type));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 2;
		render_ability(148, 128, 32, true);
		render_combat(224, 128, 32, true);
		y = 168; x = 223;
		buttonx(x, y, "Roll", 'R', roll_character, 0);
		buttonx(x + 39, y, "Keep", 'K', buttonok, 0);
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

alignment_s creature::choosealignment(bool interactive, class_s depend) {
	answers source;
	for(auto i = FirstAlignment; i <= LastAlignment; i = (alignment_s)(i + 1)) {
		if(!creature::isallow(i, depend))
			continue;
		source.add(i, getstr(i));
	}
	return (alignment_s)source.choose("Select Alignment:", interactive);
}

race_s creature::chooserace(bool interactive) {
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

class_s creature::chooseclass(bool interactive, race_s race) {
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
		auto& e = bsdata<creature>::elements[i];
		if(!e)
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
		auto pn = current_player->getname();
		draw::textb(x + (width - draw::textw(pn)) / 2, y, pn); y += draw::texth() + 1;
		zprint(temp, "%1 %2", getstr(race), getstr(gender));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		zprint(temp, getstr(type));
		draw::textb(x + (width - draw::textw(temp)) / 2, y, temp); y += draw::texth() + 1;
		current_player->render_ability(148, 128, 32, true);
		current_player->render_combat(224, 128, 32, true);
		y = 168; x = 223;
		buttonx(x, y, 0, 'D', delete_character, 0);
		buttonx(x + 39, y, "OK", 'K', buttonok, 0);
		domodal();
	}
}

static void change_character() {
	auto pc = (creature*)hot::param;
	if(!pc)
		return;
	current_player = pc;
	if(*pc)
		apply_change_character();
	else {
		auto gender = choosegender(true);
		auto race = creature::chooserace(true);
		auto type = creature::chooseclass(true, race);
		auto alignment = creature::choosealignment(true, type);
		current_player->create(gender, race, type, alignment, true);
	}
	current_player = 0;
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
	if(bsdata<creature>::source.getcount() < 4)
		bsdata<creature>::source.setcount(4);
	openform();
	while(ismodal()) {
		genheader(change_character);
		rect rc = {150, 74, 296, 184};
		char temp[1024];
		zprint(temp, "Select the box of the character you wish to create or view.");
		if(is_party_created()) {
			zcat(temp, "\n\n");
			zprint(temp, "Your party is complete. Select PLAY button or press 'P' to start the game.");
			buttonx(25, 181, 0, 'P', new_game, 0);
		}
		textb(rc, temp);
		domodal();
		navigate();
	}
	closeform();
	party.clear();
}

void creature::create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive) {
	set(gender);
	set(race);
	set(type);
	set(alignment);
	random_ability();
	setavatar(game.getavatar(race, gender, getclass(type, 0)));
	if(interactive)
		view_ability();
	else
		finish();
	random_name();
}