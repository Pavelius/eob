#include "main.h"
#include "draw.h"

using namespace draw;

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
	const contexti*		parent;
	int					title;
	const char*			header;
	const char*	getheader(const markup& e) const {
		if(e.title)
			return e.title;
		return header;
	}
	constexpr contexti(void* object) : object(object), parent(0), title(84), header(0) {}
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
static imagei			current_image;
static int				current_level;
static creature*		current_hero;
static item*			drag_item;
static char				log_message[128];
static rect				log_rect = {5, 180, 285, 198};
static int				focus_level;
static const void*		focus_stack[8];
static const void*		focus_pressed;
extern "C" void			scale3x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height);
callback				draw::domodal;
static textedit			current_text;
static void*			current_edit;
static int				current_c1;
static point			current_p1;
static int				current_portrait;
static creature*		current_player;
const int				dx = 4;
const int				menu_width = 168;
static callback			next_proc;
extern sprite*			gres(resource_s id);

bool draw::isallowmodal() {
	return next_proc == 0;
}

void draw::application() {
	while(next_proc) {
		auto p = next_proc;
		next_proc = 0; p();
	}
}

void draw::setnext(void(*v)()) {
	if(!next_proc)
		next_proc = v;
}

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
	if(flags&TextBold) {
		auto push_fore = fore;
		fore = colors::black;
		glyph(x + 1, y + 1, sym, 0);
		fore = push_fore;
	}
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

