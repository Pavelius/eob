#include "view.h"

using namespace draw;

namespace draw {
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
struct parami {
	int					origin;
	int					maximum;
	int					perpage;
	void correct() {
		if(origin + perpage > maximum)
			origin = maximum - perpage;
		if(origin < 0)
			origin = 0;
	}
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
static int				current_param;
static const markup*	current_markup;
static item*			current_item;
static imagei			current_image;
static item*			drag_item;
static char				log_message[128];
static rect				log_rect = {5, 180, 285, 198};
static int				focus_level;
static const void*		focus_stack[8];
static const void*		focus_pressed;
extern callback			next_proc;
extern "C" void			scale3x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height);
callback				draw::domodal;
static textedit			current_text;
static void*			current_edit;
static int				current_c1;
static point			current_p1;

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
		if(pc) {
			if(pc->is(DetectEvil) && pitm->iscursed())
				state = DetectEvil;
			else if(pc->is(DetectMagic) && pitm->ismagical())
				state = DetectMagic;
		}
		itemicn(x, y, *pitm, alpha, state);
	}
	if(flags&Disabled)
		rectf({rc.x1, rc.y1, rc.x2 + 1, rc.y2 + 1}, colors::black, 192);
}

static int texti(rect rc, const char* string, unsigned state, int i1, point& p1) {
	p1.x = rc.x1; p1.y = rc.y1;
	auto ps = string;
	int x1 = rc.x1;
	int y1 = rc.y1 + alignedh(rc, string, state);
	int dy = texth();
	if(state&TextSingleLine) {
		draw::state push;
		setclip(rc);
		auto w = draw::textw(string);
		auto b = aligned(x1, rc.width(), state, w);
		p1.x = b + i1*textw('A');
		p1.y = y1;
		text(b, y1, string, -1, state);
		return dy;
	} else {
		int w1 = rc.width();
		while(true) {
			int w;
			int c = textbc(string, w1, &w);
			if(!c)
				break;
			auto b = aligned(x1, w1, state, w);
			auto c1 = i1 - (string - ps);
			if(c1 <= c) {
				p1.x = b + c1*textw('A');
				p1.y = y1;
			} else
				c1 -= c;
			text(b, y1, string, c, state);
			y1 += dy;
			string = skiptr(string + c);
		}
		return y1 - rc.y1;
	}
}

void draw::textb(int x, int y, const char* string, int count) {
	if(true) {
		state push;
		fore = colors::black;
		text(x + 1, y + 1, string, count);
	}
	text(x, y, string);
}

static int textbi(rect rc, const char* string, unsigned flags, int i1, point& p1) {
	if(true) {
		draw::state push;
		fore = colors::black;
		rect rc1 = rc; rc1.move(1, 1);
		text(rc1, string, flags);
	}
	return texti(rc, string, flags, i1, p1);
}

