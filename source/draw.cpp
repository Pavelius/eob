#include "command.h"
#include "crt.h"
#include "draw.h"
#include "drawbits.h"
#include "surface.h"

#ifndef __GNUC__
#pragma optimize("t", on)
#endif

// Clipping context
rect					draw::clipping;
color					draw::fore;
const sprite*			draw::font;
color*					draw::palt;
static unsigned char	forc;
// Hot keys and menus
int						hot::animate; //  аждый такт таймера это значение увеличиваетс€ на единицу.
int						hot::key; // —обытие, которое происходит в данный момент
point					hot::mouse; // current mouse coordinates
bool					hot::pressed; // flag if any of mouse keys is pressed
int						hot::param;
static int				current_command;
int						metrics::padding = 4;
sprite*					metrics::font;

float sqrt(const float x) {
	const float xhalf = 0.5f*x;
	union // get bits for floating value
	{
		float x;
		int i;
	} u;
	u.x = x;
	u.i = 0x5f3759df - (u.i >> 1);  // gives initial guess y0
	return x * u.x*(1.5f - xhalf * u.x*u.x);// Newton step, repeating increases accuracy 
}

int isqrt(int num) {
	int res = 0;
	int bit = 1 << 30;
	// "bit" starts at the highest power of four <= the argument.
	while(bit > num)
		bit >>= 2;
	while(bit != 0) {
		if(num >= res + bit) {
			num -= res + bit;
			res = (res >> 1) + bit;
		} else
			res >>= 1;
		bit >>= 2;
	}
	return res;
}

static inline void correct(int& x1, int& y1, int& x2, int& y2) {
	if(x1 > x2)
		iswap(x1, x2);
	if(y1 > y2)
		iswap(y1, y2);
}

static bool correct(int& x1, int& y1, int& x2, int& y2, const rect& clip, bool include_edge = true) {
	correct(x1, y1, x2, y2);
	if(x2 < clip.x1 || x1 >= clip.x2 || y2 < clip.y1 || y1 >= clip.y2)
		return false;
	if(x1 < clip.x1)
		x1 = clip.x1;
	if(y1 < clip.y1)
		y1 = clip.y1;
	if(include_edge) {
		if(x2 > clip.x2)
			x2 = clip.x2;
		if(y2 > clip.y2)
			y2 = clip.y2;
	} else {
		if(x2 >= clip.x2)
			x2 = clip.x2 - 1;
		if(y2 >= clip.y2)
			y2 = clip.y2 - 1;
	}
	return true;
}

static const char* glink(char* temp, const char** source) {
	temp[0] = 0;
	const char* p = *source;
	if(*p != '(')
		return 0;
	p++;
	char* s = temp;
	while(*p && *p != ')')
		*s++ = *p++;
	if(*p == ')')
		p++;
	*s++ = 0;
	*source = p;
	return temp;
}

static const char* word(const char* text) {
	while(((unsigned char)*text) > 0x20 && *text != '*' && *text != '[' && *text != ']')
		text++;
	return text;
}

static bool match(const char** string, const char* name) {
	int n = zlen(name);
	if(memcmp(*string, name, n) != 0)
		return false;
	(*string) += n;
	return true;
}

