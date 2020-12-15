#include "view.h"

using namespace draw;

static int			current_level = 1;
static creature*	current_hero = 0;

static unsigned get_hero_spells_flags(creature* pe, class_s type, creature* pse) {
	unsigned flags = 0;
	if(!pe->iscast(type))
		flags |= Disabled;
	if(pe == pse)
		flags |= Checked;
	return flags;
}

static void spell_avatar(int x, int y, int i, class_s type, creature* pc) {
	auto p = party[i].getcreature();
	if(!p)
		return;
	draw::avatar(x, y, p, get_hero_spells_flags(p, type, pc), 0);
}

static void spells_portraits(int x, int y, class_s type, creature* pc) {
	spell_avatar(x, y, 0, type, pc);
	spell_avatar(x + 72, y, 1, type, pc);
	spell_avatar(x, y + 52, 2, type, pc);
	spell_avatar(x + 72, y + 52, 3, type, pc);
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
	if(!current_hero)
		return;
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

static unsigned select_spells(spell_s* result, spell_s* result_maximum, const creature* pc, class_s type, int level) {
	auto p = result;
	for(auto i = spell_s(1); i <= LastSpellAbility; i = (spell_s)(i + 1)) {
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
	for(auto i = spell_s(1); i < LayOnHands; i = (spell_s)(i + 1)) {
		if(creature::getlevel(i, type) != level)
			continue;
		if(!pc->isknown(i))
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

creature* creature::choosehero() {
	answers elements;
	char temp[260];
	for(auto v : party) {
		auto p = v.getcreature();
		if(!p)
			continue;
		elements.add((int)p, p->getname(temp, zendof(temp)));
	}
	return (creature*)elements.choosesm("On which hero?", true);
}

int variantc::chooselv(class_s type) const {
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	unsigned current_element = 0;
	while(ismodal()) {
		variantc result;
		result = *this;
		result.matchsl(type, current_level);
		result.sort();
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
			flatb(x + i * dx, y, dx, (level == current_level) ? Focused : 0, temp);
		}
		x = rc.x1;
		y = rc.y1 + draw::texth() + 2;
		for(unsigned i = 0; i < result.count; i++) {
			unsigned flags = (i == current_element) ? Focused : 0;
			y += labelb(x, y, rc.width(), flags, result.data[i].getname());
		}
		domodal();
		switch(hot::key) {
		case KeyEscape:
			return Moved;
		case KeyEnter:
		case Alpha + 'U':
			breakmodal(result.data[current_element].value);
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
	return getresult();
}

spell_s creature::choosespell(class_s type) const {
	variantc spells;
	spells.cspells(this, true);
	return (spell_s)spells.chooselv(type);
}

static bool choose_creature(class_s type, creature** hero) {
	creature* p;
	switch(hot::key) {
	case Alpha + '1':
	case Alpha + '2':
	case Alpha + '3':
	case Alpha + '4':
		if(true) {
			auto id = hot::key - (Alpha + '1');
			p = party[id].getcreature();
			if(!p || !p->iscast(type))
				break;
			*hero = p;
		}
		break;
	default:
		return false;
	}
	return true;
}

void creature::preparespells(class_s type) {
	adat<spell_s, 32> result;
	auto hero = game.getvalid(0, type);
	openform();
	while(ismodal()) {
		result.count = 0;
		auto maximum_spells = 0;
		auto prepared_spells = 0;
		if((int)getfocus() >= 1 && (int)getfocus() <= 9)
			current_level = (int)getfocus();
		if(hero) {
			result.count = select_known_spells(result.data, zendof(result.data), hero, type, current_level);
			maximum_spells = hero->getspellsperlevel(type, current_level);
			prepared_spells = get_spells_prepared(hero, {result.data, result.count});
		}
		draw::animation::render(0, false);
		render_spell_window(result, hero, type, maximum_spells, prepared_spells);
		spells_portraits(184, 2, type, hero);
		domodal();
		auto current_index = result.indexof((spell_s*)getfocus());
		switch(hot::key) {
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
		default:
			choose_creature(type, &hero);
			break;
		}
		navigate(true);
	}
	closeform();
}

static void scribe_scroll() {
	auto pi = (item*)hot::param;
	current_hero->scribe(*pi);
}

void creature::scribe(item& it) {
	char temp[260];
	auto sv = it.getpower();
	if(!sv || sv.type!=Spell) {
		say("This is not magic scroll");
		return;
	}
	auto sp = (spell_s)sv.value;
	if(roll(LearnSpell)) {
		setknown(sp);
		mslog("%1 learn %2 spell", getname(temp, zendof(temp)), getstr(sp));
		addexp(100);
	} else
		mslog("%1 don't learn %2 spell", getname(temp, zendof(temp)), getstr(sp));
	it.clear();
}

void creature::scriblescrolls() {
	adat<item*, 32> source;
	const auto caster_type = Mage;
	current_hero = game.getvalid(0, caster_type);
	openform();
	while(ismodal()) {
		source.count = 0;
		if(current_hero) {
			for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i+1)) {
				auto& it = current_hero->wears[i];
				if(!it || !it.isidentified())
					continue;
				auto sv = it.getpower();
				if(!sv || sv.type != Spell)
					continue;
				auto sp = (spell_s)sv.value;
				if(!bsdata<spelli>::elements[sp].levels[0])
					continue;
				if(current_hero->isknown(sp))
					continue;
				source.add(&it);
			}
		}
		draw::animation::render(0, false);
		if(true) {
			draw::state push;
			setbigfont();
			form({0, 0, 22 * 8 + 2, 174}, 2);
			fore = colors::title;
			textb(6, 6, "Scrolls available:");
			auto x = 6, y = 18;
			fore = colors::white;
			int count = imin(source.count, (unsigned)13);
			for(int i = 0; i < count; i++) {
				auto sv = source.data[i]->getpower();
				auto sp = (spell_s)sv.value;
				y += buttont(x, y, 168, cmd(scribe_scroll, (int)source.data[i], (int)source.data[i]),
					getstr(sp));
			}
			draw::button(x, 156, -1, buttoncancel, "Close");
		}
		spells_portraits(184, 2, caster_type, current_hero);
		domodal();
		navigate(true);
		choose_creature(caster_type, &current_hero);
	}
	closeform();
}