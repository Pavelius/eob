#include "view.h"

using namespace draw;

namespace draw {
struct fxt {
	short int		filesize;			// the size of the file
	short int		charoffset[128];	// the offset of the pixel data from the beginning of the file, the index is the ascii value
	unsigned char	height;				// the height of a character in pixel
	unsigned char	width;				// the width of a character in pixel
	unsigned char	data[1];			// the pixel data, one byte per line 
};
}
namespace colors {
color				selected = color::create(250, 250, 250);
color				drag = color::create(250, 100, 250);
color				title = color::create(64, 255, 255);
namespace info {
color				text = color::create(64, 64, 64);
}
}
static surface		dc(320, 200, 32);
static surface		dw(dc.width * 3, dc.height * 3, 32);
static fxt*			font6 = (fxt*)loadb("art/misc/font6.fnt");
static fxt*			font8 = (fxt*)loadb("art/misc/font8.fnt");
unsigned			draw::frametick;
static unsigned		frametick_last;
static infoproc		show_mode;
static item*		current_item;
static item*		drag_item;
static char			log_message[128];
static rect			log_rect = {5, 180, 285, 198};
extern "C" void		scale3x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height);
void				view_dungeon_reset();

static struct resource_info {
	const char*		name;
	const char*		path;
	sprite*			data;
} objects[] = {{"NONE"},
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
{"XSPL", "art/misc"},
//
{"ANKHEG", "art/monsters"},
{"ANT", "art/monsters"},
{"BLDRAGON", "art/monsters"},
{"BUGBEAR", "art/monsters"},
{"CLERIC1", "art/monsters"},
{"CLERIC2", "art/monsters"},
{"CLERIC3", "art/monsters"},
{"DRAGON", "art/monsters"},
{"FLIND", "art/monsters"},
{"GHOUL", "art/monsters"},
{"GOBLIN", "art/monsters"},
{"GUARD1", "art/monsters"},
{"GUARD2", "art/monsters"},
{"KOBOLD", "art/monsters"},
{"KUOTOA", "art/monsters"},
{"LEECH", "art/monsters"},
{"ORC", "art/monsters"},
{"SKELETON", "art/monsters"},
{"SKELWAR", "art/monsters"},
{"SPIDER1", "art/monsters"},
{"ZOMBIE", "art/monsters"},
};
static_assert((sizeof(objects) / sizeof(objects[0])) == Count, "Need resource update");

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
		int count = (cur - frametick_last) / ms;
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

int draw::input(bool redraw) {
	preredraw();
	draw::state push;
	draw::canvas = &dw;
	int id = sysinput(redraw);
	if(!id)
		exit(0);
	update_frame_counter();
	return id;
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
		case DetectedMagic:
			pallette[12] = colors::blue.mix(colors::white, draw::ciclic(128, 4));
			break;
		case DetectedEvil:
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
	stringcreator e(zend(log_message), zendof(log_message));
	if(log_message[0])
		e.add("\n");
	e.addv(format, vl);
}

void mslog(const char* format, ...) {
	mslogv(format, xva_start(format));
}

void draw::logs() {
	draw::state push;
	draw::setclip(log_rect);
	//draw::textf(log_rect.x1, log_rect.y1, log_rect.width(), log_message);
	draw::text(log_rect, log_message);
}

void draw::itemicn(int x, int y, item* pitm, bool invlist, unsigned flags, void* current_item) {
	if(!pitm)
		return;
	rect rc;
	auto pid = game::getitempart(pitm);
	auto pc = game::gethero(pitm);
	unsigned char alpha = 0xFF;
	if(invlist) {
		if(pid == LeftRing || pid == RightRing)
			rc.set(x - 4, y - 5, x + 4, y + 3);
		else
			rc.set(x - 8, y - 9, x + 8, y + 7);
	} else
		rc.set(x - 16, y - 7, x + 14, y + 8);
	focusing(rc, pitm);
	if(pitm == current_item)
		rectb(rc, colors::selected);
	if(pitm == drag_item)
		rectb(rc, colors::drag);
	if(!(*pitm)) {
		if(!invlist)
			image(x, y, gres(ITEMS), 83 + ((pid == RightHand) ? 0 : 1), 0, alpha);
	} else {
		auto state = NoState;
		if(pc->is(DetectedEvil) && pitm->iscursed())
			state = DetectedEvil;
		else if(pc->is(DetectedMagic) && pitm->getmagic())
			state = DetectedMagic;
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
	draw::invertory(178, 0, game::gethero(current_item), current_item);
}

static void show_abilities(item* current_item) {
	draw::abilities(178, 0, game::gethero(current_item));
}

static void show_skills(item* current_item) {
	draw::skills(178, 0, game::gethero(current_item));
}

command_s game::action::actions() {
	creature* pc;
	while(true) {
		if(!current_item)
			current_item = game::party[0]->getitem(RightHand);
		draw::animation::update();
		draw::animation::render(0, true, current_item);
		int id = draw::input();
		switch(id) {
		case KeyEscape:
			if(true) {
				setmode(0);
				auto pc = game::gethero(current_item);
				auto pid = game::getitempart(current_item);
				if(pid != RightHand && pid != LeftHand)
					current_item = pc->getitem(RightHand);
				draw::animation::update();
				draw::animation::render(0);
			}
			switch(game::action::options()) {
			case NewGame:
				return NewGame;
			}
			break;
		case Alpha + 'I':
			if(getmode() == show_invertory) {
				setmode(0);
				auto pc = game::gethero(current_item);
				auto pid = game::getitempart(current_item);
				if(pid != RightHand && pid != LeftHand)
					current_item = pc->getitem(RightHand);
			} else {
				setmode(show_invertory);
				return PassSegment;
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
				if(game::action::question(current_item))
					return PassSegment;
			}
			break;
		case KeyLeft:
		case KeyRight:
		case KeyDown:
		case KeyUp:
			return game::action::move(map_key_to_dir(id));
		case KeyHome:
			game::action::rotate(Left);
			break;
		case KeyPageUp:
			game::action::rotate(Right);
			break;
		case Alpha + 'W':
			current_item = getnext(current_item, KeyUp);
			break;
		case Alpha + 'Z':
			current_item = getnext(current_item, KeyDown);
			break;
		case Alpha + 'S':
			current_item = getnext(current_item, KeyRight);
			break;
		case Alpha + 'A':
			current_item = getnext(current_item, KeyLeft);
			break;
		case Alpha + 'P':
			place_item(current_item);
			break;
		case Alpha + 'G':
			game::action::getitem(current_item);
			break;
		case Alpha + 'D':
			game::action::dropitem(current_item);
			break;
		case Alpha + 'U':
			if(game::action::use(current_item))
				return PassSegment;
			break;
		case Alpha + 'T':
			game::action::thrown(current_item);
			break;
		case Alpha + 'M':
			if(game::action::manipulate(current_item, vectorized(game::getdirection(), Up)))
				return PassSegment;
			break;
		case Alpha + 'V':
			game::action::automap(location, true);
			break;
		case Alpha + 'F':
			draw::animation::thrown(getcamera(), getdirection(), Arrow, Left, 50);
			break;
		case Alpha + 'H':
			draw::animation::thrown(getcamera(), getdirection(), Arrow, Right, 50);
			break;
		case Alpha + '1':
		case Alpha + '2':
		case Alpha + '3':
		case Alpha + '4':
		case Alpha + '5':
		case Alpha + '6':
			pc = game::party[id - (Alpha + '1')];
			if(!pc)
				break;
			if(game::gethero(current_item) != pc)
				current_item = pc->getitem(RightHand);
			break;
		}
	}
}