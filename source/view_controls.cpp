#include "view.h"

using namespace draw;

static struct resource_info {
	const char*			name;
	const char*			path;
	sprite*				data;
} objects[] = {{"NONE"},
{"BORDER", "art/interface"},
{"SCENES", "art/interface"},
{"CHARGEN", "art/interface"},
{"CHARGENB", "art/interface"},
{"COMPASS", "art/interface"},
{"INVENT", "art/interface"},
{"ITEMS", "art/misc"},
{"ITEMGS", "art/misc"},
{"ITEMGL", "art/misc"},
//
{"AZURE", "art/dungeons"},
{"BLUE", "art/dungeons"},
{"BRICK", "art/dungeons"},
{"CRIMSON", "art/dungeons"},
{"DROW", "art/dungeons"},
{"DUNG", "art/dungeons"},
{"GREEN", "art/dungeons"},
{"FOREST", "art/dungeons"},
{"MEZZ", "art/dungeons"},
{"SILVER", "art/dungeons"},
{"XANATHA", "art/dungeons"},
//
{"MENU", "art/interface"},
{"PLAYFLD", "art/interface"},
{"PORTM", "art/misc"},
{"THROWN", "art/misc"},
{"XSPL", "art/interface"},
//
{"ANKHEG", "art/monsters"},
{"ANT", "art/monsters"},
{"BLDRAGON", "art/monsters"},
{"BUGBEAR", "art/monsters"},
{"CLERIC1", "art/monsters"},
{"CLERIC2", "art/monsters"},
{"CLERIC3", "art/monsters"},
{"DRAGON", "art/monsters"},
{"DWARF", "art/monsters"},
{"FLIND", "art/monsters"},
{"GHOUL", "art/monsters"},
{"GOBLIN", "art/monsters"},
{"GUARD1", "art/monsters"},
{"GUARD2", "art/monsters"},
{"KOBOLD", "art/monsters"},
{"KUOTOA", "art/monsters"},
{"LEECH", "art/monsters"},
{"ORC", "art/monsters"},
{"SHADOW", "art/monsters"},
{"SKELETON", "art/monsters"},
{"SKELWAR", "art/monsters"},
{"SPIDER1", "art/monsters"},
{"WIGHT", "art/monsters"},
{"WOLF", "art/monsters"},
{"ZOMBIE", "art/monsters"},
};
static_assert((sizeof(objects) / sizeof(objects[0])) == Count, "Need resource update");

namespace draw {
struct render_control {
	int					id;
	rect				rc;
};
struct fxt {
	short int			filesize;			// the size of the file
	short int			charoffset[128];	// the offset of the pixel data from the beginning of the file, the index is the ascii value
	unsigned char		height;				// the height of a character in pixel
	unsigned char		width;				// the width of a character in pixel
	unsigned char		data[1];			// the pixel data, one byte per line 
};
}
namespace colors {
static color			dark = color::create(52, 52, 80);
static color			drag = color::create(250, 100, 250);
static color			down = color::create(81, 85, 166);
color					focus = color::create(250, 100, 100);
color					header = color::create(255, 255, 100);
color					light = color::create(148, 148, 172);
color					main = color::create(108, 108, 136);
color					selected = color::create(250, 250, 250);
color					title = color::create(64, 255, 255);
static color			hilite = main.mix(dark, 160);
namespace info {
color					text = color::create(64, 64, 64);
}
}

static render_control	render_objects[48];
static render_control*	render_current;
static bool				break_modal;
static int				break_result;
static surface			dc(320, 200, 32);
static surface			dw(dc.width * 3, dc.height * 3, 32);
static fxt*				font6 = (fxt*)loadb("art/misc/font6.fnt");
static fxt*				font8 = (fxt*)loadb("art/misc/font8.fnt");
unsigned				draw::frametick;
static unsigned			frametick_last;
static infoproc			show_mode;
static int				current_focus;
static item*			current_item;
static item*			drag_item;
static char				log_message[128];
static rect				log_rect = {5, 180, 285, 198};
static int				focus_stack[8];
static int				focus_level;
static int				focus_pressed;
extern callback			next_proc;
extern "C" void			scale3x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height);
void					view_dungeon_reset();
callback				draw::domodal;