static int flatb(int x, int y, int width, unsigned flags, const char* string) {
	int height = draw::texth() + 1;
	if(flags&Focused)
		rectf({x, y, x + width, y + height + 1}, colors::dark);
	else
		border_up({x, y, x + width, y + height});
	draw::text(x + 1 + (width - draw::textw(string)) / 2, y + 1, string);
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
	draw::text(x, y, text, -1, TextBold);
	return draw::texth() + 2;
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
	auto pc = creature::get(pitm);
	auto pid = game.getwear(pitm);
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
	auto sp = gres(id);
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
			draw::image(x0, y0, gres(BLUE), 0, 0);
			draw::image(x0, y1, sp, 0, 0);
		} else if(p->isdungeon())
			dungeon_preview(x0, y0, sp);
		else if(p->pack == Pack160x96) {
			draw::image(x + 1, y + 1, gres(BORDER), 0, 0);
			draw::image(x + 1 + 8, y + 1 + 8, sp, current_res_frame, 0);
		} else if(p->pack == PackOuttake) {
			dungeon_corner(x0, y0, gres(BLUE));
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
	text(x, y1, temp, -1, TextBold); y1 += texth();
	if(sp) {
		if(ei.choose_frame) {
			sb.clear(); sb.add("Sprite: %1i of %2i", current_res_frame, sp->count);
			text(x, y1, temp, -1, TextBold); y1 += texth();
		}
	}
	if(p->ismonster()) {
		auto overlays = (sp->count / 6) - 1;
		if(overlays) {
			sb.clear(); sb.add("Number of overlays: %1i", overlays);
			text(x, y1, temp, -1, TextBold); y1 += texth();
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
		object, &ei, getnm<resourcei>, current_markup->list.match, resourcei::preview, 320 - 190);
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

static void show_worldmap() {
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
	current_param = 0;
	if(next_proc) {
		break_modal = false;
		return false;
	}
	if(!break_modal)
		return true;
	break_modal = false;
	return false;
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
		case '1': case '2': case '3': case '4': case '5': case '6':
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

static bool labelt(int& x, int& y, int width, const char* title, void* ev, unsigned key) {
	state push;
	auto vertical = true;
	if(width == -1) {
		vertical = false;
		width = getbuttonwidth(title);
	}
	auto run = false;
	rect rc = {x, y, x + width, y + texth()};
	focusing(rc, ev);
	if(isfocus(ev)) {
		fore = colors::focus;
		if(hot::key == KeyEnter)
			run = true;
	}
	if(key && hot::key == key)
		run = true;
	text(x, y, title, -1, TextBold);
	if(vertical)
		y += texth();
	else
		x += width;
	return run;
}

static bool labelm(int x, int& y, int width, const char* title, void* ev, unsigned key) {
	state push;
	auto run = false;
	rect rc = {x, y, x + width, y + texth()};
	focusing(rc, ev);
	if(isfocus(ev)) {
		fore = colors::focus;
		if(hot::key == KeyEnter)
			run = true;
	}
	if(key && hot::key == key)
		run = true;
	text(aligned(x, width, AlignCenter, textw(title)), y, title, -1, TextBold);
	y += texth();
	return run;
}

static bool labelxm(const rect& rc, const char* title, unsigned key) {
	draw::state push;
	auto run = false;
	auto ev = (void*)title;
	focusing(rc, ev);
	auto isfocused = isfocus(ev);
	if((isfocused && hot::key == KeyEnter) || (key && hot::key == key))
		focus_pressed = ev;
	else if(hot::key == InputKeyUp && focus_pressed == ev) {
		focus_pressed = 0;
		run = true;
	}
	draw::setclip(rc);
	if(isfocused) {
		fore = colors::focus.mix(fore, 128);
		text(rc, title, AlignLeft | TextBold);
	} else {
		fore = colors::white.mix(colors::main, 64);
		text(rc, title, AlignLeft);
	}
	return run;
}

static bool labelxm(int x, int& y, int width, const char* title, unsigned key) {
	rect rc = {x, y, x + width, y};
	textw(rc, title);
	y += rc.height() + 1;
	return labelxm(rc, title, key);
}

static bool buttonx(int& x, int& y, int width, const char* title, void* ev, unsigned key, int height = -1) {
	draw::state push;
	auto vertical = true;
	if(width == -1) {
		vertical = false;
		width = getbuttonwidth(title);
	}
	if(height == -1)
		height = texth();
	if(!ev)
		ev = (void*)title;
	auto run = false;
	rect rc = {x, y, x + width, y + height + 3};
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
	text(rc.x1 + 4, rc.y1 + 2, title, -1, TextBold);
	if(vertical)
		y += height + 3;
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

void draw::dlgmsg(const char* text) {
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
		if(buttonx(x1, y1, 32, "OK", (void*)"OK", 0))
			execute(buttonok);
		domodal();
		navigate();
	}
	closeform();
}

bool draw::dlgask(const char* text) {
	draw::state push;
	draw::screenshoot screen(true);
	setsmallfont();
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
		x1 = (320 - (6 * 3 + 6 * 3 + 2 + 6 * 2)) / 2;
		if(buttonx(x1, y1, -1, "Yes", (void*)"Yes", KeyEnter))
			execute(buttonok);
		if(buttonx(x1, y1, -1, "No", (void*)"No", KeyEscape))
			execute(buttoncancel);
		domodal();
		navigate();
	}
	closeform();
	return getresult() != 0;
}

void answers::clearimage() {
	last_image = {};
}

int answers::choosehz(const char* title) const {
	draw::animation::render(0);
	draw::state push;
	setsmallfont();
	openform();
	while(ismodal()) {
		draw::animation::render(0, true, 0, 0);
		rect rc = {0, 121, 319, 199};
		fore = colors::white;
		if(last_image) {
			image(0, 0, gres(BORDER), 0, 0);
			image(8, 8, gres(last_image.res), last_image.frame, 0);
		}
		form(rc);
		rc.offset(6, 4);
		rc.y1 += text(rc, title, AlignLeft) + 2;
		auto x = rc.x1, y = rc.y1;
		y = getheight() - texth() - 6;
		for(unsigned i = 0; i < elements.count; i++) {
			auto pn = elements.data[i].text;
			auto wd = getbuttonwidth(pn);
			if(x + wd >= rc.x2) {
				y -= texth() + 6;
				x = rc.x1;
			}
			x += buttonw(x, y, elements.data[i].text, (void*)&elements.data[i], '1' + i, 0, (int)&elements.data[i]);
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

item* itema::choose(const char* title, bool cancel_button, fntext panel) {
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

static int headerc(int x, int y, const char* prefix, const char* title, const char* subtitle, int page, int page_maximum) {
	if(!title)
		return 0;
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1 %2", prefix, title);
	if(subtitle && subtitle[0])
		sb.adds(subtitle);
	auto fore_push = fore;
	fore = colors::title;
	text(6, 6, temp, -1, TextBold);
	if(page_maximum > 1) {
		sb.clear();
		sb.add("Page %1i of %2i", page + 1, page_maximum);
		text(draw::getwidth() - 6 - textw(temp), 6, temp, -1, TextBold);
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
			x += buttonw(x, y, "Add", (void*)"Add", F3, setparam, F3);
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
	auto current_value = (void*)markup::get(field, field_size);
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
	markup::set(field, field_size, (int)current_value);
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

static void add_number(void* p, unsigned size, int r, int d, int v) {
	markup::set(p, size, markup::get(p, size) * r / d + v);
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
		markup::set(current_markup->value.ptr(current_object), current_markup->value.size, 0);
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
	auto rc = rco;
	rc.x1 += 4; rc.x2 -= 4;
	rc.y1 += 2; rc.y2 -= 1;
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
		texti(rc, title, flags, current_c1, current_p1);
		if((frametick / 10) % 2)
			line(current_p1.x, current_p1.y - 1, current_p1.x, current_p1.y + texth());
	} else
		text(rc, title, flags);
	draw::fore = push_fore;
	return rco.height() + 2;
}

static int field(int x, int y, int width, const char* title, void* object, int title_width, const markup& e) {
	char temp[260]; stringbuilder sb(temp);
	if(e.proc.getheader)
		title = e.proc.getheader(object, sb);
	if(title && title[0]) {
		text(x + 6, y + 2, title, -1, TextBold);
		x += title_width;
		width -= title_width;
	}
	sb.clear(); e.getname(object, sb);
	return field({x, y, x + width, y + draw::texth() + 3}, temp, object, e, TextSingleLine | TextBold);
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
	auto v = markup::get(pv, size);
	v ^= mask;
	markup::set(pv, size, v);
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
	auto s = ((markup::get(pv, e.value.size) & mask) != 0) ? 1 : 0;
	checkmark(rc.x1, rc.y1, s);
	rc.x1 += cw;
	text(rc, title, flags | TextSingleLine | TextBold);
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
			c1.parent = &ctx;
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
		if(hot::key == KeyEscape
			|| hot::key == KeySpace)
			break;
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

static int get_spells_prepared(creature* pc, aref<spell_s> spells) {
	int result = 0;
	for(auto e : spells)
		result += pc->getprepare(e);
	return result;
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

static void select_known_spells(adat<spell_s>& result, creature* pc, class_s type, int level) {
	result.clear();
	for(auto i = spell_s(1); i < LayOnHands; i = (spell_s)(i + 1)) {
		if(creature::getlevel(i, type) != level)
			continue;
		if(!pc->isknown(i))
			continue;
		result.add(i);
	}
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
		case 'U':
			breakmodal(result.data[current_element].value);
			break;
		case KeyLeft:
		case 'A':
			current_level--;
			if(current_level < 1)
				current_level = 1;
			break;
		case KeyRight:
		case 'S':
			current_level++;
			if(current_level > 9)
				current_level = 9;
			break;
		case KeyDown:
		case 'Z':
			current_element++;
			break;
		case KeyUp:
		case 'W':
			current_element--;
			break;
		case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			current_level = hot::key - '1' + 1;
			break;
		}
	}
	return getresult();
}

static void avatar(int x, int y, int i, const creature* current, const creaturea* allowed, creature** change, callback proc) {
	if(i >= party.getcount())
		return;
	auto p = party[i];
	if(!p)
		return;
	unsigned flags = 0;
	if(allowed && allowed->indexof(p) == -1)
		flags |= Disabled;
	if(p == current)
		flags |= Checked;
	else if((flags&Disabled) == 0) {
		if(allowed && change && hot::key == ('1' + i)) {
			auto new_index = allowed->indexof(p);
			if(new_index != -1)
				*change = allowed->data[new_index];
			if(proc)
				execute(proc, 0);
		}
	}
	draw::avatar(x, y, p, flags, 0);
}

static void avatars(int x, int y, const creature* pc, const creaturea* allowed, creature** change, callback proc) {
	avatar(x, y, 0, pc, allowed, change, proc);
	avatar(x + 72, y, 1, pc, allowed, change, proc);
	avatar(x, y + 52, 2, pc, allowed, change, proc);
	avatar(x + 72, y + 52, 3, pc, allowed, change, proc);
}

item* itema::choose(const char* format, bool* cancel_button, const creature* current, const creaturea* allowed, creature** change, fntext getname) const {
	if(cancel_button)
		*cancel_button = false;
	if(!getname)
		getname = getnm<item>;
	openform();
	while(ismodal()) {
		draw::animation::render(0, false);
		if(true) {
			draw::state push;
			setbigfont();
			form({0, 0, 22 * 8 + 2, 174}, 2);
			auto x = 6, y = 6;
			if(format) {
				fore = colors::title;
				text(6, 6, "Scrolls available:", -1, TextBold);
				y += texth() + 3;
			}
			fore = colors::white;
			for(unsigned i = 0; i < count; i++) {
				char temp[260]; stringbuilder sb(temp);
				auto pn = getname(data[i], sb);
				if(labelt(x, y, 168, pn, data[i], 0))
					execute(buttonparam, (int)data[i]);
				y += 3;
				if(y >= 156 - texth())
					break;
			}
			y = 174 - texth() - 7;
			if(cancel_button) {
				if(buttonx(x, y, -1, "Close", cancel_button, KeyEscape)) {
					execute(buttoncancel, 0);
					*cancel_button = true;
				}
			}
		}
		avatars(184, 2, current, allowed, change, buttoncancel);
		draw::domodal();
		draw::navigate(false);
	}
	closeform();
	return (item*)getresult();
}

int answers::choosemn(const char* title, bool allow_cancel) const {
	draw::state push;
	setbigfont();
	openform();
	while(ismodal()) {
		draw::animation::render(0, false);
		form({0, 0, menu_width + 10, 174}, 2);
		auto x = 6, y = 6;
		if(title) {
			fore = colors::title;
			text(x + 2, y, title, -1, TextBold);
			y += texth() + 2;
		}
		fore = colors::white;
		for(auto& e : elements) {
			if(buttonx(x, y, menu_width, e.text, (void*)e.text, 0))
				execute(buttonparam, e.id);
			y += 2;
		}
		y = 174 - texth() - 7;
		if(allow_cancel) {
			if(buttonx(x, y, -1, "Cancel", (void*)"Cancel", 0))
				execute(buttoncancel);
			y += 2;
		}
		domodal();
		navigate(true);
	}
	closeform();
	return getresult();
}

int answers::choosemn(int x0, int y0, int width, resource_s id) const {
	draw::state push;
	fore = colors::white;
	setbigfont();
	openform();
	while(ismodal()) {
		draw::background(MENU);
		auto x = x0, y = y0;
		for(auto& e : elements) {
			if(labelm(x, y, width, e.text, (void*)e.text, 0))
				execute(buttonparam, e.id);
			y += 1;
		}
		domodal();
		navigate(false);
	}
	closeform();
	return getresult();
}

char monsteri::getpallette() const {
	auto pr = gres(rfile);
	if(!pr)
		return 0;
	auto ph = pr->getheader("COL");
	if(!ph)
		return 0;
	return xrand(0, 2);
}

static void texth3(int x, int y, const char* name) {
	text(x, y, name);
}

static const char* get_race(creature* p, stringbuilder& sb) {
	sb.clear();
	sb.add(bsdata<racei>::elements[p->getrace()].name);
	sb.adds(bsdata<genderi>::elements[p->getgender()].name);
	return sb;
}

static int number(int x, int y, const char* name, int v1) {
	char temp[32]; stringbuilder sb(temp);
	texth3(x, y, name);
	sb.add("%1i", v1);
	text(x + 6 * 4, y, temp);
	return 7;
}

static int number(int x, int y, const char* name, int v1, int v2) {
	char temp[32]; stringbuilder sb(temp);
	texth3(x, y, name);
	sb.add("%1i/%2i", v1, v2);
	text(x + 6 * 4, y, temp);
	return 7;
}

static int number(int x, int y, const char* name, const combati& v) {
	char temp[64]; stringbuilder sb(temp);
	texth3(x, y, name);
	v.damage.range(sb);
	text(x + 6 * 4, y, temp);
	return 7;
}

static void sheet_head(int x, int y, creature* pc) {
	draw::state push; setsmallfont();
	image(x, y, gres(INVENT), 0, 0);
	pc->view_portrait(x + 4, y + 3);
	text({x + 38, y + 6, x + 38 + 82, y + 6 + draw::texth()}, pc->getname(), AlignCenterCenter);
	greenbar({x + 70, y + 16, x + 135, y + 16 + 5}, pc->gethits(), pc->gethitsmaximum());
	greenbar({x + 70, y + 25, x + 135, y + 25 + 5}, pc->getfood(), pc->getfoodmax());
}

static void blanksheet(int x, int y, creature* pc) {
	sheet_head(x, y, pc);
	rectf({x + 2, y + 36, 319, 166}, color::create(164, 164, 184));
	line(274, y + 35, 319, y + 35, color::create(208, 208, 216));
	line(319, 140, 319, 166, color::create(208, 208, 216));
	line(300, 166, 318, 166, color::create(88, 88, 116));
}

static void invertory(int x, int y, creature* pc, void* current_item) {
	if(!pc)
		return;
	const int dx = 18;
	const int dy = 18;
	sheet_head(x, y, pc);
	for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i + 1)) {
		int x1 = x + 11 + ((i - Backpack) % 2)*dx;
		int y1 = y + 48 + ((i - Backpack) / 2)*dy;
		itemicn(x1, y1, pc->getitem(i), true, 0, current_item);
	}
	itemicn(x + 55, y + 64, pc->getitem(Quiver), true, 0, current_item);
	itemicn(x + 128, y + 63, pc->getitem(Head), true, 0, current_item);
	itemicn(x + 117, y + 83, pc->getitem(Neck), true, 0, current_item);
	itemicn(x + 54, y + 84, pc->getitem(Body), true, 0, current_item);
	itemicn(x + 55, y + 104, pc->getitem(Elbow), true, 0, current_item);
	itemicn(x + 130, y + 102, pc->getitem(FirstBelt), true, 0, current_item);
	itemicn(x + 130, y + 120, pc->getitem(SecondBelt), true, 0, current_item);
	itemicn(x + 130, y + 138, pc->getitem(LastBelt), true, 0, current_item);
	itemicn(x + 60, y + 124, pc->getitem(RightHand), true, 0, current_item);
	itemicn(x + 54, y + 140, pc->getitem(RightRing), true, 0, current_item);
	itemicn(x + 66, y + 140, pc->getitem(LeftRing), true, 0, current_item);
	itemicn(x + 108, y + 124, pc->getitem(LeftHand), true, 0, current_item);
	itemicn(x + 107, y + 145, pc->getitem(Legs), true, 0, current_item);
}

static void abilities(int x, int y, creature* pc) {
	if(!pc)
		return;
	char temp[260]; stringbuilder sb(temp);
	blanksheet(x, y, pc);
	state push; setsmallfont();
	fore = colors::info::text;
	int x1 = x + 4;
	int y1 = y + 54;
	header(x1, y + 42, "Character info");
	text(x1, y1, bsdata<classi>::elements[pc->getclass()].name); y1 += draw::texth();
	text(x1, y1, bsdata<alignmenti>::elements[pc->getalignment()].name); y1 += draw::texth();
	text(x1, y1, get_race(pc, sb)); y1 += draw::texth() * 2;
	pc->render_ability(x1, y1, 24, false);
	combati e = {}; pc->get(e, RightHand, Medium);
	x1 = x + 7 * 10 + 4;
	y1 += number(x1, y1, "AC", 10 - pc->get(AC));
	y1 += number(x1, y1, "ATT", 20 - e.bonus);
	y1 += number(x1, y1, "DAM", e);
	y1 += number(x1, y1, "SPD", pc->getspeed());
	y1 += number(x1, y1, "HP", pc->gethits(), pc->gethitsmaximum());
	x1 = x + 4;
	y1 = y + 136;
	header(x1, y1, "Class");
	header(x1 + 6 * 7, y1, "Lev");
	header(x1 + 6 * 11, y1, "Exp");
	y1 += 8;
	auto cls = pc->getclass();
	auto exp = pc->getexperience() / bsdata<classi>::elements[cls].classes.count;
	for(int i = 0; i < 3; i++) {
		char temp[16];
		auto m = pc->getclass(cls, i);
		if(!m)
			continue;
		text(x1, y1, bsdata<classi>::elements[m].name);
		sznum(temp, pc->get(m));
		text(x1 + 6 * 8, y1, temp);
		sznum(temp, (int)exp);
		text(x1 + 6 * 11, y1, temp);
		y1 += 7;
	}
}

static void skills(int x, int y, creature* pc) {
	if(!pc)
		return;
	blanksheet(x, y, pc);
	state push; setsmallfont();
	fore = colors::info::text;
	int x1 = x + 4;
	int y1 = y + 54;
	header(x1, y + 42, "Skills");
	for(auto i = SaveVsParalization; i <= DetectSecrets; i = (ability_s)(i + 1)) {
		int value = pc->get(i);
		if(value <= 0)
			continue;
		char temp[16]; stringbuilder sb(temp);
		texth3(x1, y1, bsdata<abilityi>::elements[i].name);
		sb.add("%1i%%", value);
		text(x1 + 6 * 19, y1, temp);
		y1 += 7;
	}
}

static void prev_portrait() {
	current_portrait--;
}

static void next_portrait() {
	current_portrait++;
}

void creature::roll_character() {
	current_player->basic.random_ability(current_player->getrace(), current_player->getgender(), current_player->getclass());
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
	auto pi = gres(CHARGENB);
	auto si = 0;
	if(proc == next_portrait || proc == prev_portrait)
		si = 2;
	else if(proc == new_game && key=='P')
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
		image(x, y, gres(XSPL), (clock() / 150) % 10, 0);
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
	auto ps = gres(PORTM);
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

static int number(int x, int y, int w, const char* title, const char* v, unsigned flags) {
	text(x, y, title, -1, flags);
	text(x + w, y, v, -1, flags);
	return draw::texth() + 1;
}

static int number(int x, int y, int w, const char* title, int v, unsigned flags) {
	char temp[32];
	sznum(temp, v);
	return number(x, y, w, title, temp, flags);
}

static int number(int x, int y, int w, const char* title, const dice& v, unsigned flags) {
	char temp[32]; stringbuilder sb(temp);
	v.range(sb);
	return number(x, y, w, title, temp, flags);
}

static int number(int x, int y, int w, const char* title, int v1, int v2, const char* format, unsigned flags) {
	char temp[32]; stringbuilder sb(temp);
	sb.add(format, v1, v2);
	return number(x, y, w, title, temp, flags);
}

int creature::render_ability(int x, int y, int width, unsigned flags) const {
	auto y0 = y;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1)) {
		auto v = get(i);
		auto emn = bsdata<abilityi>::elements[i].name;
		if(i == Strenght && v == 18 && ability[ExeptionalStrenght] > 0) {
			if(ability[ExeptionalStrenght] == 100)
				y += number(x, y, width, emn, 18, 0, "%1i/00", flags);
			else
				y += number(x, y, width, emn, 18, ability[ExeptionalStrenght], "%1i/%2.2i", flags);
		} else
			y += number(x, y, width, emn, get(i), flags);
	}
	return y - y0;
}

int creature::render_combat(int x, int y, int width, unsigned flags) const {
	auto y0 = y;
	combati ai = {}; get(ai, RightHand, Medium);
	y += number(x, y, width, "AC", 10 - get(AC), flags);
	y += number(x, y, width, "ATT", 20 - ai.bonus, flags);
	y += number(x, y, width, "DAM", ai.damage, flags);
	y += number(x, y, width, "HP", gethits(), gethitsmaximum(), "%1i/%2i", flags);
	return y - y0;
}

void creature::view_ability() {
	adat<short unsigned, 256> source;
	source.count = game.getavatar(source.data, getrace(), getgender(), type);
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
		zprint(temp, "%1 %2", bsdata<racei>::elements[getrace()].name, bsdata<genderi>::elements[getgender()].name);
		text(x + (width - draw::textw(temp)) / 2, y, temp, -1, TextBold); y += draw::texth() + 2;
		zprint(temp, bsdata<classi>::elements[type].name);
		text(x + (width - draw::textw(temp)) / 2, y, temp, -1, TextBold); y += draw::texth() + 2;
		render_ability(148, 128, 32, TextBold);
		render_combat(224, 128, 32, TextBold);
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
			source.add(i, bsdata<genderi>::elements[i].name);
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
		source.add(i, bsdata<alignmenti>::elements[i].name);
	}
	return (alignment_s)source.choose("Select Alignment:", interactive);
}

race_s creature::chooserace(bool interactive) {
	if(interactive) {
		answers source;
		for(auto i = Dwarf; i <= Human; i = (race_s)(i + 1))
			source.add(i, bsdata<racei>::elements[i].name);
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
		source.add(i, bsdata<classi>::elements[i].name);
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
	char temp[260]; stringbuilder sb(temp);
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
		text(x + (width - draw::textw(pn)) / 2, y, pn, -1, TextBold); y += draw::texth() + 1;
		sb.clear(); sb.add("%1 %2", bsdata<racei>::elements[race].name, bsdata<genderi>::elements[gender].name);
		text(x + (width - draw::textw(temp)) / 2, y, temp, -1, TextBold); y += draw::texth() + 1;
		sb.clear(); sb.add(bsdata<classi>::elements[type].name);
		text(x + (width - draw::textw(temp)) / 2, y, temp, -1, TextBold); y += draw::texth() + 1;
		current_player->render_ability(148, 128, 32, TextBold);
		current_player->render_combat(224, 128, 32, TextBold);
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
			y += header(148, 68, title_string);
		for(auto& e : elements) {
			if(labelt(x, y, 128, e.text, (void*)&e, 0))
				execute(buttonparam, e.id);
		}
		domodal();
		navigate();
	}
	closeform();
	return getresult();
}

void creature::view_party() {
	char temp[1024];
	draw::state push;
	fore = colors::white;
	setbigfont();
	openform();
	while(ismodal()) {
		genheader(change_character);
		rect rc = {150, 74, 296, 184};
		stringbuilder sb(temp);
		sb.add("Select the box of the character you wish to create or view.");
		if(is_party_created()) {
			sb.add("\n\n");
			sb.add("Your party is complete. Select PLAY button or press 'P' to start the game.");
			buttonx(25, 181, 0, 'P', new_game, 0);
		}
		text(rc, temp, TextBold);
		domodal();
		navigate();
	}
	closeform();
}

void creature::create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive) {
	clear();
	nameable::set(gender);
	nameable::set(race);
	set(type);
	set(alignment);
	basic.random_ability(race, gender, type);
	setavatar(game.getavatar(race, gender, getclass(type, 0)));
	if(interactive)
		view_ability();
	else
		finish();
	random_name();
}

void creature::preparespells(class_s type) {
	draw::animation::render(0, false);
	draw::state push;
	char temp[260]; stringbuilder sb(temp);
	creaturea source;
	for(auto p : party) {
		if(p && p->iscast(type))
			source.add(p);
	}
	if(!source) {
		sb.clear();
		sb.add("You don't have any %1 in party.", bsdata<classi>::elements[type].name);
		dlgmsg(temp);
		return;
	}
	adat<spell_s> result;
	auto pc = source[0];
	openform();
	setbigfont();
	while(ismodal()) {
		result.clear();
		auto maximum_spells = 0;
		auto prepared_spells = 0;
		if((int)getfocus() >= 1 && (int)getfocus() <= 9)
			current_level = (int)getfocus();
		if(pc) {
			select_known_spells(result, pc, type, current_level);
			maximum_spells = pc->getspellsperlevel(type, current_level);
			prepared_spells = get_spells_prepared(pc, {result.data, result.count});
		}
		auto level = pc ? pc->get(type) : 0;
		form({0, 0, menu_width + 10, 174}, 2);
		fore = colors::title;
		auto x = 6, y = 6;
		text(x + 2, y, "Spells available:", -1, TextBold);
		y += texth() + 2;
		fore = colors::white;
		auto x1 = x;
		for(int i = 0; i < 9; i++)
			buttonx(x1, y, -1, sznum(temp, i + 1), (void*)(i + 1), 0);
		y += texth() + 8;
		sb.clear();
		sb.add("%1i of %2i remaining", prepared_spells, maximum_spells);
		fore = colors::title;
		text(x + 2, y, temp, -1, TextBold);
		y += texth() + 2;
		fore = colors::white;
		auto ym = 174 - texth() * 2 - 7 - 6;
		for(auto& e : result) {
			sznum(temp, pc->getprepare(e));
			text(aligned(x, menu_width, AlignRight, textw(temp)), y, temp, -1, TextBold);
			labelt(x, y, menu_width - 8 * 2, bsdata<spelli>::elements[e].name, &e, 0);
			if(y >= ym)
				break;
		}
		y = 174 - texth() - 7;
		if(buttonx(x, y, -1, "Close", 0, 0))
			execute(buttoncancel);
		if(buttonx(x, y, -1, "Clear", 0, 0))
			execute(setparam, KeyDelete);
		avatars(184, 2, pc, &source, &pc, 0);
		domodal();
		auto current_index = result.indexof((spell_s*)getfocus());
		switch(current_param) {
		case KeyDelete:
			for(auto e : result)
				pc->set(e, 0);
			break;
		}
		switch(hot::key) {
		case KeyRight:
			if(current_index != -1) {
				if(prepared_spells < maximum_spells)
					pc->setprepare(result.data[current_index],
						pc->getprepare(result.data[current_index]) + 1);
				continue;
			}
			break;
		case KeyLeft:
			if(current_index != -1) {
				auto c = pc->getprepare(result.data[current_index]);
				if(c)
					pc->setprepare(result.data[current_index], c - 1);
				continue;
			}
			break;
		}
		navigate(true);
	}
	closeform();
}

static void show_invertory(void* current_item) {
	invertory(178, 0, creature::get(current_item), current_item);
}

static void show_abilities(void* current_item) {
	abilities(178, 0, creature::get(current_item));
}

static void show_skills(void* current_item) {
	skills(178, 0, creature::get(current_item));
}

static bool handle_shortcuts(bool allow_move) {
	auto pc = creature::get(current_focus);
	item* current_item = (item*)current_focus;
	switch(hot::key) {
	case KeyEscape:
		if(pc) {
			setmode(0);
			if(current_focus != pc->getitem(RightHand)
				&& current_focus != pc->getitem(LeftHand))
				setfocus(pc->getitem(RightHand));
			draw::animation::update();
			draw::animation::render(0);
		}
		options(!allow_move);
		break;
	case 'I':
		if(getmode() == show_invertory) {
			setmode(0);
			if(pc) {
				if(current_focus != pc->getitem(RightHand)
					&& current_focus != pc->getitem(LeftHand))
					setfocus(pc->getitem(RightHand));
			}
		} else {
			setmode(show_invertory);
			return true;
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
		if(current_item && pc) {
			if(game.question((item*)current_item))
				return true;
		}
		break;
	case KeyLeft:
	case KeyRight:
	case KeyDown:
	case KeyUp:
		if(allow_move)
			return location.move(map_key_to_dir(hot::key));
		navigate(false);
		break;
	case KeyHome:
		location.rotate(Left);
		break;
	case KeyPageUp:
		location.rotate(Right);
		break;
	case 'E':
		if(pc) {
			auto caster = pc->getcaster();
			auto spell_element = pc->choosespell(caster);
			if(spell_element) {
				if(pc->cast(spell_element, caster, 0))
					return true;
			}
		}
		break;
	case 'W': movenext(KeyUp); break;
	case 'Z': movenext(KeyDown); break;
	case 'S': movenext(KeyRight); break;
	case 'A': movenext(KeyLeft); break;
	case 'P':
		place_item((item*)current_focus);
		return true;
	case 'G':
		if(allow_move) {
			location.pickitem((item*)current_focus);
			return true;
		}
		break;
	case 'D':
		if(allow_move)
			location.dropitem((item*)current_focus);
		else if(dlgask("You can't pick this item again. Do you really want to drop item?"))
			((item*)current_focus)->clear();
		return true;
	case 'U':
		return creature::use((item*)current_focus);
	case 'T':
		game.thrown((item*)current_focus);
		return true;
	case 'M':
		if(allow_move)
			return game.manipulate((item*)current_focus, to(game.getdirection(), Up));
		break;
	case 'V':
		if(allow_move)
			location.automap(true);
		break;
	case 'F':
		draw::animation::thrown(game.getcamera(), game.getdirection(), Arrow, Left, 50);
		break;
	case '1': case '2': case '3': case '4': case '5': case '6':
		pc = party[hot::key - '1'];
		if(!pc)
			break;
		if(current_focus) {
			if(creature::get(current_focus) != pc)
				setfocus(pc->getitem(RightHand));
		}
		break;
	}
	return false;
}

void adventurei::play() {
	while(ismodal()) {
		if(!getfocus())
			setfocus(party[0]->getitem(RightHand));
		draw::animation::update();
		draw::animation::render(0, true, getfocus());
		domodal();
		if(handle_shortcuts(true)) {
			game.endround();
			setnext(play);
		}
	}
}

static void main_new_game() {
	setnext(game.newgame);
}

static void option_new_game() {
	if(!dlgask("Are you really want to start new game?"))
		return;
	setnext(game.newgame);
}

static void memorize_spells() {
	creature::preparespells(Mage);
}

static void pray_for_spells() {
	creature::preparespells(Cleric);
}

static void option_save_game() {
	game.write();
}

static void quit_game() {
	if(!dlgask("Are you really want to quit game?"))
		setnext(mainmenu);
}

static void settings() {}

static void load_game() {
	draw::resetres();
	if(!game.read())
		return;
}

void draw::options(bool camp_mode) {
	const char* on = "Game options:";
	answers aw;
	aw.add((int)pray_for_spells, "Pray for spells");
	aw.add((int)memorize_spells, "Memorize spells");
	aw.add((int)game.scriblescrolls, "Scrible scrolls");
	aw.add((int)option_new_game, "New game");
	aw.add((int)load_game, "Load game");
	aw.add((int)option_save_game, "Save game");
	aw.add((int)settings, "Settings");
	aw.add((int)quit_game, "Quit game");
	auto p = (callback)aw.choosemn(on);
	if(p)
		p();
}

void editor();

void draw::mainmenu() {
	answers aw;
	aw.add((int)main_new_game, "Create New Game");
	aw.add((int)load_game, "Load Saved game");
	aw.add((int)editor, "Game editor");
	aw.add((int)quit_game, "Exit game");
	auto p = (callback)aw.choosemn(80, 110, 170, MENU);
	if(p)
		setnext(p);
}