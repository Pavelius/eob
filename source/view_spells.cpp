#include "view.h"

using namespace draw;

static int current_level = 1;

static unsigned get_hero_spells_flags(creature* pe, class_s type, creature* pse) {
	unsigned flags = 0;
	if(!pe->get(type))
		flags |= Disabled;
	if(pe == pse)
		flags |= Checked;
	return flags;
}

static void spells_portraits(int x, int y, class_s type, creature* pc) {
	draw::avatar(x, y, game::party[0], get_hero_spells_flags(game::party[0], type, pc), 0);
	draw::avatar(x + 72, y, game::party[1], get_hero_spells_flags(game::party[1], type, pc), 0);
	draw::avatar(x, y + 52, game::party[2], get_hero_spells_flags(game::party[2], type, pc), 0);
	draw::avatar(x + 72, y + 52, game::party[3], get_hero_spells_flags(game::party[3], type, pc), 0);
}

static int get_spells_prepared(creature* pc, aref<spell_s> spells) {
	int result = 0;
	for(auto e : spells)
		result += pc->getprepare(e);
	return result;
}

static void choose_level() {
}

static void clear_spells() {
}

static void add_spell() {
}

static void render_spell_window(aref<spell_s> source, creature* pc, class_s type, int maximum_spells, int prepared_spells) {
	draw::state push;
	char temp[64];
	int level = pc ? pc->get(type) : 0;
	setbigfont();
	form({0, 0, 22 * 8 + 2, 174}, 2);
	fore = colors::title;
	textb(6, 6, "Spells available:");
	fore = colors::white;
	for(int i = 0; i < 9; i++)
		draw::button(4 + i * 19, 16, 17, cmd(choose_level, i+1, i+1), sznum(temp, i + 1));
	szprint(temp, zendof(temp), "%1i of %2i remaining", prepared_spells, maximum_spells);
	fore = colors::title;
	textb(6, 36, temp);
	fore = colors::white;
	int count = imin(source.count, (unsigned)13);
	for(int i = 0; i < count; i++)
		buttont(6, 46 + 8 * i, 168, cmd(add_spell, (int)(source.data + i), (int)(source.data + i)),
			getstr(source.data[i]), sznum(temp, pc->getprepare(source.data[i])));
	draw::button(6, 156, -1, buttoncancel, "Close");
	draw::button(60, 156, -1, clear_spells, "Clear");
}

static creature* get_valid_hero(creature* pc, class_s type) {
	auto i = zfind(game::party, pc);
	if(i == -1)
		i = 0;
	auto stop = i;
	while(true) {
		if(game::party[i] && game::party[i]->get(type))
			return game::party[i];
		if(++i >= (int)(sizeof(game::party) / sizeof(game::party[0])))
			i = 0;
		if(i == stop)
			return 0;
	}
}

static unsigned select_spells(spell_s* result, spell_s* result_maximum, const creature* pc, class_s type, int level) {
	auto p = result;
	for(auto i = NoSpell; i <= LastSpellAbility; i = (spell_s)(i + 1)) {
		if(creature::getlevel(i, type) != level)
			continue;
		int value = pc->get(i);
		while(value--) {
			if(result < result_maximum)
				*p++ = i;
		}
	}
	return p - result;
}

static unsigned select_known_spells(spell_s* result, spell_s* result_maximum, creature* pc, class_s type, int level) {
	auto p = result;
	for(auto i = NoSpell; i < LayOnHands; i = (spell_s)(i + 1)) {
		if(creature::getlevel(i, type) != level)
			continue;
		if(!pc->getknown(i))
			continue;
		if(result < result_maximum)
			*p++ = i;
	}
	return p - result;
}

static int labelb(int x, int y, int width, unsigned flags, const char* string) {
	draw::state push;
	auto height = draw::texth();
	rect rc = {x, y, x + width, y + height};
	if(flags&Focused)
		draw::rectf(rc, colors::blue.darken());
	draw::setclip(rc);
	draw::text(x + 1, y, string);
	return height;
}