int draw::ciclic(int range, int speed) {
	return iabs((int)((frametick*speed) % range * 2) - range);
}

sprite* draw::gres(resource_s id) {
	if(!objects[id].data) {
		if(!id)
			return 0;
		char temp[260];
		objects[id].data = (sprite*)loadb(szurl(temp, objects[id].path, objects[id].name, "pma"));
	}
	return objects[id].data;
}

void draw::resetres() {
	for(auto& e : objects) {
		if(e.data) {
			delete e.data;
			e.data = 0;
		}
	}
	view_dungeon_reset();
}

static void update_frame_counter() {
	unsigned cur = clock();
	if(!frametick_last)
		frametick_last = cur;
	const int ms = 1000 / 20;
	if(frametick_last + ms <= cur) {
		auto count = (cur - frametick_last) / ms;
		draw::frametick += count;
		frametick_last += count * ms;
	}
}

static void preredraw() {
	scale3x(
		dw.bits, dw.scanline,
		dc.ptr(0, 0), dc.scanline,
		dc.bpp / 8,
		dc.width, dc.height);
}

void draw::redraw() {
	preredraw();
	draw::state push;
	draw::canvas = &dw;
	rawredraw();
	update_frame_counter();
	frametick_last = clock() + 500;
}

int draw::texth() {
	if(!font)
		return 0;
	return ((fxt*)font)->height;
}

int draw::textw(int sym) {
	if(!font)
		return 0;
	return ((fxt*)font)->width;
}

void draw::glyph(int x, int y, int sym, unsigned flags) {
	auto f = (fxt*)font;
	int height = f->height;
	int width = f->width;
	for(int h = 0; h < height; h++) {
		unsigned char line = *((unsigned char*)font + ((fxt*)font)->charoffset[sym] + h);
		unsigned char bit = 0x80;
		for(int w = 0; w < width; w++) {
			if((line & bit) == bit)
				pixel(x + w, y + h);
			bit = bit >> 1;
		}
	}
}

void draw::setbigfont() {
	font = (sprite*)font8;
}

void draw::setsmallfont() {
	font = (sprite*)font6;
}

void draw::setmode(infoproc mode) {
	show_mode = mode;
}

static void border_up(rect rc) {
	draw::state push;
	draw::fore = colors::dark;
	draw::line(rc.x1, rc.y1, rc.x1, rc.y2);
	draw::line(rc.x1 + 1, rc.y2, rc.x2, rc.y2);
	draw::fore = colors::light;
	draw::line(rc.x2, rc.y1, rc.x2, rc.y2 - 1);
	draw::line(rc.x1 + 1, rc.y1, rc.x2 - 1, rc.y1);
}

static void border_down(rect rc) {
	draw::state push;
	draw::fore = colors::light;
	draw::line(rc.x1, rc.y1, rc.x1, rc.y2);
	draw::line(rc.x1 + 1, rc.y2, rc.x2, rc.y2);
	draw::fore = colors::dark;
	draw::line(rc.x2, rc.y1, rc.x2, rc.y2 - 1);
	draw::line(rc.x1 + 1, rc.y1, rc.x2 - 1, rc.y1);
}

int draw::flatb(int x, int y, int width, unsigned flags, const char* string) {
	int height = draw::texth() + 1;
	if(flags&Focused)
		rectf({x, y, x + width, y + height + 1}, colors::dark);
	else
		border_up({x, y, x + width, y + height});
	draw::text(x + 1 + (width - draw::textw(string)) / 2, y + 1, string);
	return draw::texth();
}

int draw::buttonm(int x, int y, int width, const cmd& ev, const char* name) {
	state push;
	rect rc = {x, y, x + width, y + texth()};
	focusing(rc, ev.focus);
	unsigned flags = 0;
	if(getfocus() == ev.focus) {
		flags |= Focused;
		fore = colors::focus;
		if(hot::key == KeyEnter)
			ev.execute();
	}
	textb(aligned(x, width, AlignCenter, textw(name)), y, name);
	return texth() + 1;
}

int draw::buttont(int x, int y, int width, const cmd& ev, const char* name) {
	state push;
	rect rc = {x, y, x + width, y + texth()};
	focusing(rc, ev.focus);
	unsigned flags = 0;
	if(getfocus() == ev.focus) {
		flags |= Focused;
		fore = colors::focus;
		if(hot::key == KeyEnter)
			ev.execute();
	}
	textb(draw::aligned(x, width, flags, textw(name)), y, name);
	return texth() + 1;
}