char* key2str(char* result, int key) {
	result[0] = 0;
	if(key&Ctrl)
		zcat(result, "Ctrl+");
	if(key&Alt)
		zcat(result, "Alt+");
	if(key&Shift)
		zcat(result, "Shift+");
	key = key & 0xFFFF;
	switch(key) {
	case KeyUp: zcat(result, "Up"); break;
	case KeyPageDown: zcat(result, "Page Down"); break;
	case KeyPageUp: zcat(result, "Page Up"); break;
	case KeyHome: zcat(result, "Home"); break;
	case KeyEnd: zcat(result, "End"); break;
	case KeyDown: zcat(result, "Down"); break;
	case KeyLeft: zcat(result, "Left"); break;
	case KeyRight: zcat(result, "Right"); break;
	case F1: zcat(result, "F1"); break;
	case F2: zcat(result, "F2"); break;
	case F3: zcat(result, "F3"); break;
	case F4: zcat(result, "F4"); break;
	case F5: zcat(result, "F5"); break;
	case F6: zcat(result, "F6"); break;
	case F7: zcat(result, "F7"); break;
	case F8: zcat(result, "F8"); break;
	case F9: zcat(result, "F9"); break;
	case F10: zcat(result, "F10"); break;
	case F11: zcat(result, "F11"); break;
	case F12: zcat(result, "F12"); break;
	case KeySpace: zcat(result, "Space"); break;
	default:
		zcat(result, char(szupper(key - Alpha)));
		break;
	}
	return result;
}

command* command_hot_clear;

void hot::clear() {
	command_hot_clear->execute();
	current_command = 0;
	hot::key = 0;
}

draw::state::state() :
	fore(draw::fore),
	font(draw::font),
	palt(draw::palt),
	canvas(draw::canvas),
	clip(clipping) {}

draw::state::~state() {
	draw::font = this->font;
	draw::fore = this->fore;
	draw::palt = this->palt;
	draw::clipping = this->clip;
	draw::canvas = this->canvas;
}

int draw::getcommand() {
	return current_command;
}

void draw::pixel(int x, int y) {
	if(x >= clipping.x1 && x < clipping.x2 && y >= clipping.y1 && y < clipping.y2) {
		if(!canvas)
			return;
		*((color*)draw::ptr(x, y)) = fore;
	}
}

void draw::pixel(int x, int y, unsigned char a) {
	if(x < clipping.x1 || x >= clipping.x2 || y < clipping.y1 || y >= clipping.y2 || a == 0xFF)
		return;
	auto p = (color*)ptr(x, y);
	if(a == 0)
		*p = fore;
	else {
		p->b = (((unsigned)p->b*(a)) + (fore.b*(255 - a))) >> 8;
		p->g = (((unsigned)p->g*(a)) + (fore.g*(255 - a))) >> 8;
		p->r = (((unsigned)p->r*(a)) + (fore.r*(255 - a))) >> 8;
		p->a = 0;
	}
}

void draw::line(int x0, int y0, int x1, int y1) {
	if(!canvas)
		return;
	if(y0 == y1) {
		if(!correct(x0, y0, x1, y1, clipping, false))
			return;
		switch(canvas->bpp) {
		case 8:
			bop::set8(canvas->ptr(x0, y0), canvas->scanline, x1 - x0 + 1, 1, forc);
			break;
		case 32:
			bop::set32(canvas->ptr(x0, y0), canvas->scanline, x1 - x0 + 1, 1, fore);
			break;
		}
	} else if(x0 == x1) {
		if(!correct(x0, y0, x1, y1, clipping, false))
			return;
		switch(canvas->bpp) {
		case 8:
			bop::set8(canvas->ptr(x0, y0), canvas->scanline, 1, y1 - y0 + 1, forc);
			break;
		case 32:
			bop::set32(canvas->ptr(x0, y0), canvas->scanline, 1, y1 - y0 + 1, fore);
			break;
		}
	}
	int dx = iabs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -iabs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	for(;;) {
		pixel(x0, y0);
		e2 = 2 * err;
		if(e2 >= dy) {
			if(x0 == x1) break;
			err += dy; x0 += sx;
		}
		if(e2 <= dx) {
			if(y0 == y1) break;
			err += dx; y0 += sy;
		}
	}
}

void draw::line(int x0, int y0, int x1, int y1, color c1) {
	draw::state push; fore = c1;
	line(x0, y0, x1, y1);
}

void draw::rectb(rect rc) {
	line(rc.x1, rc.y1, rc.x2, rc.y1);
	line(rc.x2, rc.y1 + 1, rc.x2, rc.y2);
	line(rc.x2 - 1, rc.y2, rc.x1, rc.y2);
	line(rc.x1, rc.y2 - 1, rc.x1, rc.y1);
}

