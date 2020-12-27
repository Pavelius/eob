#include "view.h"

using namespace draw;

namespace draw {
struct render_control {
	void*				av;
	unsigned			param;
	rect				rc;
	void clear() { memset(this, 0, sizeof(*this)); }
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
namespace {
struct contexti {
	void*				object;
	int					title;
	const char*			header;
	const char*	getheader(const markup& e) const {
		if(e.title)
			return e.title;
		return header;
	}
	constexpr contexti(void* object) : object(object), title(84), header(0) {}
};
struct textedit {
	char				data[1024];
};
}

typedef adat<void*, 512> rowa;
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
static void*			current_focus;
static unsigned			current_focus_param;
static const markup*	current_focus_markup;
static void*			current_object;
static unsigned			current_size;
static const markup*	current_markup;
static item*			current_item;
static item*			drag_item;
static char				log_message[128];
static rect				log_rect = {5, 180, 285, 198};
static int				focus_level;
static const void*		focus_stack[8];
static const void*		focus_pressed;
extern callback			next_proc;
extern "C" void			scale3x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height);
static draw::surface	bitmap;
char					bitmap_url[260];
callback				draw::domodal;
static textedit			current_text;

int draw::ciclic(int range, int speed) {
	return iabs((int)((frametick*speed) % range * 2) - range);
}

static void setblink(color v) {
	const int ci = 16;
	int m = frametick % ci;
	if(m > ci / 2)
		m = ci - m;
	m += ci / 2;
	fore.r = v.r * m / ci;
	fore.g = v.g * m / ci;
	fore.b = v.b * m / ci;
}

static void correct(point& p, short x1, short y1, short x2, short y2) {
	if(x2 < 0)
		p.x = -x2 / 2;
	else {
		if(p.x > x2)
			p.x = x2;
		if(p.x < x1)
			p.x = x1;
	}
	if(y2 < 0)
		p.x = -y2 / 2;
	else {
		if(p.y > y2)
			p.y = y2;
		if(p.y < y1)
			p.y = y1;
	}
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
	focusing(rc, ev);
	unsigned flags = 0;
	if(isfocus(ev)) {
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
	focusing(rc, ev);
	unsigned flags = 0;
	if(isfocus(ev)) {
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
	focusing(rc, ev);
	unsigned flags = 0;
	if(isfocus(ev)) {
		flags |= Focused;
		fore = colors::focus;
		if(hot::key == KeyEnter)
			ev.execute();
	}
	textb(rc.x1, rc.y1, name);
	int w2 = textw(name2);
	textb(rc.x2 - w2, rc.y1, name2);
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
	if(i.is(Countable)) {
		auto count = i.getcount();
		if(count > 1) {
			state push;
			setsmallfont();
			fore = colors::white;
			char temp[16]; stringbuilder sb(temp);
			sb.add("%1i", count);
			text(x + 1 - textw(temp) / 2, y + 1, temp);
		}
	}
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
	draw::state push; setsmallfont();
	setclip(log_rect);
	text(log_rect, log_message);
}

void draw::itemicn(int x, int y, item* pitm, bool invlist, unsigned flags, void* current_item) {
	if(!pitm)
		return;
	rect rc;
	auto pid = game.getwear(pitm);
	auto pc = pitm->getowner();
	unsigned char alpha = 0xFF;
	if(invlist) {
		if(pid == LeftRing || pid == RightRing)
			rc.set(x - 4, y - 5, x + 4, y + 3);
		else
			rc.set(x - 8, y - 9, x + 8, y + 7);
	} else
		rc.set(x - 16, y - 7, x + 14, y + 8);
	if(current_item)
		focusing(rc, pitm);
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
	draw::invertory(178, 0, current_item->getowner(), current_item);
}

static void show_abilities(item* current_item) {
	draw::abilities(178, 0, current_item->getowner());
}

static void show_skills(item* current_item) {
	draw::skills(178, 0, current_item->getowner());
}

void gamei::endround() {
	rounds++;
	passround();
	findsecrets();
	setnext(adventure);
}

static int current_res_frame = 0;
static void* current_res_focus = 0;

static void increment_frame() {
	current_res_frame++;
}

static void decrement_frame() {
	current_res_frame--;
}

static void inverse_horizontal() {
	auto p = (messagei::imagei*)hot::param;
	p->flags ^= ImageMirrorH;
}

static void inverse_vertical() {
	auto p = (messagei::imagei*)hot::param;
	p->flags ^= ImageMirrorV;
}

int messagei::imagei::preview(int x, int y, int width, const void* object) {
	auto p = (messagei::imagei*)object;
	setimage(p->custom.get());
	auto sx = bitmap.width;
	auto sy = bitmap.height;
	if(sx > width)
		sx = width;
	if(sy > 184 - y - 4)
		sy = 184 - y - 4;
	rect rc = {x, y, x + sx + 1, y + sy + 1};
	border_down(rc);
	draw::state push; setclip(rc);
	draw::blit(*draw::canvas, rc.x1 + 1, rc.y1 + 1, sx, sy, p->flags, bitmap, 0, 0);
	switch(hot::key) {
	case Alpha + 'H': execute(inverse_horizontal, (int)p); break;
	case Alpha + 'V': execute(inverse_vertical, (int)p); break;
	}
	return rc.height();
}

int resourcei::preview(int x, int y, int width, const void* object) {
	char temp[260]; stringbuilder sb(temp);
	auto p = (resourcei*)object;
	auto id = (resource_s)(p - bsdata<resourcei>::elements);
	auto x0 = x + 86 + 3;
	auto y0 = y + 128 / 2 - 3;
	auto y1 = y + 196 / 2 - 3;
	auto sp = draw::gres(id);
	rect rc = {x, y, x + 176 + 1, y + 120 + 1};
	if(!isfocus(current_res_focus)) {
		current_res_focus = getfocus();
		current_res_focus = 0;
	}
	if(sp) {
		if(current_res_frame >= sp->count)
			current_res_frame = sp->count - 1;
		if(hot::key == Alpha + '+' && current_res_frame < sp->count - 1)
			execute(increment_frame);
		if(hot::key == Alpha + '-' && current_res_frame > 0)
			execute(decrement_frame);
	}
	border_down(rc);
	if(p->ismonster()) {
		draw::image(x0, y0, draw::gres(BLUE), 0, 0);
		draw::image(x0, y1, sp, 0, 0);
	} else if(p->isdungeon())
		draw::image(x0, y0, sp, 0, 0);
	else if(p->pack == PackOuttake) {
		draw::image(x0, y0, draw::gres(MEZZ), 0, 0);
		draw::image(x0, y1, sp, current_res_frame, 0);
	} else if(p->pack == PackCenter)
		draw::image(x0, y1 - 32, sp, current_res_frame, 0);
	else {
		draw::state push; setclip(rc);
		draw::image(x + 1, y + 1, sp, current_res_frame, 0);
	}
	y1 = rc.y2 + 3;
	sb.clear(); sb.add("Path: %1", bsdata<packi>::elements[p->pack].url);
	textb(x, y1, temp, -1); y1 += texth();
	if(sp) {
		sb.clear(); sb.add("Number of sprites: %1i", sp->count);
		textb(x, y1, temp, -1); y1 += texth();
	}
	if(p->ismonster()) {
		auto overlays = (sp->count / 6) - 1;
		if(overlays) {
			sb.clear(); sb.add("Number of overlays: %1i", overlays);
			textb(x, y1, temp, -1); y1 += texth();
		}
	}
	return y1 - y;
}

static render_control* getby(void* av, unsigned param) {
	for(auto& e : render_objects) {
		if(!e.av)
			return 0;
		if(e.av == av && e.param == param)
			return &e;
	}
	return 0;
}

static render_control* getfirst() {
	for(auto& e : render_objects) {
		if(!e.av)
			return 0;
		return &e;
	}
	return 0;
}

static render_control* getlast() {
	auto p = render_objects;
	for(auto& e : render_objects) {
		if(!e.av)
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

static render_control* getnextfocus(void* ev, int key, unsigned param) {
	if(!key)
		return 0;
	auto pc = getby(ev, param);
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

static void test_map() {
}

static void setfocus(void* v, unsigned param = 0) {
	current_focus = v;
	current_focus_param = param;
	current_focus_markup = 0;
}

static void movenext(int key) {
	auto p = getnextfocus(current_focus, key, current_focus_param);
	if(p)
		setfocus(p->av, p->param);
}

static item* movenext(item* current, int key) {
	auto p = getnextfocus(current, key, 0);
	if(p)
		return (item*)p->av;
	return (item*)current;
}

static void show_worldmap() {
}

void draw::adventure() {
	creature* pc;
	if(!game.isalive())
		setnext(mainmenu);
	while(ismodal()) {
		if(!current_item)
			current_item = party[0]->getitem(RightHand);
		draw::animation::update();
		draw::animation::render(0, true, current_item);
		domodal();
		switch(hot::key) {
		case KeyEscape:
			if(true) {
				setmode(0);
				auto pc = current_item->getowner();
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
				auto pc = current_item->getowner();
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
		case Alpha + 'E':
			if(true) {
				auto pc = current_item->getowner();
				auto caster = pc->getcaster();
				auto spell_element = pc->choosespell(caster);
				if(spell_element)
					pc->cast(spell_element, caster, 0);
			}
			break;
		case Alpha + 'W':
			current_item = movenext(current_item, KeyUp);
			break;
		case Alpha + 'Z':
			current_item = movenext(current_item, KeyDown);
			break;
		case Alpha + 'S':
			current_item = movenext(current_item, KeyRight);
			break;
		case Alpha + 'A':
			current_item = movenext(current_item, KeyLeft);
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
		case Ctrl + Alpha + 'V':
			game.worldmap();
			break;
		case Alpha + 'F':
			draw::animation::thrown(game.getcamera(), game.getdirection(), Arrow, Left, 50);
			break;
		case Alpha + 'H':
			test_map();
			break;
		case Alpha + '1':
		case Alpha + '2':
		case Alpha + '3':
		case Alpha + '4':
		case Alpha + '5':
		case Alpha + '6':
			pc = party[hot::key - (Alpha + '1')];
			if(!pc)
				break;
			if(current_item->getowner() != pc)
				current_item = pc->getitem(RightHand);
			break;
		}
	}
}

bool draw::isfocus(void* av, unsigned param) {
	return current_focus == av && current_focus_param == param;
}

void* draw::getfocus() {
	return current_focus;
}

void draw::focusing(const rect& rc, void* av, unsigned param) {
	if(!av)
		return;
	if(!render_current
		|| render_current >= render_objects + sizeof(render_objects) / sizeof(render_objects[0]))
		render_current = render_objects;
	render_current[0].rc = rc;
	render_current[0].av = av;
	render_current[0].param = param;
	render_current++;
	render_current->clear();
	if(!current_focus)
		setfocus(av, param);
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
	if(next_proc) {
		break_modal = false;
		return false;
	}
	if(!break_modal)
		return true;
	break_modal = false;
	return false;
}

void cmd::execute() const {
	if(proc)
		draw::execute(proc, param);
}

void draw::openform() {
	if((unsigned)focus_level < sizeof(focus_stack) / sizeof(focus_stack[0]))
		focus_stack[focus_level] = current_focus;
	focus_level++;
	current_focus = 0;
	focus_pressed = 0;
	hot::key = 0;
}

void draw::closeform() {
	if(focus_level > 0) {
		focus_level--;
		if((unsigned)focus_level < sizeof(focus_stack) / sizeof(focus_stack[0]))
			setfocus((void*)focus_stack[focus_level]);
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
		movenext(hot::key);
		break;
	case KeyEscape:
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

static int buttonw(int x, int y, const char* title, void* ev, unsigned key = 0, callback proc = 0) {
	auto w = textw(title);
	rect r1 = {x, y, x + w + 6, y + texth() + 3};
	focusing(r1, ev);
	auto isfocused = isfocus(ev);
	if((isfocused && hot::key == KeyEnter)
		|| (key && hot::key == key))
		focus_pressed = ev;
	else if(hot::key == InputKeyUp && focus_pressed == ev) {
		focus_pressed = 0;
		if(!proc)
			proc = buttonparam;
		execute(buttonparam, (int)ev);
	}
	form(r1, 1, isfocused, focus_pressed == ev);
	text(r1.x1 + 4, r1.y1 + 2, title);
	return w + 8;
}

int answers::choosebg(const char* title, const char* footer, const messagei::imagei* pi, bool horizontal_buttons) const {
	draw::animation::render(0);
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	openform();
	while(ismodal()) {
		screen.restore();
		if(pi) {
			setimage(pi->custom.get());
			if(bitmap)
				blit(*draw::canvas, 8, 8, bitmap.width, bitmap.height, pi->flags, bitmap, 0, 0);
			image(0, 0, gres(BORDER), 0, 0);
			auto push_color = fore;
			fore = color::create(120, 120, 120);
			line(8, 7, 167, 7);
			fore = push_color;
		}
		rect rc = {0, 121, 319, 199};
		form(rc);
		rc.offset(6, 4);
		rc.y1 += text(rc, title, AlignLeft) + 2;
		auto x = rc.x1, y = rc.y1;
		if(horizontal_buttons)
			y = getheight() - texth() - 6;
		for(unsigned i = 0; i < elements.count; i++) {
			if(horizontal_buttons)
				x += buttonw(x, y, elements.data[i].text, (void*)&elements.data[i], Alpha + '1' + i);
			else {
				buttonw(x, y, elements.data[i].text, (void*)&elements.data[i], Alpha + '1' + i);
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

item* itema::choose(const char* title, bool cancel_button) {
	char temp[260]; stringbuilder sb(temp);
	draw::animation::render(0);
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	openform();
	while(ismodal()) {
		screen.restore();
		rect rc = {0, 0, 180, 176};
		form(rc);
		rc.offset(6, 4);
		rc.y1 += text(rc, title, AlignLeft) + 2;
		auto x = rc.x1 - 2, y = rc.y1;
		for(unsigned i = 0; i < count; i++) {
			sb.clear();
			data[i]->getname(sb);
			buttonw(x, y, temp, data[i], Alpha + '1' + i);
			y += texth() + 4;
		}
		if(cancel_button) {
			if(hot::key == KeyEscape)
				breakmodal(0);
		}
		domodal();
		navigate();
	}
	closeform();
	return (item*)getresult();
}

int draw::button(int x, int y, int width, const cmd& ev, const char* name, int key) {
	draw::state push;
	if(width == -1)
		width = textw(name) + 3 * 2;
	rect rc = {x, y, x + width, y + draw::texth() + 4};
	form(rc);
	focusing(rc, ev);
	auto run = false;
	unsigned flags = 0;
	if(isfocus(ev)) {
		flags |= Focused;
		fore = colors::focus;
		if(hot::key == KeyEnter)
			run = true;
	}
	if(key && key == hot::key)
		run = true;
	rc.offset(3, 2);
	textb(rc, name, flags | TextSingleLine);
	if(run)
		ev.execute();
	return draw::texth();
}

static int buttonwb(int x, int y, const char* title, const cmd& proc, unsigned key = 0) {
	auto w = textw(title) + 3 * 2;
	button(x, y, w, proc, title, key);
	return w + 2;
}

static bool buttontxt(int x, int& y, int width, void* focus, const char* name, int key) {
	draw::state push;
	if(width == -1)
		width = textw(name) + 2;
	rect rc = {x, y, x + width, y + draw::texth()};
	focusing(rc, focus);
	auto run = false;
	if(isfocus(focus)) {
		fore = colors::focus;
		if(hot::key == KeyEnter)
			run = true;
	}
	if(key && key == hot::key)
		run = true;
	textb(rc, name, TextSingleLine);
	y += rc.height() + 1;
	return run;
}

static int headerc(int x, int y, const char* prefix, const char* title, const char* subtitle, int page, int page_maximum) {
	if(!title)
		return 0;
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1 %2", prefix, title);
	if(subtitle && subtitle[0])
		sb.adds(subtitle);
	auto fore_push = fore;
	fore = colors::title;
	textb(6, 6, temp);
	if(page_maximum > 1) {
		sb.clear();
		sb.add("Page %1i of %2i", page + 1, page_maximum);
		textb(draw::getwidth() - 6 - textw(temp), 6, temp);
	}
	fore = fore_push;
	return 11;
}

static fntext compare_callback;

static int qsort_compare(const void* v1, const void* v2) {
	auto p1 = (void**)v1;
	auto p2 = (void**)v2;
	char t1[260]; stringbuilder sb1(t1);
	auto s1 = compare_callback(*p1, sb1);
	if(!s1)
		s1 = "";
	char t2[260]; stringbuilder sb2(t2);
	auto s2 = compare_callback(*p2, sb2);
	if(!s2)
		s2 = "";
	return strcmp(s1, s2);
}

static void sort(void** storage, unsigned maximum, fntext getname) {
	compare_callback = getname;
	qsort(storage, maximum, sizeof(storage[0]), qsort_compare);
}

class choose_control : public rowa {
	int					origin;
	fntext				getname;
	fndraw				preview;
	int					perpage;
	variant_s			type;
	array*				source;
	static fntext		compare_callback;
	static void choose_item() {
		breakmodal(hot::param);
	}
	void correct() {
		if(origin + perpage > (int)count)
			origin = count - perpage;
		if(origin < 0)
			origin = 0;
	}
	static void button_next() {
		auto p = (choose_control*)hot::param;
		p->origin += p->perpage;
		p->correct();
	}
	static void button_prev() {
		auto p = (choose_control*)hot::param;
		p->origin -= p->perpage;
		p->correct();
	}
	static void edit_item() {
		auto p = (choose_control*)hot::param;
		auto pc = getfocus();
		if(!pc)
			return;
		auto& ei = bsdata<varianti>::elements[p->type];
		edit(ei.name, pc, ei.form.form, false);
	}
	static void button_add() {
		auto p = (choose_control*)hot::param;
		if(!p->source)
			return;
		auto pc = p->source->add();
		p->add(pc);
		setfocus(pc);
		p->ensurevisible(pc);
		edit_item();
	}
	int getindex(const void* v) const {
		for(auto p : *this) {
			if(v == p)
				return &p - data;
		}
		return -1;
	}
public:
	void setsource(array* v, variant_s t) {
		source = v;
		type = t;
	}
	void ensurevisible(const void* v) {
		auto i = getindex(v);
		if(i == -1)
			return;
		origin = (i / perpage)*perpage;
		correct();
	}
	int getcount() const {
		return count;
	}
	choose_control(fntext getname, fndraw preview) : origin(0), perpage(preview ? 11 : 11 * 2),
		getname(getname), preview(preview),
		type(NoVariant), source(0) {
	}
	void* choose(const char* title, const void* current_value, int width) const {
		if(!*this && type == NoVariant)
			return 0;
		openform();
		setfocus((void*)current_value);
		while(ismodal()) {
			form({0, 0, 320, 200}, 2);
			auto x = 4, y = 6;
			y += headerc(x, y, "Choose", title, 0, (origin + perpage - 1) / perpage, (count + perpage - 1) / perpage);
			auto y1 = y;
			void* current_element = 0;
			for(unsigned i = origin; i < count; i++) {
				char temp[260]; stringbuilder sb(temp);
				auto pt = data[i];
				auto pn = getname(pt, sb);
				if(!pn)
					pn = "None";
				if(source)
					y += button(x, y, width, cmd(edit_item, (int)this, (int)pt), pn) + 3 * 2;
				else
					y += button(x, y, width, cmd(choose_item, (int)pt, (int)pt), pn) + 3 * 2;
				if(isfocus(pt))
					current_element = pt;
				if(y >= 200 - 16 * 2) {
					if(preview)
						break;
					x += width + 4;
					y = y1;
				}
			}
			if(preview && current_element) {
				x += width + 4;
				preview(x, y1, 320 - 4 - x, current_element);
			}
			y = 200 - 12 - 4;
			x = 4;
			x += buttonwb(x, y, "Cancel", buttoncancel, KeyEscape);
			if(origin + perpage < (int)count)
				x += buttonwb(x, y, "Next", cmd(button_next, (int)this, (int)button_next), KeyPageDown);
			if(origin > 0)
				x += buttonwb(x, y, "Prev", cmd(button_prev, (int)this, (int)button_prev), KeyPageUp);
			if(source)
				x += buttonwb(x, y, "Add", cmd(button_add, (int)this, (int)button_add), F3);
			domodal();
			navigate(true);
		}
		closeform();
		return (void*)getresult();
	}
};

void draw::chooseopt(const menu* source, unsigned count, const char* title) {
	openform();
	while(ismodal()) {
		if(true) {
			draw::state push;
			setbigfont();
			form({0, 0, 320, 200}, 2);
			auto x = 4, y = 6;
			if(title) {
				fore = colors::title;
				textb(6, 6, title);
				y += 12;
			}
			fore = colors::white;
			for(unsigned i = 0; i < count; i++)
				button(4, 17 + i * 15, 166, source[i].proc, source[i].text);
		}
		auto y = 200 - 12 - 4;
		auto x = 4;
		x += buttonwb(x, y, "Cancel", buttoncancel, KeyEscape);
		domodal();
		navigate(true);
	}
	closeform();
}

static void setvalue(void* p, unsigned size, int v) {
	if(!p)
		return;
	switch(size) {
	case sizeof(char) : *((char*)p) = v; break;
	case sizeof(short) : *((short*)p) = v; break;
	case sizeof(int) : *((int*)p) = v; break;
	}
}

static int getvalue(void* p, unsigned size) {
	if(!p)
		return 0;
	switch(size) {
	case sizeof(char) : return *((char*)p);
	case sizeof(short) : return *((short*)p);
	case sizeof(int) : return *((int*)p);
	}
	return 0;
}

static void getrows(const array& source, const void* object, rowa& result, fnallow pallow, fntext getname) {
	auto p = result.begin();
	auto pe = result.endof();
	auto sm = source.getcount();
	for(unsigned i = 0; i < sm; i++) {
		auto ps = source.ptr(i);
		if(pallow && !pallow(object, ps))
			continue;
		if(p < pe) {
			*p = ps;
			p++;
		}
	}
	result.count = p - result.data;
	if(getname)
		sort(result.data, result.count, getname);
}

void* draw::choose(array& source, const char* title, void* object, const void* current, fntext pgetname, fnallow pallow, fndraw preview, int view_width, bool can_add) {
	if(!view_width)
		view_width = 154;
	auto type = varianti::find(&source);
	choose_control control(pgetname, preview);
	if(can_add && type)
		control.setsource(&source, type);
	getrows(source, object, control, pallow, pgetname);
	control.ensurevisible(current);
	return control.choose(title, current, view_width);
}

bool draw::choose(array& source, const char* title, void* object, void* field, unsigned field_size, const fnlist& list) {
	if(list.choose)
		return list.choose(object, source, field);
	auto current_value = (void*)getvalue(field, field_size);
	if(field_size < sizeof(int))
		current_value = source.ptr((int)current_value);
	auto result = choose(source, title, object, current_value, list.getname, list.allow, list.preview, list.view_width);
	if(!result)
		return false;
	if(field_size < sizeof(int)) {
		current_value = (void*)source.indexof(result);
		if(current_value == (void*)0xFFFFFFFF)
			return false;
	}
	setvalue(field, field_size, (int)current_value);
	if(list.update)
		list.update(object);
	return true;
}

DGINF(textedit) = {{"Enter text", DGREQ(data)},
{}};

bool textable::edit(void* object, const array& source, void* pointer) {
	auto v = (textable*)pointer;
	memset(&current_text, 0, sizeof(current_text));
	stringbuilder sb(current_text.data);
	sb.add(v->get());
	if(!draw::edit("Text", &current_text, dginf<meta_decoy<decltype(current_text)>::value>::meta, true))
		return false;
	v->set(current_text.data);
	return true;
}

static void delete_symbol() {
	auto p = (char*)current_markup->value.ptr(current_object);
	auto s = current_markup->value.size;
	p[0] = 0;
}

static void add_symbol(char* p, unsigned size, int k) {
	auto n = zlen(p);
	switch(k) {
	case 8:
		if(n)
			p[n - 1] = 0;
		break;
	case 13: case 10:
		break;
	default:
		if(k <= 128) {
			if(n < (int)(size - 1)) {
				p[n] = k;
				p[n + 1] = 0;
			}
		}
		break;
	}
}

static void add_symbol() {
	add_symbol((char*)current_markup->value.ptr(current_object),
		current_markup->value.size,
		hot::param);
}

static void add_text_symbol() {
	add_symbol(current_text.data,
		sizeof(current_text.data) - 1,
		hot::param);
}

static void choose_enum_field() {
	if(!current_markup->value.source) {
		if(current_markup->list.choose) {
			array source;
			choose(source, current_markup->title,
				current_object, current_markup->value.ptr(current_object), current_markup->value.size,
				current_markup->list);
		} else {
			edit(current_markup->title,
				current_markup->value.ptr(current_object),
				current_markup->value.type, false);
		}
	} else {
		choose(*current_markup->value.source, current_markup->title,
			current_object, current_markup->value.ptr(current_object), current_markup->value.size,
			current_markup->list);
	}
}

static void getname(const markup& e, const void* object, stringbuilder& sb) {
	auto value = e.value.ptr((void*)object);
	if(e.value.source) {
		value = (void*)getvalue(value, e.value.size);
		if(e.value.size < sizeof(int))
			value = e.value.source->ptr((int)value);
	} else if(e.list.getptr)
		value = e.list.getptr(object, getvalue(value, e.value.size));
	auto pfn = e.list.getname;
	if(pfn) {
		if(value) {
			auto pn = pfn((void*)value, sb);
			if(pn && pn != sb)
				sb.add(pn);
		}
	} else if(e.value.istext()) {
		auto p = (const char*)getvalue(value, e.value.size);
		if(!p)
			p = "";
		sb.add(p);
	} else if(e.value.isnum()) {
		if(e.value.size <= sizeof(int)) {
			auto v = getvalue(value, e.value.size);
			sb.add("%1i", v);
		} else
			sb.add((char*)value);
	}
	if(!sb || !sb[0])
		sb.add("None");
}

static void add_number(void* p, unsigned size, int r, int d, int v) {
	setvalue(p, size, getvalue(p, size) * r / d + v);
}

static void add_number() {
	add_number(current_object, current_size, 10, 1, hot::param);
}

static void increment() {
	add_number(current_object, current_size, 1, 1, 1);
}

static void decrement() {
	add_number(current_object, current_size, 1, 1, -1);
}

static void inverse() {
	add_number(current_object, current_size, -1, 1, 0);
}

static void sub_number() {
	add_number(current_object, current_size, 1, 10, 0);
}

static void clear_value() {
	if(current_markup->value.size <= sizeof(int))
		setvalue(current_markup->value.ptr(current_object), current_markup->value.size, 0);
	else
		memset(current_markup->value.ptr(current_object), 0, current_markup->value.size);
}

static void clear_text_value() {
	current_text.data[0] = 0;
	clear_value();
}

static void post(const markup& e, void* object, callback proc, int param) {
	current_markup = &e;
	current_object = object;
	execute(proc, param);
}

static void event_number(void* object, unsigned size) {
	if(hot::key == KeyBackspace) {
		current_object = object;
		current_size = size;
		execute(sub_number);
	} else if(hot::key >= (Alpha + '0') && hot::key <= (Alpha + '9')) {
		current_object = object;
		current_size = size;
		execute(add_number, hot::key - (Alpha + '0'));
	} else if(hot::key == (Alpha + '+')) {
		current_object = object;
		current_size = size;
		execute(increment);
	} else if(hot::key == (Alpha + '-')) {
		current_object = object;
		current_size = size;
		execute(decrement);
	} else if(hot::key == (Alpha + '*')) {
		current_object = object;
		current_size = size;
		execute(inverse);
	}
}

static int field(const rect& rco, const char* title, void* object, const markup& e, unsigned flags) {
	auto rich_edit = rco.height() >= texth() * 2;
	auto push_fore = fore;
	fore = colors::white;
	if(!title || title[0] == 0) {
		if(rich_edit) {
			fore = fore.mix(colors::gray, 128);
			title = e.title;
		} else
			title = "None";
	}
	auto pv = e.value.ptr(object);
	if(rich_edit)
		form(rco, 1, false, true);
	else
		form(rco);
	focusing(rco, pv);
	auto rc = rco;
	rc.offset(3, 2);
	auto focused = isfocus(pv);
	if(focused) {
		fore = colors::focus;
		if(hot::key == KeyDelete)
			post(e, object, clear_value, 0);
		else if(e.list.choose || !e.value.isnum()) {
			if(hot::key == KeyEnter)
				post(e, object, choose_enum_field, 0);
		} else if(e.value.size <= sizeof(int))
			event_number(e.value.ptr(object), e.value.size);
		else if(hot::key == InputSymbol)
			post(e, object, add_symbol, hot::param);
	}
	textb(rc, title, flags);
	fore = push_fore;
	return rco.height();
}

static int field(int x, int y, int width, const char* title, void* object, int title_width, const markup& e) {
	char temp[260]; stringbuilder sb(temp);
	if(e.proc.getheader)
		title = e.proc.getheader(object, sb);
	if(title) {
		textb(x + 6, y + 2, title);
		x += title_width;
		width -= title_width;
	}
	sb.clear(); getname(e, object, sb);
	return field({x, y, x + width, y + draw::texth() + 4}, temp, object, e, TextSingleLine);
}

static void checkmark(int x, int y, int state) {
	auto dy = texth();
	rect rc = {x, y, x + 7, y + dy - 1};
	form(rc, 1, false, state ? false : true);
	rc.x1++; rc.y1++;
	if(state)
		rectf({rc.x1 + 1, rc.y1 + 1, rc.x2 - 1, rc.y2 - 1}, colors::title);
}

static void change_current_check(void* pv, unsigned size, unsigned mask) {
	auto v = getvalue(pv, size);
	v ^= mask;
	setvalue(pv, size, v);
}

static void change_current_check() {
	change_current_check(current_markup->value.ptr(current_object),
		current_markup->value.size, hot::param);
}

static int checkbox(int x, int y, const char* title, const markup& e, void* object, unsigned mask) {
	draw::state push;
	const auto cw = 16;
	auto width = textw(title) + cw;
	rect rc = {x, y, x + width, y + draw::texth() + 2};
	auto pv = e.value.ptr(object);
	focusing(rc, pv, mask);
	auto run = false;
	unsigned flags = 0;
	if(isfocus(pv, mask)) {
		flags |= Focused;
		fore = colors::focus;
		if(hot::key == KeyEnter || hot::key == KeySpace)
			post(e, object, change_current_check, mask);
	}
	rc.offset(0, 1);
	auto s = ((getvalue(pv, e.value.size) & mask) != 0) ? 1 : 0;
	checkmark(rc.x1, rc.y1, s);
	rc.x1 += cw;
	textb(rc, title, flags | TextSingleLine);
	return draw::texth() + 2;
}

static int checkboxes(int x, int y, int width, const markup& e, void* object, unsigned char size) {
	auto ar = e.value.source;
	auto gn = e.list.getname;
	auto pv = e.value.ptr(object);
	rowa storage;
	auto y0 = y;
	auto y1 = y0 + 16 * (texth() + 2);
	getrows(*ar, object, storage, e.list.allow, e.list.getname);
	if(storage.count > 16)
		width = width / 2;
	auto im = storage.count;
	for(unsigned i = 0; i < im; i++) {
		char temp[260]; stringbuilder sb(temp);
		auto ps = storage[i];
		auto pi = ar->indexof(ps);
		y += checkbox(x, y, gn(ps, sb), e, object, 1 << pi);
		if(y >= 200 - 16 * 2) {
			y = y0;
			x += width;
		}
	}
	return y - y0;
}

static void add_record(const markup& e, void* object) {
	auto value = draw::choose(*e.value.source, e.title, object, 0,
		e.list.getname, e.list.allow, e.list.preview, e.list.view_width);
	auto index = e.value.source->indexof(value);
	if(index == -1)
		return;
	auto mx = e.value.source->getcount();
	if(!mx)
		return;
	auto se = e.value.size / mx;
	auto pv = (char*)e.value.ptr(object) + index*se;
	*pv = 1;
}

static void add_record_call() {
	add_record(*current_markup, current_object);
}

static int tablerow(int x, int y, int width, const char* title, const markup& e, const void* object, void* pv, unsigned size) {
	auto fore_push = fore;
	rect rc = {x, y, x + width - 1, y + draw::texth()};
	focusing(rc, pv);
	auto focused = isfocus(pv);
	if(focused) {
		fore = colors::focus;
		event_number(pv, size);
	}
	char temp[260]; stringbuilder sb(temp);
	auto value = getvalue(pv, size);
	sb.add(title, value);
	sb.adds("%1i", value);
	textb(rc, temp, AlignLeft);
	fore = fore_push;
	return rc.height() + 2;
}

static int tableadatc(int x, int y, int width, const markup& e, void* object, unsigned char size) {
	auto ar = e.value.source;
	auto gn = e.list.getname;
	auto pv = e.value.ptr(object);
	auto y0 = y;
	auto y1 = 170;
	rowa storage;
	getrows(*ar, object, storage, e.list.allow, e.list.getname);
	auto im = storage.count;
	if(!im)
		return 0;
	if(im > 16)
		width = width / 2;
	auto element_size = 1;
	for(unsigned i = 0; i < im; i++) {
		auto v = storage[i];
		auto pi = ar->indexof(v);
		auto pr = (char*)pv + pi*element_size;
		auto nv = getvalue(pr, element_size);
		if(!nv)
			continue;
		char temp[260]; stringbuilder sb(temp);
		y += tablerow(x, y, width, gn(v, sb), e, object, pr, element_size);
		if(y >= y1) {
			y = y0;
			x += width;
		}
	}
	auto fore_push = fore;
	fore = colors::header;
	if(buttontxt(x, y, width, add_record, "Add record", F3)) {
		current_markup = &e;
		current_object = object;
		execute(add_record_call);
	}
	fore = fore_push;
	return y - y0;
}

class edit_control : contexti {
	int				page, page_maximum;
	const markup*	elements;
	static void next_page() {
		((edit_control*)hot::param)->page++;
		setfocus(0);
	}
	static void prev_page() {
		((edit_control*)hot::param)->page--;
		setfocus(0);
	}
	const markup* getcurrentpage() {
		const markup* pages[32];
		auto ps = pages;
		auto pe = pages + sizeof(pages) / sizeof(pages[0]);
		if(!elements->ispage())
			*ps++ = elements;
		for(auto p = elements; *p; p++) {
			if(!p->ispage())
				continue;
			if(!p->isvisible(object))
				continue;
			if(ps < pe)
				*ps++ = p;
		}
		page_maximum = ps - pages;
		if(page >= page_maximum)
			page = page_maximum - 1;
		if(!page_maximum)
			pages[0] = elements;
		return pages[page];
	}
	static int group(int x, int y, int width, const contexti& ctx, const markup* form) {
		if(!form)
			return 0;
		auto y0 = y;
		for(auto f = form; *f; f++) {
			if(f->ispage())
				break;
			auto h = element(x, y, width, ctx, *f);
			if(!h)
				continue;
			y += h + 2;
		}
		return y - y0;
	}
	static int element(int x, int y, int width, const contexti& ctx, const markup& e) {
		if(!e.isvisible(ctx.object))
			return 0;
		if(e.isgroup()) {
			contexti c1 = ctx;
			c1.header = ctx.getheader(e);
			c1.object = e.value.ptr(ctx.object);
			return group(x, y, width, c1, e.value.type);
		} else if(e.ischeckboxes())
			return checkboxes(x, y, width, e, ctx.object, e.value.size);
		else if(e.value.mask)
			return checkbox(x, y, e.title, e, ctx.object, e.value.mask);
		else if(e.value.istext())
			return 0;
		else if(e.value.isnum() && e.value.size > sizeof(nameablei::name))
			return field({x, y, x + width, y + draw::texth() * 5 + 4}, (const char*)e.value.ptr(ctx.object), ctx.object, e, 0);
		else
			return field(x, y, width, ctx.getheader(e), ctx.object, ctx.title, e);
	}
	const char* gettitle(const markup* pm) const {
		if(!pm || !pm->title || pm->title[0] != '#')
			return "";
		return zskipsp(pm->title + 4);
	}
public:
	constexpr edit_control(void* object, const markup* pm) : contexti(object), elements(pm),
		page(0), page_maximum(0) {
	}
	bool edit(const char* title, bool cancel_button) {
		state push; setsmallfont();
		openform();
		while(ismodal()) {
			form({0, 0, 320, 200}, 2);
			auto x = 4, y = 6;
			auto pm = getcurrentpage();
			y += headerc(x, y, "Edit", title, gettitle(pm), page, page_maximum);
			auto width = draw::getwidth() - x * 2;
			if(pm->ischeckboxes())
				checkboxes(x, y, width, *pm, object, pm->value.size);
			else if(pm->is("adc"))
				tableadatc(x, y + 1, width, *pm, object, pm->value.size);
			else if(pm->is("div"))
				y += group(x, y, width, *this, pm + 1);
			else if(pm->ispage()) {
				contexti ctx = *this;
				ctx.object = pm->value.ptr(object);
				y += group(x, y, width, ctx, pm->value.type);
			} else
				y += group(x, y, width, *this, pm);
			// Footer
			x = 4; y = 200 - 12 - 4;
			if(cancel_button) {
				x += buttonwb(x, y, "Cancel", buttoncancel, KeyEscape);
				x += buttonwb(x, y, "OK", buttonok, Ctrl + Alpha + 'S');
			} else
				x += buttonwb(x, y, "OK", buttonok, KeyEscape);
			if(page > 0)
				x += buttonwb(x, y, "Prev", cmd(prev_page, (int)this, (int)prev_page), KeyPageUp);
			if(page < page_maximum - 1)
				x += buttonwb(x, y, "Next", cmd(next_page, (int)this, (int)next_page), KeyPageDown);
			domodal();
			navigate(false);
		}
		closeform();
		return getresult() != 0;
	}
};

bool draw::edit(const char* title, void* object, const markup* pm, bool cancel_button) {
	edit_control e(object, pm);
	return e.edit(title, cancel_button);
}

void draw::editor() {
	auto push_font = font;
	setsmallfont();
	messagei it = {};
	draw::edit("Test", &it, dginf<decltype(it)>::meta, false);
	//game.companyi::read("default");
	//edit("Company", &game, dginf<companyi>::meta, false);
	//game.companyi::write("default");
	font = push_font;
}

static void paintparty(point camera, point party) {
	auto p = party - camera;
	setblink(colors::white);
	pixel(p.x, p.y);
	pixel(p.x, p.y + 1);
	pixel(p.x, p.y + 2);
	pixel(p.x, p.y - 1);
	pixel(p.x, p.y - 2);
	pixel(p.x + 2, p.y);
	pixel(p.x + 1, p.y);
	pixel(p.x - 1, p.y);
	pixel(p.x - 2, p.y);
}

void draw::setimage(const char* id) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("art/custom/%-1.bmp", id);
	if(strcmp(bitmap_url, temp) == 0)
		return;
	zcpy(bitmap_url, temp);
	bitmap.read(bitmap_url);
}

point draw::choosepoint(point camera) {
	state push;
	openform();
	while(ismodal()) {
		point origin;
		correct(camera, 0, 0, bitmap.width, bitmap.height);
		fullimage(camera, &origin);
		auto p = camera - origin;
		setblink(colors::white);
		pixel(p.x, p.y);
		pixel(p.x - 1, p.y);
		pixel(p.x + 1, p.y);
		pixel(p.x, p.y - 1);
		pixel(p.x, p.y + 1);
		domodal();
		switch(hot::key) {
		case KeyLeft: camera.x--; break;
		case KeyHome: camera.x--; camera.y--; break;
		case KeyEnd: camera.x--; camera.y++; break;
		case KeyRight: camera.x++; break;
		case KeyPageUp: camera.x++; camera.y--; break;
		case KeyPageDown: camera.x++; camera.y++; break;
		case KeyUp: camera.y--; break;
		case KeyDown: camera.y++; break;
		case KeyEnter: breakmodal(1); break;
		case KeyEscape: breakmodal(-1); break;
		}
	}
	closeform();
	if(getresult() == -1)
		return {0, 0};
	return camera;
}

void draw::fullimage(point camera, point* origin) {
	auto sx = draw::getwidth();
	auto sy = draw::getheight();
	auto mx = bitmap.width - sx;
	auto my = bitmap.height - sy;
	if(mx < 0 && my < 0) {
		rectf({0, 0, sx, sy}, colors::black);
		blit(*draw::canvas, -mx / 2, -my / 2, bitmap.width, bitmap.height, 0, bitmap, 0, 0);
		if(origin)
			*origin = {0, 0};
	} else {
		camera.x -= sx / 2;
		camera.y -= sy / 2;
		correct(camera, 0, 0, mx, my);
		blit(*draw::canvas, 0, 0, sx, sy, 0, bitmap, camera.x, camera.y);
		if(origin)
			*origin = camera;
	}
}

void draw::fullimage(point from, point to, point* origin) {
	auto sx = bitmap.width, sy = bitmap.height;
	correct(from, getwidth() / 2, getheight() / 2,
		sx - getwidth() / 2, sy - getheight() / 2);
	correct(to, getwidth() / 2, getheight() / 2,
		sx - getwidth() / 2, sy - getheight() / 2);
	const auto step = 1;
	auto x0 = from.x;
	auto y0 = from.y;
	auto w = getwidth();
	auto h = getheight();
	auto x1 = to.x;
	auto y1 = to.y;
	auto lenght = distance({(short)x0, (short)y0}, {(short)x1, (short)y1});
	if(!lenght)
		return;
	auto start = 0;
	auto dx = x1 - x0;
	auto dy = y1 - y0;
	auto camera = from;
	while(start <= lenght && ismodal()) {
		fullimage(camera, origin);
		redraw();
		start += step;
		short x2 = x0 + dx * start / lenght;
		short y2 = y0 + dy * start / lenght;
		camera.x = x2;
		camera.y = y2;
	}
	camera.x = x1;
	camera.y = y1;
}

void draw::appear(pobject proc, void* object, unsigned duration) {
	screenshoot before;
	proc(object);
	draw::screenshoot after;
	before.blend(after, duration);
}

void draw::pause() {
	openform();
	while(draw::ismodal()) {
		draw::domodal();
		switch(hot::key) {
		case KeyEscape:
		case KeySpace:
			return;
		}
	}
	closeform();
}

static void newgame() {
	game.clear();
	game.companyi::read("default");
	game.setcamera(Blocked);
	creature::view_party();
	draw::resetres();
	game.enter(game.start, 1);
	setnext(adventure);
}

static void main_new_game() {
	setnext(newgame);
}

static void option_new_game() {
	if(!dlgask("Are you really want to start new game?"))
		return;
	setnext(newgame);
}

static void memorize_spells() {
	creature::preparespells(Mage);
}

static void pray_for_spells() {
	creature::preparespells(Cleric);
}

static void option_save_game() {
	game.write();
	setnext(adventure);
}

static void quit_game() {
	if(!dlgask("Are you really want to quit game?"))
		return;
	exit(0);
}

static void settings() {}

extern void load_game();

void draw::options() {
	static menu elements[] = {{pray_for_spells, "Pray for spells"},
	{memorize_spells, "Memorize spells"},
	{creature::scriblescrolls, "Scrible scrolls"},
	{option_new_game, "New game"},
	{load_game, "Load game"},
	{option_save_game, "Save game"},
	{settings, "Settings"},
	{quit_game, "Quit game"},
	{}};
	chooseopt(elements);
}

void draw::mainmenu() {
	static draw::menu source[] = {{main_new_game, "Create New Game"},
	{load_game, "Load Saved game"},
	{draw::editor, "Game editor"},
	{quit_game, "Exit game"},
	{}};
	choose(source);
}