void draw::buttont(int x, int y, int width, const cmd& ev, const char* name, const char* name2) {
	state push;
	rect rc = {x, y, x + width, y + texth()};
	focusing(rc, ev.focus);
	unsigned flags = 0;
	if(getfocus() == ev.focus) {
		flags |= Focused;
		fore = colors::focus;
		if(hot::key == KeyEnter)
			ev.execute();
	}
	textb(rc.x1, rc.y1, name);
	int w2 = textw(name2);
	textb(rc.x2 - w2, rc.y1, name2);
}

int draw::button(int x, int y, int width, const cmd& ev, const char* name, int key) {
	draw::state push;
	if(width == -1)
		width = textw(name) + 3 * 2;
	rect rc = {x, y, x + width, y + draw::texth() + 4};
	form(rc);
	focusing(rc, ev.focus);
	auto run = false;
	unsigned flags = 0;
	if(getfocus() == ev.focus) {
		flags |= Focused;
		fore = colors::focus;
		if(hot::key == KeyEnter)
			run = true;
	}
	if(key && key == hot::key)
		run = true;
	rc.offset(3, 2);
	textb(rc, name, flags);
	if(run)
		ev.execute();
	return draw::texth();
}

void draw::greenbar(rect rc, int vc, int vm) {
	if(!vc || !vm || vc <= 0)
		return;
	color c0 = colors::black;
	color c1 = colors::green.darken().mix(colors::red, vc * 255 / vm);
	border_down(rc);
	rc.y1++;
	rc.x1++;
	rectf(rc, colors::down);
	rc.x2 = rc.x1 + vc * rc.width() / vm;
	rectf(rc, c1);
}

rect draw::form(rect rc, int count, bool focused, bool pressed) {
	for(int i = 0; i < count; i++) {
		if(pressed)
			border_down(rc);
		else
			border_up(rc);
		rc.offset(1);
	}
	rectf({rc.x1, rc.y1, rc.x2 + 1, rc.y2 + 1}, focused ? colors::hilite : colors::main);
	rc.offset(4, 4);
	return rc;
}

int draw::header(int x, int y, const char* text) {
	state push;
	fore = colors::header;
	draw::textb(x, y, text);
	return draw::texth() + 2;
}

void dlgmsg(const char* title, const char* text) {
	draw::screenshoot push;
}

void dlgerr(const char* title, const char* format, ...) {
	mslogv(format, xva_start(format));
}

infoproc draw::getmode() {
	return show_mode;
}

void draw::background(int rid) {
	image(0, 0, gres(resource_s(rid)), 0, 0);
}

void draw::initialize() {
	create(dw.width, dw.height);
	settimer(1000 / 20);
	setcaption("Eye of Beholder - Mehers dungeons");
	canvas = &dc;
	fore = colors::black;
	font = (sprite*)font6;
	setclip();
}

void draw::itemicn(int x, int y, item i, unsigned char alpha, int spell) {
	auto rs = gres(ITEMS);
	auto pt = i.getportrait();
	if(spell) {
		draw::state push;
		color pallette[256];
		auto e = rs->get(pt);
		auto p = (color*)rs->offs(e.pallette);
		memcpy(pallette, p, sizeof(pallette));
		switch(spell) {
		case DetectMagic:
			pallette[12] = colors::blue.mix(colors::white, draw::ciclic(128, 4));
			break;
		case DetectEvil:
			pallette[12] = colors::red.mix(colors::white, draw::ciclic(128, 4));
			break;
		}
		draw::palt = pallette;
		image(x, y, rs, pt, ImagePallette, alpha);
	} else
		image(x, y, rs, pt, 0, alpha);
}

static void log_delete_line() {
	auto p = zchr(log_message, '\n');
	if(p) {
		p++;
		memcpy(log_message, p, zlen(p) + 1);
	} else
		log_message[0] = 0;
}

static int get_log_line_count() {
	if(log_message[0] == 0)
		return 0;
	auto count = 1;
	auto p = log_message;
	while(true) {
		p = (char*)zchr(p, '\n');
		if(!p)
			break;
		p++;
		count++;
	}
	return count;
}