creature* game::action::choosehero() {
	char temp[260];
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	adat<creature*, 7> elements;
	for(auto p : game::party) {
		if(p)
			elements.add(p);
	}
	int current_element = 0;
	while(ismodal()) {
		if(current_element >= (int)elements.count)
			current_element = elements.count - 1;
		if(current_element < 0)
			current_element = 0;
		screen.restore();
		rect rc = {70, 124, 178, 174};
		form(rc);
		if(true) {
			draw::state push;
			fore = colors::yellow;
			text(rc.x1 + 1, rc.y1 + 1, "On which hero?");
		}
		int x = rc.x1;
		int y = rc.y1 + 8;
		for(auto p : elements) {
			unsigned flags = (elements.indexof(p) == current_element) ? Focused : 0;
			y += labelb(x, y, rc.width(), flags,
				p->getname(temp, zendof(temp)));
		}
		domodal();
		switch(hot::key) {
		case KeyEscape:
		case Cancel:
			breakmodal(0);
			break;
		case KeyEnter:
		case Alpha + 'U':
			breakmodal((int)elements[current_element]);
			break;
		case KeyDown:
		case Alpha + 'Z':
			current_element++;
			break;
		case KeyUp:
		case Alpha + 'W':
			current_element--;
			break;
		case Alpha + '1':
		case Alpha + '2':
		case Alpha + '3':
		case Alpha + '4':
		case Alpha + '5':
		case Alpha + '6':
			if(true) {
				auto id = hot::key - (Alpha + '1');
				if(id < (int)elements.count)
					breakmodal((int)elements[id]);
			}
			break;
		}
	}
	return (creature*)getresult();
}

spell_s creature::choosespell(class_s type) const {
	adat<spell_s, 32> result;
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	unsigned current_element = 0;
	while(ismodal()) {
		result.count = select_spells(result.data, zendof(result.data), this, type, current_level);
		if(current_element >= result.count)
			current_element = result.count - 1;
		if(current_element < 0)
			current_element = 0;
		screen.restore();
		rect rc = {70, 124, 178, 174};
		form(rc);
		int x = rc.x1;
		int y = rc.y1;
		for(int i = 0; i < 9; i++) {
			const int dx = 12;
			char temp[16];
			auto level = i + 1;
			sznum(temp, level);
			unsigned flags = 0;
			flatb(x + i * dx, y, dx, ChooseLevels,
				(level == current_level) ? Focused : 0, temp);
		}
		x = rc.x1;
		y = rc.y1 + draw::texth() + 2;
		for(unsigned i = 0; i < result.count; i++) {
			unsigned flags = (i == current_element) ? Focused : 0;
			y += labelb(x, y, rc.width(), flags, getstr(result.data[i]));
		}
		domodal();
		switch(hot::key) {
		case KeyEscape:
		case Cancel:
			return NoSpell;
		case KeyEnter:
		case Alpha + 'U':
			breakmodal(result.data[current_element]);
			break;
		case KeyLeft:
		case Alpha + 'A':
			current_level--;
			if(current_level < 1)
				current_level = 1;
			break;
		case KeyRight:
		case Alpha + 'S':
			current_level++;
			if(current_level > 9)
				current_level = 9;
			break;
		case KeyDown:
		case Alpha + 'Z':
			current_element++;
			break;
		case KeyUp:
		case Alpha + 'W':
			current_element--;
			break;
		case Alpha + '1':
		case Alpha + '2':
		case Alpha + '3':
		case Alpha + '4':
		case Alpha + '5':
		case Alpha + '6':
		case Alpha + '7':
		case Alpha + '8':
		case Alpha + '9':
			current_level = hot::key - (Alpha + '1') + 1;
			break;
		}
	}
	return (spell_s)getresult();
}

void game::action::preparespells(class_s type) {
	adat<spell_s, 32> result;
	auto hero = get_valid_hero(0, type);
	auto old_focus = getfocus();
	openform();
	while(ismodal()) {
		result.count = 0;
		auto maximum_spells = 0;
		auto prepared_spells = 0;
		if(getfocus() >= 1 && getfocus() <= 9)
			current_level = getfocus();
		if(hero) {
			result.count = select_known_spells(result.data, zendof(result.data), hero, type, current_level);
			maximum_spells = hero->getspellsperlevel(type, current_level);
			prepared_spells = get_spells_prepared(hero, {result.data, result.count});
		}
		draw::background(PLAYFLD);
		render_spell_window(result, hero, type, maximum_spells, prepared_spells);
		spells_portraits(184, 2, type, hero);
		domodal();
		auto current_index = result.indexof((spell_s*)getfocus());
		switch(hot::key) {
		case Alpha + '1':
		case Alpha + '2':
		case Alpha + '3':
		case Alpha + '4':
			if(true) {
				auto id = hot::key - (Alpha + '1');
				if(!game::party[id] || !game::party[id]->get(type))
					break;
				hero = game::party[id];
			}
			break;
		case Alpha + 'C':
			for(auto e : result)
				hero->set(e, 0);
			break;
		case KeyRight:
			if(current_index != -1) {
				if(prepared_spells < maximum_spells)
					hero->setprepare(result.data[current_index],
					hero->getprepare(result.data[current_index]) + 1);
				continue;
			}
			break;
		case KeyLeft:
			if(current_index != -1) {
				auto c = hero->getprepare(result.data[current_index]);
				if(c)
					hero->setprepare(result.data[current_index], c - 1);
				continue;
			}
			break;
		}
		navigate(true);
	}
	closeform();
	setfocus(old_focus, true);
}