void draw::rectb(rect rc, unsigned char c1) {
	forc = c1;
	rectb(rc);
}

void draw::rectb(rect rc, color c1) {
	draw::state push;
	fore = c1;
	rectb(rc);
}

void draw::rectf(rect rc) {
	if(!canvas)
		return;
	if(!correct(rc.x1, rc.y1, rc.x2, rc.y2, clipping))
		return;
	if(rc.x1 == rc.x2)
		return;
	switch(canvas->bpp) {
	case 8:
		bop::set8(ptr(rc.x1, rc.y1), canvas->scanline,
			rc.x2 - rc.x1, rc.y2 - rc.y1, forc);
		break;
	case 32:
		bop::set32(ptr(rc.x1, rc.y1), canvas->scanline,
			rc.x2 - rc.x1, rc.y2 - rc.y1, fore);
		break;
	}
}

void draw::rectf(rect rc, unsigned char c1) {
	forc = c1;
	rectf(rc);
}

void draw::rectf(rect rc, color c1) {
	state push;
	fore = c1;
	rectf(rc);
}

void draw::rectf(rect rc, color c1, unsigned char alpha) {
	if(!canvas)
		return;
	if(!correct(rc.x1, rc.y1, rc.x2, rc.y2, clipping))
		return;
	if(rc.x1 == rc.x2)
		return;
	switch(canvas->bpp) {
	case 8:
		break;
	case 32:
		bop::set32(ptr(rc.x1, rc.y1), canvas->scanline,
			rc.x2 - rc.x1, rc.y2 - rc.y1, c1, alpha);
		break;
	}
}

void draw::setclip() {
	clipping.set(0, 0, getwidth(), getheight());
}

void draw::setclip(rect rcn) {
	rect rc = draw::clipping;
	if(rc.x1 < rcn.x1)
		rc.x1 = rcn.x1;
	if(rc.y1 < rcn.y1)
		rc.y1 = rcn.y1;
	if(rc.x2 > rcn.x2)
		rc.x2 = rcn.x2;
	if(rc.y2 > rcn.y2)
		rc.y2 = rcn.y2;
	draw::clipping = rc;
}

int	draw::aligned(int x, int width, unsigned flags, int dx) {
	switch(flags&AlignMask) {
	case AlignRightBottom:
	case AlignRightCenter:
	case AlignRight: return x + width - dx;
	case AlignCenterBottom:
	case AlignCenterCenter:
	case AlignCenter: return x + (width - dx) / 2;
	default: return x;
	}
}

int draw::textw(const char* string, int count) {
	if(!font)
		return 0;
	int x1 = 0;
	if(count == -1) {
		const char *s1 = string;
		while(*s1)
			x1 += textw(szget(&s1));
	} else {
		const char *s1 = string;
		const char *s2 = string + count;
		while(s1 < s2)
			x1 += textw(szget(&s1));
	}
	return x1;
}

const char* draw::skiptr(const char* string) {
	// skiping trail symbols
	for(; *string && *string == 0x20; string++);
	if(*string == 13) {
		string++;
		if(*string == 10)
			string++;
	} else if(*string == 10) {
		string++;
		if(*string == 13)
			string++;
	}
	return string;
}

int draw::textw(rect& rc, const char* string) {
	int w1 = rc.width();
	rc.y2 = rc.y1;
	rc.x2 = rc.x1;
	while(string[0]) {
		int c = textbc(string, w1);
		if(!c)
			break;
		int m = textw(string, c);
		if(rc.width() < m)
			rc.x2 = rc.x1 + m;
		rc.y2 += texth();
		string = skiptr(string + c);
	}
	return rc.height();
}

int draw::texth(const char* string, int width) {
	int dy = texth();
	int y1 = 0;
	while(*string) {
		int c = textbc(string, width);
		if(!c)
			break;
		y1 += dy;
		string = skiptr(string + c);
	}
	return y1;
}