void mslogv(const char* format, const char* vl) {
	if(!format) {
		log_delete_line();
		return;
	}
	if(log_message[0]) {
		if(get_log_line_count() >= 3)
			log_delete_line();
	}
	stringbuilder e(zend(log_message), zendof(log_message));
	if(log_message[0])
		e.add("\n");
	e.addv(format, vl);
}

void mslog(const char* format, ...) {
	mslogv(format, xva_start(format));
}

void draw::logs() {
	draw::state push;
	setclip(log_rect);
	text(log_rect, log_message);
}

void draw::itemicn(int x, int y, item* pitm, bool invlist, unsigned flags, void* current_item) {
	if(!pitm)
		return;
	rect rc;
	auto pid = game.getwear(pitm);
	auto pc = game.getcreature(pitm);
	unsigned char alpha = 0xFF;
	if(invlist) {
		if(pid == LeftRing || pid == RightRing)
			rc.set(x - 4, y - 5, x + 4, y + 3);
		else
			rc.set(x - 8, y - 9, x + 8, y + 7);
	} else
		rc.set(x - 16, y - 7, x + 14, y + 8);
	if(current_item)
		focusing(rc, (int)pitm);
	if(pitm == current_item)
		rectb(rc, colors::selected);
	if(pitm == drag_item)
		rectb(rc, colors::drag);
	if(!(*pitm)) {
		if(!invlist)
			image(x, y, gres(ITEMS), 83 + ((pid == RightHand) ? 0 : 1), 0, alpha);
	} else {
		auto state = Bless;
		if(pc->is(DetectEvil) && pitm->iscursed())
			state = DetectEvil;
		else if(pc->is(DetectMagic) && pitm->ismagical())
			state = DetectMagic;
		itemicn(x, y, *pitm, alpha, state);
	}
	if(flags&Disabled)
		rectf({rc.x1, rc.y1, rc.x2 + 1, rc.y2 + 1}, colors::black, 192);
}

void draw::textb(int x, int y, const char* string, int count) {
	if(true) {
		state push;
		fore = colors::black;
		text(x + 1, y + 1, string, count);
	}
	text(x, y, string);
}

int draw::textb(rect rc, const char* string, unsigned flags) {
	if(true) {
		draw::state push;
		fore = colors::black;
		rect rc1 = rc; rc1.move(1, 1);
		text(rc1, string, flags);
	}
	return text(rc, string, flags);
}

static void place_item(item* itm) {
	if(drag_item) {
		creature::swap(drag_item, itm);
		drag_item = 0;
	} else
		drag_item = itm;
}

direction_s map_key_to_dir(int e) {
	switch(e) {
	case KeyLeft: return Left;
	case KeyRight: return Right;
	case KeyDown:return Down;
	case KeyUp: return Up;
	default: return Center;
	}
}

static void show_invertory(item* current_item) {
	draw::invertory(178, 0, game.getcreature(current_item), current_item);
}

static void show_abilities(item* current_item) {
	draw::abilities(178, 0, game.getcreature(current_item));
}

static void show_skills(item* current_item) {
	draw::skills(178, 0, game.getcreature(current_item));
}

void gamei::endround() {
	rounds++;
	passround();
	findsecrets();
	setnext(adventure);
}

