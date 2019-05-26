#include "draw.h"
#include "main.h"

const int dx = 4;

namespace colors {
color				dark = color::create(52, 52, 80);
color				down = color::create(81, 85, 166);
color				focus = color::create(250, 100, 100);
color				header = color::create(255, 255, 100);
color				light = color::create(148, 148, 172);
color				main = color::create(108, 108, 136);
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

int draw::flatb(int x, int y, int width, int id, unsigned flags, const char* string) {
	int height = draw::texth() + 1;
	if(flags&Focused)
		rectf({x, y, x + width, y + height + 1}, colors::dark);
	else
		border_up({x, y, x + width, y + height});
	draw::text(x + 1 + (width - draw::textw(string)) / 2, y + 1, string);
	return draw::texth();
}

int draw::linetext(int x, int y, int width, int id, unsigned flags, const char* name) {
	state push;
	if(flags&Focused)
		fore = colors::focus;
	textb(draw::aligned(x, width, flags, textw(name)), y, name);
	return texth() + 1;
}

void draw::linetext(int x, int y, int width, int id, unsigned flags, const char* name, const char* name2) {
	state push;
	rect rc = {x, y, x + width, y + 8};
	if(flags&Focused)
		fore = colors::focus;
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

rect draw::form(rect rc, int count) {
	for(int i = 0; i < count; i++) {
		border_up(rc);
		rc.offset(1);
	}
	rectf({rc.x1, rc.y1, rc.x2 + 1, rc.y2 + 1}, colors::main);
	rc.offset(4, 4);
	return rc;
}

unsigned draw::getfstate(int id, int focus) {
	return (id == focus) ? Focused : 0;
}

int draw::button(int x, int y, int width, int id, unsigned state, const char* name) {
	draw::state push;
	if(width == -1)
		width = textw(name) + 3 * 2;
	rect rc = {x, y, x + width, y + draw::texth() + 4};
	form(rc);
	if(state&Focused)
		fore = colors::focus;
	rc.offset(3, 2);
	textb(rc, name, state);
	return draw::texth();
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

bool draw::dlgask(const char* text) {
	draw::state push;
	draw::screenshoot screen(true);
	draw::fore = colors::white;
	rect rc = getformpos(text, draw::texth() + dx * 2);
	int focus = OK;
	while(draw::ismodal()) {
		screen.restore();
		draw::form(rc);
		int x1 = rc.x1 + dx;
		int y1 = rc.y1 + dx;
		int wd = rc.width() - dx * 2;
		rect rct = rc; rct.offset(dx, dx);
		y1 += draw::text(rct, text) + dx;
		x1 = (320 - (36 + 36)) / 2;
		draw::button(x1, y1, 32, OK, draw::getfstate(OK, focus), "Yes");
		draw::button(x1 + 36, y1, 32, Cancel, draw::getfstate(Cancel, focus), "No");
		draw::domodal();
		switch(hot::key) {
		case KeyEscape:
		case Cancel:
			return false;
		case OK:
			return true;
		case KeyEnter:
			return (focus == OK);
		case KeyLeft:
		case Alpha + 'A':
			//focus = draw::getnext(focus, KeyLeft);
			break;
		case Alpha + 'S':
		case KeyRight:
			//focus = draw::getnext(focus, KeyRight);
			break;
		}
	}
}