void draw::text(int x, int y, const char* string, int count, unsigned flags) {
	if(!font)
		return;
	int dy = texth();
	if(y >= clipping.y2 || y + dy < clipping.y1)
		return;
	if(count == -1)
		count = zlen(string);
	const char *s1 = string;
	const char *s2 = string + count;
	while(s1 < s2) {
		int sm = szget(&s1);
		if(sm >= 0x21)
			glyph(x, y, sm, flags);
		x += textw(sm);
	}
}

int draw::textc(int x, int y, int width, const char* string, int count, unsigned flags) {
	state push;
	setclip({x, y, x + width, y + texth()});
	text(x, y, string, count, flags);
	return texth();
}

int draw::textbc(const char* string, int width, int* string_width) {
	if(!font)
		return 0;
	int p = -1;
	int w = 0;
	const char* s1 = string;
	while(true) {
		unsigned s = szget(&s1);
		if(s == 0x20 || s == 9)
			p = s1 - string;
		else if(s == 0) {
			p = s1 - string - 1;
			break;
		} else if(s == 10 || s == 13) {
			p = s1 - string;
			break;
		}
		w += textw(s);
		if(w > width)
			break;
	}
	if(p == -1)
		p = s1 - string;
	if(string_width)
		*string_width = w;
	return p;
}

int draw::alignedh(const rect& rc, const char* string, unsigned state) {
	int ty;
	switch(state&AlignMask) {
	case AlignCenterCenter:
	case AlignRightCenter:
	case AlignLeftCenter:
		if(state&TextSingleLine)
			ty = draw::texth();
		else
			ty = draw::texth(string, rc.width());
		return (rc.height() - ty) / 2;
	case AlignCenterBottom:
	case AlignRightBottom:
	case AlignLeftBottom:
		if(state&TextSingleLine)
			ty = draw::texth();
		else
			ty = draw::texth(string, rc.width());
		return rc.y2 - ty;
	default:
		return 0;
	}
}

int	draw::text(rect rc, const char* string, unsigned state) {
	int x1 = rc.x1;
	int y1 = rc.y1 + alignedh(rc, string, state);
	int dy = texth();
	if(state&TextSingleLine) {
		text(aligned(x1, rc.width(), state, draw::textw(string)), y1,
			string, -1, state);
		return dy;
	} else {
		int w1 = rc.width();
		//while(y1 < rc.y2)
		while(true) {
			int w;
			int c = textbc(string, w1, &w);
			if(!c)
				break;
			text(aligned(x1, w1, state, w), y1, string, c, state);
			y1 += dy;
			string = skiptr(string + c);
		}
		return y1 - rc.y1;
	}
}