void draw::adventure() {
	creature* pc;
	if(!game.isalive())
		setnext(mainmenu);
	while(ismodal()) {
		if(!current_item)
			current_item = party[0].getcreature()->getitem(RightHand);
		draw::animation::update();
		draw::animation::render(0, true, current_item);
		domodal();
		switch(hot::key) {
		case KeyEscape:
			if(true) {
				setmode(0);
				auto pc = game.getcreature(current_item);
				auto pid = game.getwear(current_item);
				if(pid != RightHand && pid != LeftHand)
					current_item = pc->getitem(RightHand);
				draw::animation::update();
				draw::animation::render(0);
			}
			options();
			break;
		case Alpha + 'I':
			if(getmode() == show_invertory) {
				setmode(0);
				auto pc = game.getcreature(current_item);
				auto pid = game.getwear(current_item);
				if(pid != RightHand && pid != LeftHand)
					current_item = pc->getitem(RightHand);
			} else {
				setmode(show_invertory);
				game.endround();
			}
			break;
		case Alpha + 'C':
			if(getmode() == show_abilities)
				setmode(0);
			else
				setmode(show_abilities);
			break;
		case Alpha + 'X':
			if(getmode() == show_skills)
				setmode(0);
			else
				setmode(show_skills);
			break;
		case Alpha + 'Q':
			if(current_item) {
				if(game.question(current_item))
					game.endround();
			}
			break;
		case KeyLeft:
		case KeyRight:
		case KeyDown:
		case KeyUp:
			location.move(map_key_to_dir(hot::key));
			break;
		case KeyHome:
			location.rotate(Left);
			break;
		case KeyPageUp:
			location.rotate(Right);
			break;
		case Alpha + 'W':
			current_item = (item*)getnext((int)current_item, KeyUp);
			break;
		case Alpha + 'Z':
			current_item = (item*)getnext((int)current_item, KeyDown);
			break;
		case Alpha + 'S':
			current_item = (item*)getnext((int)current_item, KeyRight);
			break;
		case Alpha + 'A':
			current_item = (item*)getnext((int)current_item, KeyLeft);
			break;
		case Alpha + 'P':
			place_item(current_item);
			break;
		case Alpha + 'G':
			location.pickitem(current_item);
			break;
		case Alpha + 'D':
			location.dropitem(current_item);
			break;
		case Alpha + 'U':
			if(creature::use(current_item))
				game.endround();
			break;
		case Alpha + 'T':
			game.thrown(current_item);
			break;
		case Alpha + 'M':
			if(game.manipulate(current_item, to(game.getdirection(), Up)))
				game.endround();
			break;
		case Alpha + 'V':
			location.automap(true);
			break;
		case Alpha + 'F':
			draw::animation::thrown(game.getcamera(), game.getdirection(), Arrow, Left, 50);
			break;
		case Alpha + 'H':
			if(true) {
				static messagei first_dialog[] = {{Say, 1, {}, "You see strange door from blue stone", {}, {{SHADOW}}},
				{Ask, 1, {OpenLocks}, "Pick", {0,2}},
				{Ask, 1, {Strenght}, "Crash"},
				{}};
				first_dialog->choose(true);
			}
			break;
		case Alpha + '1':
		case Alpha + '2':
		case Alpha + '3':
		case Alpha + '4':
		case Alpha + '5':
		case Alpha + '6':
			pc = party[hot::key - (Alpha + '1')].getcreature();
			if(!pc)
				break;
			if(game.getcreature(current_item) != pc)
				current_item = pc->getitem(RightHand);
			break;
		}
	}
}

static render_control* getby(int id) {
	for(auto& e : render_objects) {
		if(!e.id)
			return 0;
		if(e.id == id)
			return &e;
	}
	return 0;
}

static render_control* getfirst() {
	for(auto& e : render_objects) {
		if(!e.id)
			return 0;
		return &e;
	}
	return 0;
}

static render_control* getlast() {
	auto p = render_objects;
	for(auto& e : render_objects) {
		if(!e.id)
			break;
		p = &e;
	}
	return p;
}

static point center(const rect& rc) {
	return{(short)rc.x1, (short)rc.y1};
}

static int distance(point p1, point p2) {
	int dx = p1.x - p2.x;
	int dy = p1.y - p2.y;
	return isqrt(dx*dx + dy * dy);
}