int draw::textb(rect rc, const char* string, unsigned flags) {
	if(true) {
		draw::state push;
		fore = colors::black;
		rect rc1 = rc; rc1.move(1, 1);
		point p1;
		texti(rc1, string, flags, 0, p1);
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

static void dungeon_preview(int x0, int y0, const sprite* sp) {
	draw::image(x0, y0, sp, 0, 0);
	draw::image(x0, y0, sp, 3, 0);
	draw::image(x0, y0, sp, 4, ImageMirrorH);
	draw::image(x0 - 80, y0, sp, 8, 0);
	draw::image(x0 + 48, y0, sp, 7, ImageMirrorH);
	draw::image(x0 + 48 * 2, y0, sp, 7, 0);
	draw::image(x0 + 48, y0, sp, 76, 0);
}

static void dungeon_corner(int x0, int y0, const sprite* sp) {
	draw::image(x0, y0, sp, 0, 0);
	draw::image(x0, y0, sp, 8, ImageMirrorH);
	draw::image(x0, y0, sp, 2, 0);
	draw::image(x0, y0, sp, 2, ImageMirrorH);
	draw::image(x0, y0, sp, 1, ImageMirrorH);
	draw::image(x0 - 128, y0, sp, 9, 0);
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
		current_res_frame = 0;
	}
	if(sp) {
		if(current_res_frame >= sp->count)
			current_res_frame = sp->count - 1;
		if(hot::key == '+' && current_res_frame < sp->count - 1)
			execute(increment_frame);
		if(hot::key == '-' && current_res_frame > 0)
			execute(decrement_frame);
	}
	border_down(rc);
	auto& ei = bsdata<packi>::elements[p->pack];
	if(true) {
		draw::state push; draw::setclip({rc.x1 + 1, rc.y1 + 1, rc.x2, rc.y2});
		if(p->ismonster()) {
			draw::image(x0, y0, draw::gres(BLUE), 0, 0);
			draw::image(x0, y1, sp, 0, 0);
		} else if(p->isdungeon())
			dungeon_preview(x0, y0, sp);
		else if(p->pack == Pack160x96) {
			draw::image(x + 1, y + 1, gres(BORDER), 0, 0);
			draw::image(x + 1 + 8, y + 1 + 8, sp, current_res_frame, 0);
		} else if(p->pack == PackOuttake) {
			dungeon_corner(x0, y0, draw::gres(BLUE));
			draw::image(x0, y1, sp, current_res_frame, 0);
		} else if(p->pack == PackCenter)
			draw::image(x0, y1 - 32, sp, current_res_frame, 0);
		else {
			draw::state push; setclip(rc);
			draw::image(x + 1, y + 1, sp, current_res_frame, 0);
		}
	}
	y1 = rc.y2 + 3;
	sb.clear(); sb.add("Path: %1", bsdata<packi>::elements[p->pack].url);
	textb(x, y1, temp, -1); y1 += texth();
	if(sp) {
		if(ei.choose_frame) {
			sb.clear(); sb.add("Sprite: %1i of %2i", current_res_frame, sp->count);
			textb(x, y1, temp, -1); y1 += texth();
		}
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

bool imagei::choose(void* object, const array& source, void* pointer) {
	auto p = (imagei*)pointer;
	auto& ei = p->gete();
	current_res_focus = (void*)&ei;
	current_res_frame = p->frame;
	auto pr = draw::choose(bsdata<resourcei>::source, "Images",
		object, &ei, getnm<resourcei>, current_markup->list.match, resourcei::preview, 320-190);
	if(!pr)
		return false;
	p->res = (resource_s)((resourcei*)pr - bsdata<resourcei>::elements);
	p->frame = current_res_frame;
	return true;
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
	auto p = "When you arrive to the bank, test this.\nAnd then test this.\n#NPC 11\nFinally try to understand.";
	answers aw;
	aw.add(1, "Accept");
	aw.add(0, "Decline");
	aw.choosebg(p);
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
		case 'I':
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
		case 'C':
			if(getmode() == show_abilities)
				setmode(0);
			else
				setmode(show_abilities);
			break;
		case 'X':
			if(getmode() == show_skills)
				setmode(0);
			else
				setmode(show_skills);
			break;
		case 'Q':
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
		case 'E':
			if(true) {
				auto pc = current_item->getowner();
				auto caster = pc->getcaster();
				auto spell_element = pc->choosespell(caster);
				if(spell_element)
					pc->cast(spell_element, caster, 0);
			}
			break;
		case 'W':
			current_item = movenext(current_item, KeyUp);
			break;
		case 'Z':
			current_item = movenext(current_item, KeyDown);
			break;
		case 'S':
			current_item = movenext(current_item, KeyRight);
			break;
		case 'A':
			current_item = movenext(current_item, KeyLeft);
			break;
		case 'P':
			place_item(current_item);
			break;
		case 'G':
			location.pickitem(current_item);
			break;
		case 'D':
			location.dropitem(current_item);
			break;
		case 'U':
			if(creature::use(current_item))
				game.endround();
			break;
		case 'T':
			game.thrown(current_item);
			break;
		case 'M':
			if(game.manipulate(current_item, to(game.getdirection(), Up)))
				game.endround();
			break;
		case 'V':
			location.automap(true);
			break;
		case Ctrl + 'V':
			game.worldmap();
			break;
		case 'F':
			draw::animation::thrown(game.getcamera(), game.getdirection(), Arrow, Left, 50);
			break;
		case 'H':
			test_map();
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
			pc = party[hot::key - '1'];
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
		case 'U':
			breakmodal(elements.data[current_element].id);
			break;
		case KeyDown:
		case 'Z':
			current_element++;
			break;
		case KeyUp:
		case 'W':
			current_element--;
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
			if(true) {
				auto id = hot::key - '1';
				if(id < (int)elements.count)
					breakmodal(elements.data[id].id);
			}
			break;
		}
	}
	return getresult();
}

static int getbuttonwidth(const char* title) {
	return textw(title) + 6;
}

static bool buttonx(int& x, int& y, int width, const char* title, void* ev, unsigned key) {
	draw::state push;
	auto vertical = true;
	if(width == -1) {
		vertical = false;
		width = getbuttonwidth(title);
	}
	auto run = false;
	rect rc = {x, y, x + width, y + texth() + 3};
	focusing(rc, ev);
	auto isfocused = isfocus(ev);
	if((isfocused && hot::key == KeyEnter) || (key && hot::key == key))
		focus_pressed = ev;
	else if(hot::key == InputKeyUp && focus_pressed == ev) {
		focus_pressed = 0;
		run = true;
	}
	form(rc, 1, false, focus_pressed == ev);
	if(isfocused)
		fore = colors::focus;
	draw::setclip(rc);
	textb(rc.x1 + 4, rc.y1 + 2, title);
	if(vertical)
		y += rc.height();
	else
		x += rc.width() + 2;
	return run;
}

static int buttonw(int x, int y, const char* title, void* ev, unsigned key = 0, callback proc = 0, int param = 0) {
	auto x1 = x;
	if(buttonx(x, y, -1, title, ev, key)) {
		if(!proc)
			proc = buttonparam;
		execute(proc, param);
	}
	return x - x1;
}

int answers::choosebg(const char* title, const imagei& ei, bool horizontal_buttons) const {
	draw::animation::render(0);
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	openform();
	while(ismodal()) {
		screen.restore();
		rect rc = {0, 121, 319, 199};
		fore = colors::white;
		if(ei)
			last_image = ei;
		if(last_image) {
			image(0, 0, gres(BORDER), 0, 0);
			image(8, 8, gres(last_image.res), last_image.frame, 0);
		}
		form(rc);
		rc.offset(6, 4);
		rc.y1 += text(rc, title, AlignLeft) + 2;
		auto x = rc.x1, y = rc.y1;
		if(horizontal_buttons)
			y = getheight() - texth() - 6;
		for(unsigned i = 0; i < elements.count; i++) {
			if(horizontal_buttons) {
				auto pn = elements.data[i].text;
				auto wd = getbuttonwidth(pn);
				if(x + wd >= rc.x2) {
					y -= texth() + 6;
					x = rc.x1;
				}
				x += buttonw(x, y, elements.data[i].text, (void*)&elements.data[i], '1' + i, 0, (int)&elements.data[i]);
			}
			else {
				buttonw(x, y, elements.data[i].text, (void*)&elements.data[i], '1' + i, 0, (int)&elements.data[i]);
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

static void nextpage() {
	auto p = (parami*)hot::param;
	p->origin += p->perpage;
	p->correct();
	if(!isfocus(nextpage) || (p->origin + p->perpage >= p->maximum))
		setfocus(0, 0);
}

static void prevpage() {
	auto p = (parami*)hot::param;
	p->origin -= p->perpage;
	p->correct();
	if(!isfocus(prevpage) || !p->origin)
		setfocus(0, 0);
}

item* itema::choose(const char* title, bool cancel_button, fngetname panel) {
	parami param = {};
	param.maximum = getcount();
	char temp[260]; stringbuilder sb(temp);
	draw::animation::render(0);
	draw::screenshoot screen;
	draw::state push;
	setsmallfont();
	fore = colors::white;
	openform();
	while(ismodal()) {
		screen.restore();
		rect rc = {0, 0, 180, 199};
		rect r1 = {181, 162, 319, 199};
		form(rc);
		form(r1);
		rc.offset(10, 4);
		rc.y1 += text(rc, title, AlignLeft) + 2;
		auto x = rc.x1 - 4, y = rc.y1;
		auto x2 = rc.x2 + 4;
		unsigned i = param.origin;
		while(i < count) {
			sb.clear(); data[i]->getname(sb);
			if(buttonx(x, y, x2 - x, temp, data[i], 0))
				execute(buttonparam, (int)data[i]);
			y += 2;
			if(y >= 180)
				break;
			i++;
		}
		if(panel) {
			auto pi = (item*)getfocus();
			if(indexof(pi) != -1) {
				sb.clear();
				auto p = panel(pi, sb);
				if(sb) {
					r1.offset(4, 4);
					text(r1, p);
				}
			}
		}
		param.perpage = i - param.origin;
		y = 200 - texth() - 6;
		if((param.origin + param.perpage) < param.maximum)
			x += buttonw(x, y, "Next", nextpage, KeyPageDown, nextpage, (int)&param);
		if(param.origin > 0)
			x += buttonw(x, y, "Prev", prevpage, KeyPageUp, prevpage, (int)&param);
		if(cancel_button)
			x += buttonw(x, y, "Cancel", buttoncancel, KeyEscape, buttoncancel, 0);
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
		x += buttonw(x, y, "Cancel", buttoncancel, KeyEscape, buttoncancel);
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

static void choose_item() {
	breakmodal(hot::param);
}

static void edit_item() {
	draw::edit("Element", (void*)hot::param, current_markup, false);
}

static void setparam() {
	current_param = hot::param;
}

static void* choose_element(const char* title, const void* current_value, int width, void** data, unsigned data_count, fntext getname, fndraw preview, const markup* type, array* source) {
	openform();
	auto columns = 2;
	if(preview)
		columns = 1;
	parami params = {};
	params.maximum = data_count;
	params.perpage = ((200 - 6 - 16 - 6) / (texth() + 6))*columns;
	auto found_current = false;
	for(unsigned i = 0; i < data_count; i++) {
		if(data[i] == current_value) {
			found_current = true;
			params.origin = i;
		}
	}
	if(!found_current)
		current_value = 0;
	params.correct();
	setfocus((void*)current_value);
	while(ismodal()) {
		form({0, 0, 320, 200}, 2);
		auto x = 4, y = 6;
		y += headerc(x, y, "Choose", title, 0, (params.origin + params.perpage - 1) / params.perpage, (params.maximum + params.perpage - 1) / params.perpage);
		auto y1 = y;
		void* current_element = 0;
		for(auto i = params.origin; i < params.maximum; i++) {
			char temp[260]; stringbuilder sb(temp);
			auto pt = data[i];
			auto pn = getname(pt, sb);
			if(!pn)
				pn = "None";
			if(buttonx(x, y, width, pn, pt, 0)) {
				if(type) {
					current_markup = type;
					draw::execute(edit_item, (int)pt);
				} else
					draw::execute(choose_item, (int)pt);
			}
			y += 2;
			if(isfocus(pt))
				current_element = pt;
			if(y >= 200 - 16 * 2) {
				if(preview || x > 4)
					break;
				x += width + 4;
				y = y1;
			}
		}
		if(preview && current_element) {
			x += width + 4;
			preview(x, y1, width, current_element);
		}
		x = 4; y = 200 - 12;
		x += buttonw(x, y, "Cancel", buttoncancel, KeyEscape);
		if(params.origin + params.perpage < params.maximum)
			x += buttonw(x, y, "Next", nextpage, KeyPageDown, nextpage, (int)&params);
		if(params.origin > 0)
			x += buttonw(x, y, "Prev", prevpage, KeyPageUp, prevpage, (int)&params);
		if(source)
			x += buttonw(x, y, "Add", "Add", F3, setparam, F3);
		current_param = 0;
		domodal();
		if(current_param == F3) {
			params.origin = source->getcount();
			data[params.origin] = source->add();
			hot::param = (int)data[params.origin];
			params.maximum = source->getcount();
			params.correct();
			current_markup = type;
			edit_item();
		} else
			navigate(true);
	}
	closeform();
	return (void*)getresult();
}

void* draw::choose(array& source, const char* title, void* object, const void* current, fntext pgetname, fnallow pallow, fndraw preview, int view_width, const markup* type) {
	if(!view_width)
		view_width = 154;
	rowa rows; getrows(source, object, rows, pallow, pgetname);
	if(type)
		return choose_element(title, current, view_width, rows.data, rows.count, pgetname, preview, type, &source);
	else
		return choose_element(title, current, view_width, rows.data, rows.count, pgetname, preview, 0, 0);
}

bool draw::choose(array& source, const char* title, void* object, void* field, unsigned field_size, const fnlist& list) {
	if(list.choose)
		return list.choose(object, source, field);
	auto current_value = (void*)getvalue(field, field_size);
	if(field_size < sizeof(int))
		current_value = source.ptr((int)current_value);
	auto result = choose(source, title, object, current_value, list.getname, list.match, list.preview, list.view_width);
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
	sb.add(v->getname());
	if(!draw::edit("Text", &current_text, dginf<meta_decoy<decltype(current_text)>::value>::meta, true))
		return false;
	v->setname(current_text.data);
	return true;
}

bool textable::editrich(void* object, const array& source, void* pointer) {
	auto p = (textable*)pointer;
	if(!p)
		return false;
	richtexti tr;
	tr.load(p->getname());
	if(!draw::edit(current_markup->title, &tr, dginf<richtexti>::meta, true))
		return false;
	tr.save(*p);
	return true;
}

static void delete_symbol() {
	auto p = (char*)current_markup->value.ptr(current_object);
	auto s = current_markup->value.size;
	p[0] = 0;
}

static void remove_symbol(char* p, unsigned i, unsigned m) {
	memmove(p + i, p + i + 1, m - i + 1);
}

static void insert_symbol(char* p, unsigned i, unsigned m, char sym) {
	memmove(p + i + 1, p + i, m - i);
	p[i] = sym;
}

static void del_symbol() {
	auto p = (char*)current_markup->value.ptr(current_object);
	remove_symbol(p, current_c1, zlen(p));
}

static void add_symbol(char* p, unsigned size, int k) {
	auto m = zlen(p);
	if(current_c1 > m)
		current_c1 = m;
	if(k == 8) {
		if(current_c1 > 0)
			remove_symbol(p, --current_c1, m);
	} else if(k >= ' ' && k <= 128) {
		if(current_c1 < (int)(size - 1))
			insert_symbol(p, current_c1++, m, k);
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
	if(!current_markup->value.size) {
		if(!current_markup->value.source)
			return;
		choose(*current_markup->value.source, current_markup->title,
			current_object, 0, current_markup->list.getname, current_markup->list.match, current_markup->list.preview, current_markup->list.view_width,
			current_markup->value.type);
	} else if(!current_markup->value.source) {
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
	if(!e.value.size) {
		if(e.value.source)
			sb.add("%1i of %2i", e.value.source->getcount(), e.value.source->getmaximum());
		else
			sb.add(e.title);
	} else {
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

static void move_cursor() {
	auto n = hot::param;
	current_c1 += n;
}

static void set_cursor() {
	current_c1 = hot::param;
}

static void clear_value() {
	if(current_markup->value.size <= sizeof(int))
		setvalue(current_markup->value.ptr(current_object), current_markup->value.size, 0);
	else
		memset(current_markup->value.ptr(current_object), 0, current_markup->value.size);
}

static void post(const markup& e, void* object, callback proc, int param) {
	current_markup = &e;
	current_object = object;
	execute(proc, param);
}

static int field(const rect& rco, const char* title, void* object, const markup& e, unsigned flags) {
	auto rich_edit = rco.height() >= texth() * 2;
	auto isnum = e.value.isnum() && e.value.size <= sizeof(int);
	auto push_fore = draw::fore;
	auto zero_string = false;
	draw::fore = colors::white;
	if(!title || title[0] == 0) {
		if(rich_edit) {
			draw::fore = draw::fore.mix(colors::gray, 128);
			title = e.title;
		} else
			title = "None";
		zero_string = true;
	}
	auto pv = e.value.ptr(object);
	auto pf = pv;
	if(!e.value.size)
		pf = (void*)&e;
	if(rich_edit)
		form(rco, 1, false, true);
	else
		form(rco);
	focusing(rco, pf);
	auto rc = rco; rc.offset(4, 2);
	auto focused = isfocus(pf);
	auto islist = e.list.choose || !e.value.isnum();
	if(focused) {
		if(current_edit != pv) {
			if(zero_string)
				current_c1 = 0;
			else
				current_c1 = zlen(title);
			current_edit = pv;
		}
		if(islist || isnum)
			fore = colors::focus;
		switch(hot::key) {
		case InputSymbol:
			if(!islist && !isnum)
				post(e, object, add_symbol, hot::param);
			break;
		case KeyEnter:
			if(islist)
				post(e, object, choose_enum_field, 0);
			break;
		case KeyDelete:
			if(islist || isnum)
				post(e, object, clear_value, 0);
			else
				post(e, object, del_symbol, 0);
			break;
		case KeyLeft:
			if(!islist && !isnum && current_c1 > 0)
				post(e, object, move_cursor, -1);
			break;
		case KeyRight:
			if(!islist && !isnum) {
				auto m = zlen(title);
				if(current_c1 < m)
					post(e, object, move_cursor, 1);
			}
			break;
		case KeyHome:
			if(!islist && !isnum)
				post(e, object, set_cursor, 0);
			break;
		case KeyEnd:
			if(!islist && !isnum) {
				auto m = zlen(title);
				post(e, object, set_cursor, m);
			}
			break;
		case '0': case '1':case '2': case '3':case '4':
		case '5':case '6': case '7':case '8': case '9':
			if(isnum && !islist) {
				current_object = pv;
				current_size = e.value.size;
				execute(add_number, hot::key - '0');
			}
			break;
		case '-':
			if(isnum && !islist) {
				current_object = pv;
				current_size = e.value.size;
				execute(decrement);
			}
			break;
		case '+':
			if(isnum && !islist) {
				current_object = pv;
				current_size = e.value.size;
				execute(increment);
			}
			break;
		case KeyBackspace:
			if(isnum && !islist) {
				current_object = pv;
				current_size = e.value.size;
				execute(sub_number);
			}
			break;
		case '*':
			if(isnum && !islist) {
				current_object = pv;
				current_size = e.value.size;
				execute(inverse);
			}
			break;
		}
	}
	if(!islist && !isnum && focused) {
		textbi(rc, title, flags, current_c1, current_p1);
		if((frametick / 10) % 2)
			line(current_p1.x, current_p1.y - 1, current_p1.x, current_p1.y + texth());
	} else
		textb(rc, title, flags);
	draw::fore = push_fore;
	return rco.height() + 2;
}

static int field(int x, int y, int width, const char* title, void* object, int title_width, const markup& e) {
	char temp[260]; stringbuilder sb(temp);
	if(e.proc.getheader)
		title = e.proc.getheader(object, sb);
	if(title && title[0]) {
		textb(x + 6, y + 2, title);
		x += title_width;
		width -= title_width;
	}
	sb.clear(); getname(e, object, sb);
	return field({x, y, x + width, y + draw::texth() + 3}, temp, object, e, TextSingleLine);
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
	getrows(*ar, object, storage, e.list.match, e.list.getname);
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
			y += element(x, y, width, ctx, *f);
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
		else if(e.value.isnum() && e.value.size > 16)
			return field({x, y, x + width, y + draw::texth() * 6 + 4}, (const char*)e.value.ptr(ctx.object), ctx.object, e, 0);
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
			else if(pm->is("div"))
				y += group(x, y, width, *this, pm + 1);
			else if(pm->ispage()) {
				contexti ctx = *this;
				ctx.object = pm->value.ptr(object);
				y += group(x, y, width, ctx, pm->value.type);
			} else
				y += group(x, y, width, *this, pm);
			// Footer
			x = 4; y = 200 - 12;
			if(cancel_button) {
				x += buttonw(x, y, "Cancel", buttoncancel, KeyEscape, buttoncancel);
				x += buttonw(x, y, "OK", buttonok, KeyEnter, buttonok);
			} else
				x += buttonw(x, y, "OK", buttonok, KeyEscape, buttonok);
			if(page > 0)
				x += buttonw(x, y, "Prev", prev_page, KeyPageUp, prev_page, (int)this);
			if(page < page_maximum - 1)
				x += buttonw(x, y, "Next", next_page, KeyPageDown, next_page, (int)this);
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

void random_heroes();

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

point draw::choosepoint(point camera) {
	state push;
	openform();
	while(ismodal()) {
		point origin;
		auto pr = gres(WORLD);
		correct(camera, 0, 0, pr->frames[0].sx, pr->frames[0].sy);
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
	auto gx = game.getmapwidth();
	auto gy = game.getmapheight();
	auto sx = draw::getwidth();
	auto sy = draw::getheight();
	auto mx = gx - sx;
	auto my = gy - sy;
	if(mx < 0 && my < 0) {
		rectf({0, 0, sx, sy}, colors::black);
		draw::image(-mx / 2, -my / 2, gres(WORLD), 0, 0);
		if(origin)
			*origin = {0, 0};
	} else {
		camera.x -= sx / 2;
		camera.y -= sy / 2;
		correct(camera, 0, 0, mx, my);
		draw::image(-camera.x, -camera.y, gres(WORLD), 0, 0);
		if(origin)
			*origin = camera;
	}
}

void draw::fullimage(point from, point to, point* origin) {
	auto sx = game.getmapwidth();
	auto sy = game.getmapheight();
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
	fullimage(camera, origin);
	redraw();
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

int	gamei::getmapheight() {
	auto p = gres(WORLD);
	if(!p)
		return 0;
	return p->frames[0].sx;
}

int	gamei::getmapwidth() {
	auto p = gres(WORLD);
	if(!p)
		return 0;
	return p->frames[0].sy;
}