int draw::textf(int x0, int y0, int width, const char** string, color c1, int* max_width) {
	int tb;
	char temp[512];
	int y = y0;
	int x = x0;
	int x2 = x0 + width;
	const char* p = *string;
	unsigned flags = 0;
	fore = c1;
	if(max_width)
		*max_width = 0;
	while(true) {
		if(p[0] == '*' && p[1] == '*') {
			p += 2;
			if(flags&TextBold)
				flags &= ~TextBold;
			else
				flags |= TextBold;
			continue;
		} else if(p[0] == '*') {
			p++;
			if(flags&TextItalic)
				flags &= ~TextItalic;
			else {
				if((flags&TextItalic) == 0)
					x += texth() / 3;
				flags |= TextItalic;
			}
			continue;
		} else if(p[0] == '[' && p[1] == '[')
			p++;
		else if(p[0] == ']' && p[1] == ']')
			p++;
		else if(p[0] == '[') {
			p++;
			fore = colors::blue;
			switch(*p) {
			case '~':
				p++;
				fore = colors::gray;
				break;
			case '+':
				p++;
				fore = colors::green;
				break;
			case '-':
				p++;
				fore = colors::red;
				break;
			case '!':
				p++;
				fore = colors::yellow;
				break;
			}
			if(glink(temp, &p))
				flags |= TextUscope;
		} else if(p[0] == ']') {
			p++;
			fore = c1;
			flags &= ~TextUscope;
		}
		// ќбработаем пробелы и табул€цию
		while(true) {
			switch(p[0]) {
			case ' ':
				p++;
				x += textw(' ');
				continue;
			case '\t':
				p++;
				tb = textw(' ') * 4;
				x = x0 + ((x - x0 + tb) / tb)*tb;
				continue;
			}
			break;
		}
		const char* p2 = word(p);
		int w = textw(p, p2 - p);
		if(x + w > x2) {
			if(max_width)
				*max_width = imax(*max_width, x - x0);
			x = x0;
			y += texth();
		}
		int x4 = x;
		text(x, y, p, p2 - p, flags);
		x += w;
		p = p2;
		// ќбработаем пробелы и табул€цию
		while(true) {
			switch(p[0]) {
			case ' ':
				p++;
				x += textw(' ');
				continue;
			case '\t':
				p++;
				tb = textw(' ') * 4;
				x = x0 + ((x - x0 + tb) / tb)*tb;
				continue;
			}
			break;
		}
		// ќтметим перевод строки и окончание строки
		if(p[0] == 0 || p[0] == 10 || p[0] == 13) {
			y += texth();
			p = szskipcr(p);
			break;
		}
	}
	if(max_width)
		*max_width = imax(*max_width, x - x0);
	*string = p;
	return y - y0;
}

int draw::textlb(const char* string, int index, int width, int* line_index, int* line_count) {
	auto dy = texth();
	auto p = string;
	if(line_index)
		*line_index = 0;
	if(line_count)
		*line_count = 0;
	while(true) {
		int c = textbc(p, width);
		if(!c)
			break;
		if(index < c || p[c] == 0) {
			if(line_count)
				*line_count = c;
			break;
		}
		index -= c;
		if(line_index)
			*line_index = *line_index + 1;
		p = skiptr(p + c);
	}
	return p - string;
}

static unsigned char* skip_v3(unsigned char* s, int h) {
	const int		cbs = 1;
	const int		cbd = 1;
	if(!s || !h)
		return s;
	while(true) {
		unsigned char c = *s++;
		if(c == 0) {
			if(--h == 0)
				return s;
		} else if(c <= 0x9F) {
			if(c <= 0x7F)
				s += c * cbs;
			else {
				if(c == 0x80)
					c = *s++;
				else
					c -= 0x80;
				s++;
				s += c * cbs;
			}
		} else if(c == 0xA0)
			s++;
	}
}

static unsigned char* skip_rle32(unsigned char* s, int h) {
	const int cbs = 3;
	if(!s || !h)
		return s;
	while(true) {
		unsigned char c = *s++;
		if(c == 0) {
			if(--h == 0)
				return s;
		} else if(c <= 0x9F) {
			if(c <= 0x7F)
				s += c * cbs;
			else {
				if(c == 0x80)
					c = *s++;
				else
					c -= 0x80;
				s++;
				s += c * cbs;
			}
		} else if(c == 0xA0)
			s++;
	}
}