static render_control* getnextfocus(int id, int key) {
	if(!key)
		return 0;
	auto pc = getby(id);
	if(!pc)
		pc = getfirst();
	if(!pc)
		return 0;
	auto pe = pc;
	auto pl = getlast();
	int inc = 1;
	if(key == KeyLeft || key == KeyUp)
		inc = -1;
	render_control* r1 = 0;
	auto p1 = center(pe->rc);
	while(true) {
		pc += inc;
		if(pc > pl)
			pc = render_objects;
		else if(pc < render_objects)
			pc = pl;
		if(pe == pc) {
			if(r1)
				return r1;
			return pe;
		}
		auto p2 = center(pc->rc);
		render_control* r2 = 0;
		switch(key) {
		case KeyRight:
			if(pe->rc.x2 < pc->rc.x1
				&& ((pe->rc.y1 >= pc->rc.y1 && pe->rc.y1 <= pc->rc.y2)
					|| (pe->rc.y2 >= pc->rc.y1 && pe->rc.y2 <= pc->rc.y2)))
				r2 = pc;
			break;
		case KeyLeft:
			if(pe->rc.x1 > pc->rc.x2
				&& ((pe->rc.y1 >= pc->rc.y1 && pe->rc.y1 <= pc->rc.y2)
					|| (pe->rc.y2 >= pc->rc.y1 && pe->rc.y2 <= pc->rc.y2)))
				r2 = pc;
			break;
		case KeyDown:
			if(p1.y < p2.y)
				r2 = pc;
			break;
		case KeyUp:
			if(p2.y < p1.y)
				r2 = pc;
			break;
		default:
			return pc;
		}
		if(r2) {
			if(!r1)
				r1 = r2;
			else {
				int d1 = distance(p1, center(r1->rc));
				int d2 = distance(p1, center(r2->rc));
				if(d2 < d1)
					r1 = r2;
			}
		}
	}
}

void draw::focusing(const rect& rc, int id) {
	if(!id)
		return;
	if(!render_current
		|| render_current >= render_objects + sizeof(render_objects) / sizeof(render_objects[0]))
		render_current = render_objects;
	render_current[0].rc = rc;
	render_current[0].id = id;
	render_current++;
	render_current[0].id = 0;
	if(!current_focus)
		setfocus(id);
}

int draw::getnext(int id, int key) {
	auto p = getnextfocus(id, key);
	if(p)
		return p->id;
	return id;
}

void draw::execute(callback proc, int param) {
	domodal = proc;
	hot::key = 0;
	hot::param = param;
}

void draw::breakmodal(int result) {
	break_modal = true;
	break_result = result;
}

void draw::buttoncancel() {
	breakmodal(0);
}

void draw::buttonok() {
	breakmodal(1);
}

void draw::buttonparam() {
	breakmodal(hot::param);
}

int draw::getresult() {
	return break_result;
}

static void standart_domodal() {
	preredraw();
	draw::state push;
	draw::canvas = &dw;
	rawinput();
	if(!hot::key)
		exit(0);
	update_frame_counter();
}

bool draw::ismodal() {
	render_current = render_objects;
	domodal = standart_domodal;
	if(next_proc)
		return false;
	if(!break_modal)
		return true;
	break_modal = false;
	return false;
}

void cmd::execute() const {
	if(proc)
		draw::execute(proc, param);
}

int draw::getfocus() {
	return current_focus;
}

void draw::setfocus(int id) {
	current_focus = id;
}

void draw::openform() {
	if((unsigned)focus_level < sizeof(focus_stack) / sizeof(focus_stack[0]))
		focus_stack[focus_level] = getfocus();
	focus_level++;
	setfocus(0);
	hot::key = 0;
	focus_pressed = 0;
}

void draw::closeform() {
	if(focus_level > 0) {
		focus_level--;
		if((unsigned)focus_level < sizeof(focus_stack) / sizeof(focus_stack[0]))
			setfocus(focus_stack[focus_level]);
	}
	hot::key = 0;
}

void draw::choose(const menu* source) {
	const auto w = 170;
	openform();
	while(ismodal()) {
		auto x = 80, y = 110;
		draw::background(MENU);
		draw::state push;
		fore = colors::white;
		setbigfont();
		for(auto p = source; *p; p++)
			y += buttonm(x, y, w, cmd(p->proc, 0, (int)p), p->text);
		domodal();
		navigate(true);
	}
	closeform();
}

bool draw::navigate(bool can_cancel) {
	switch(hot::key) {
	case KeyDown:
	case KeyUp:
	case KeyLeft:
	case KeyRight:
		setfocus(getnext(getfocus(), hot::key));
		break;
	case KeyEscape:
	case Alpha + 'B':
		if(!can_cancel)
			return false;
		breakmodal(0);
		break;
	default:
		return false;
	}
	return true;
}