void draw::image(int x, int y, const sprite* e, int id, int flags, unsigned char alpha) {
	const int cbd = 1;
	int x2, y2;
	color* pal;
	if(!e)
		return;
	const sprite::frame& f = e->get(id);
	if(!f.offset)
		return;
	if(!canvas)
		return;
	if(flags&ImageMirrorH) {
		x2 = x;
		if((flags&ImageNoOffset) == 0)
			x2 += f.ox;
		x = x2 - f.sx;
	} else {
		if((flags&ImageNoOffset) == 0)
			x -= f.ox;
		x2 = x + f.sx;
	}
	if(flags&ImageMirrorV) {
		y2 = y;
		if((flags&ImageNoOffset) == 0)
			y2 += f.oy;
		y = y2 - f.sy;
	} else {
		if((flags&ImageNoOffset) == 0)
			y -= f.oy;
		y2 = y + f.sy;
	}
	unsigned char* s = (unsigned char*)e + f.offset;
	if(y2<clipping.y1 || y>clipping.y2 || x2<clipping.x1 || x>clipping.x2)
		return;
	int bypp = canvas->bpp / 8;
	if(y < clipping.y1) {
		if((flags&ImageMirrorV) == 0) {
			switch(f.encode) {
			case sprite::RAW: s += (clipping.y1 - y)*f.sx * 3; break;
			case sprite::RAW8: s += (clipping.y1 - y)*f.sx; break;
			case sprite::RLE8: s = skip_v3(s, clipping.y1 - y); break;
			case sprite::RLE: s = skip_rle32(s, clipping.y1 - y); break;
			default: break;
			}
		}
		y = clipping.y1;
	}
	if(y2 > clipping.y2) {
		if(flags&ImageMirrorV) {
			switch(f.encode) {
			case sprite::RAW: s += (y2 - clipping.y2)*f.sx * 3; break;
			case sprite::RAW8: s += (y2 - clipping.y2)*f.sx; break;
			case sprite::RLE8: s = skip_v3(s, y2 - clipping.y2); break;
			case sprite::RLE: s = skip_rle32(s, y2 - clipping.y2); break;
			default: break;
			}
		}
		y2 = clipping.y2;
	}
	if(y >= y2)
		return;
	int wd = (flags&ImageMirrorV) ? -canvas->scanline : canvas->scanline;
	int sy = (flags&ImageMirrorV) ? y2 - 1 : y;
	switch(f.encode) {
	case sprite::RAW:
		if(x < clipping.x1) {
			s += (clipping.x1 - x) * 3;
			x = clipping.x1;
		}
		if(x2 > clipping.x2)
			x2 = clipping.x2;
		if(x >= x2)
			return;
		if(bypp == 4) {
			if(flags&ImageMirrorH)
				bop::raw32m(ptr(x2 - 1, y), wd, s, f.sx * 3,
					x2 - x,
					y2 - y);
			else
				bop::raw32(ptr(x, y), wd, s, f.sx * 3,
					x2 - x,
					y2 - y);
		}
		break;
	case sprite::RAW8:
		if(x < clipping.x1) {
			s += clipping.x1 - x;
			x = clipping.x1;
		}
		if(x2 > clipping.x2)
			x2 = clipping.x2;
		if(x >= x2)
			return;
		if(!f.pallette || (flags&ImagePallette))
			pal = draw::palt;
		else
			pal = (color*)e->offs(f.pallette);
		if(!pal)
			return;
		if(flags&ImageMirrorH)
			bop::raw832m(ptr(x2 - 1, y), wd, s, f.sx,
				x2 - x,
				y2 - y,
				pal);
		else
			bop::raw832(ptr(x, y), wd, s, f.sx, x2 - x, y2 - y, pal);
		break;
	case sprite::RLE8:
		if(!f.pallette || (flags&ImagePallette))
			pal = draw::palt;
		else
			pal = (color*)e->offs(f.pallette);
		if(!pal)
			return;
		if(flags&ImageMirrorH)
			bop::rle832m(ptr(x2 - 1, sy), wd, s, y2 - y,
				ptr(clipping.x1, sy),
				ptr(clipping.x2, sy),
				alpha, pal);
		else
			bop::rle832(ptr(x, sy), wd, s, y2 - y,
				ptr(clipping.x1, sy),
				ptr(clipping.x2, sy),
				alpha, pal);
		break;
	case sprite::RLE:
		if(flags&ImageMirrorH)
			bop::rle32m(ptr(x2 - 1, sy), wd, s, y2 - y,
				ptr(clipping.x1, sy),
				ptr(clipping.x2, sy),
				alpha);
		else
			bop::rle32(ptr(x, sy), wd, s, y2 - y,
				ptr(clipping.x1, sy),
				ptr(clipping.x2, sy),
				alpha);
		break;
	default:
		break;
	}
}