void draw::chooseopt(const menu* source) {
	openform();
	while(ismodal()) {
		draw::animation::render(0, false);
		if(true) {
			draw::state push;
			setbigfont();
			form({0, 0, 22 * 8 + 2, 174}, 2);
			fore = colors::title;
			textb(6, 6, "Game Options:");
			fore = colors::white;
			for(int i = 0; source[i]; i++)
				button(4, 17 + i * 15, 166, source[i].proc, source[i].text);
		}
		domodal();
		navigate(true);
	}
	closeform();
}

const int dx = 4;

static rect getformpos(const char* text, int height = 0) {
	rect rc{0, 0, 200, 0};
	rc.y2 += draw::text(rc, text);
	rc.y2 += height;
	rc.offset(-dx, -dx);
	int x1 = (320 - rc.width()) / 2;
	int y1 = (200 - rc.height()) / 2;
	rc.move(x1, y1);
	return rc;
}

bool draw::dlgask(const char* text) {
	draw::state push;
	draw::screenshoot screen(true);
	fore = colors::white;
	rect rc = getformpos(text, draw::texth() + dx * 2);
	openform();
	while(draw::ismodal()) {
		screen.restore();
		form(rc);
		auto x1 = rc.x1 + dx;
		auto y1 = rc.y1 + dx;
		auto wd = rc.width() - dx * 2;
		auto rct = rc; rct.offset(dx, dx);
		y1 += draw::text(rct, text) + dx;
		x1 = (320 - (36 + 36)) / 2;
		button(x1, y1, 32, buttonok, "Yes");
		button(x1 + 36, y1, 32, buttoncancel, "No");
		domodal();
		navigate();
	}
	closeform();
	return getresult() != 0;
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

int answers::choosesm(const char* title, bool allow_cancel) const {
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	auto current_element = 0;
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
			text(rc.x1 + 1, rc.y1 + 1, title);
		}
		int x = rc.x1;
		int y = rc.y1 + 8;
		for(auto& e : elements) {
			unsigned flags = (elements.indexof(&e) == current_element) ? Focused : 0;
			y += labelb(x, y, rc.width(), flags, e.text);
		}
		domodal();
		switch(hot::key) {
		case KeyEscape:
			if(allow_cancel)
				breakmodal(0);
			break;
		case KeyEnter:
		case Alpha + 'U':
			breakmodal(elements.data[current_element].id);
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
					breakmodal(elements.data[id].id);
			}
			break;
		}
	}
	return getresult();
}

static int buttonw(int x, int y, const char* title, const void* id, unsigned key = 0) {
	auto w = textw(title);
	rect r1 = {x, y, x + w + 6, y + texth() + 3};
	focusing(r1, (int)id);
	auto isfocused = (getfocus() == (int)id);
	if((isfocused && hot::key == KeyEnter)
		|| (key && hot::key == key))
		focus_pressed = (int)id;
	else if(hot::key == InputKeyUp && focus_pressed == (int)id) {
		focus_pressed = 0;
		execute(buttonparam, (int)id);
	}
	form(r1, 1, isfocused, focus_pressed==(int)id);
	text(r1.x1 + 4, r1.y1 + 2, title);
	return w + 8;
}

int answers::choosebg(const char* title, bool border, const messagei::imagei* pi, bool horizontal_buttons) const {
	draw::animation::render(0);
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	openform();
	while(ismodal()) {
		screen.restore();
		if(pi) {
			for(int i = 0; i < 4; i++) {
				if(pi[i].res)
					image(100, 110, gres(pi[i].res), pi[i].id, pi[i].flags);
			}
		}
		if(border)
			image(0, 0, gres(BORDER), 0, 0);
		rect rc = {0, 121, 319, 199};
		form(rc);
		rc.offset(6, 4);
		rc.y1 += text(rc, title, AlignLeft) + 2;
		auto x = rc.x1 - 2, y = rc.y1;
		if(horizontal_buttons)
			y = getheight() - texth() - 6;
		for(unsigned i = 0; i < elements.count; i++) {
			if(horizontal_buttons)
				x += buttonw(x, y, elements.data[i].text, &elements.data[i], Alpha + '1' + i);
			else {
				buttonw(x, y, elements.data[i].text, &elements.data[i], Alpha + '1' + i);
				y += texth() + 5;
			}
		}
		domodal();
		navigate();
	}
	closeform();
	auto p = (element*)getresult();
	if(!p)
		return 0;
	return p->id;
}