void draw::image(int x, int y, const sprite* e, int id, int flags, unsigned char alpha, color* pal) {
	draw::state push;
	draw::palt = pal;
	image(x, y, e, id, flags | ImagePallette, alpha);
}

static bool corrects(const draw::surface& dc, int& x, int& y, int& width, int& height) {
	if(x + width > dc.width)
		width = dc.width - x;
	if(y + height > dc.height)
		height = dc.height - y;
	if(width <= 0 || height <= 0)
		return false;
	return true;
}

static bool correctb(int& x1, int& y1, int& w, int& h, int& ox) {
	int x11 = x1;
	int x2 = x1 + w;
	int y2 = y1 + h;
	if(!correct(x1, y1, x2, y2, draw::clipping))
		return false;
	ox = x1 - x11;
	w = x2 - x1;
	h = y2 - y1;
	return true;
}

void draw::blit(surface& ds, int x1, int y1, int w, int h, unsigned flags, draw::surface& ss, int xs, int ys) {
	if(ss.bpp != ds.bpp)
		return;
	int ox;
	if(!correctb(x1, y1, w, h, ox))
		return;
	if(ds.bpp == 8) {
		if(flags&ImageMirrorH) {
			if(flags&ImageTransparent)
				bop::raw8tm(
					ds.ptr(x1 + w - 1, y1), ds.scanline,
					ss.ptr(xs, ys) + ox, ss.scanline,
					w, h);
			else
				bop::raw8m(
					ds.ptr(x1 + w - 1, y1), ds.scanline,
					ss.ptr(xs, ys) + ox, ss.scanline,
					w, h);
		} else {
			if(flags&ImageTransparent)
				bop::raw8t(
					ds.ptr(x1, y1), ds.scanline,
					ss.ptr(xs, ys) + ox, ss.scanline,
					w, h);
			else
				bop::cpy(
					ds.ptr(x1, y1), ds.scanline,
					ss.ptr(xs, ys) + ox, ss.scanline,
					w, h, 1);
		}
	} else if(ds.bpp == 32) {
		if(flags&ImageTransparent)
			bop::cpy32t(
				ds.ptr(x1, y1), ds.scanline,
				ss.ptr(xs, ys) + ox * 4, ss.scanline,
				w, h);
		else
			bop::cpy(
				ds.ptr(x1, y1), ds.scanline,
				ss.ptr(xs, ys) + ox * 4, ss.scanline,
				w, h, 4);
	}
}

void draw::blit(surface& dest, int x, int y, int width, int height, unsigned flags, surface& source, int x_source, int y_source, int width_source, int height_source) {
	if(width == width_source && height == height_source) {
		blit(dest, x, y, width, height, flags, source, x_source, y_source);
		return;
	}
	if(source.bpp != dest.bpp)
		return;
	if(!corrects(dest, x, y, width, height))
		return;
	if(!corrects(source, x_source, y_source, width_source, height_source))
		return;
	int ox;
	if(!correctb(x_source, y_source, width, height, ox))
		return;
	if(dest.bpp == 32) {
		bop::scale32(
			dest.ptr(x, y), dest.scanline, width, height,
			source.ptr(x_source, y_source) + ox * 4, source.scanline, width_source, height_source);
	}
}

void draw::execute(int id, int param) {
	hot::key = 0;
	hot::param = param;
	current_command = id;
}

int draw::sysinput(bool redraw) {
	auto temp_command = current_command;
	hot::clear();
	if(temp_command)
		hot::key = temp_command;
	if(hot::key)
		return hot::key;
	int id = InputUpdate;
	if(redraw)
		rawredraw();
	else
		id = rawinput();
	return id;
}