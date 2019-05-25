#include "color.h"
#include "point.h"
#include "sprite.h"

#pragma once

enum input_events {
	// input events
	InputUser = 0xE000,
	InputSymbol = 0xED00, InputTimer, InputEdit, InputEditPlus, InputEditMinus,
	InputKeyUp, InputIdle, InputUpdate, InputNoUpdate,
	InputChoose, InputDropDown, InputMenu, InputSet,
	ScrollV, ScrollH, SplitterV, SplitterH,
	TreeType, TreeFlags,
	HtmlLink, HtmlControl, TabsControl, TabsCloseControl,
	Executed,
	// control keys
	MouseLeft = 0xEE00, MouseLeftDBL, MouseRight,
	MouseMove, MouseWheelUp, MouseWheelDown,
	KeyLeft, KeyRight, KeyUp, KeyDown, KeyPageUp, KeyPageDown, KeyHome, KeyEnd,
	KeyBackspace, KeyEnter, KeyDelete, KeyEscape, KeySpace, KeyTab,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	// named keys range
	Alpha,
	FirstKey = MouseLeft,
	FirstMouse = MouseLeft, LastMouse = MouseWheelDown,
	// support
	CommandMask = 0x0000FFFF,
	// misc events can be combination with previous
	Ctrl = 0x00010000,
	Alt = 0x00020000,
	Shift = 0x00040000,
	// control visual flags
	HideActiveBorder = 0x00100000,
	HideBackground = 0x00200000,
	HideBorder = 0x00400000,
	// command flags (common for all)
	ShowText = 0x01000000,
	NoHandleTab = 0x02000000,
	NoFocusing = 0x04000000,
	// state flags
	Focused = 0x10000000, // Control has keyboard input and can change visual form.
	Checked = 0x20000000, // Use in background virtual method.
	Disabled = 0x40000000, // Control not grant any input.
	FirstInput = InputSymbol,
};
enum iflags {
	ImageMirrorV = 0x0001,
	ImageMirrorH = 0x0002,
	ImageGrayscale = 0x0004,
	ImageNoOffset = 0x0008,
	ImageTransparent = 0x0010,
	ImageColor = 0x0020,
	ImagePallette = 0x0040,
	TextItalic = 0x0100,
	TextBold = 0x0200,
	TextUscope = 0x0400,
	TextSingleLine = 0x0800, // Text would be showed as single line
	AlignLeft = 0x0000,
	AlignCenter = 0x1000,
	AlignRight = 0x2000,
	AlignLeftCenter = 0x3000,
	AlignCenterCenter = 0x4000,
	AlignRightCenter = 0x5000,
	AlignLeftBottom = 0x6000,
	AlignCenterBottom = 0x7000,
	AlignRightBottom = 0x8000,
	AlignWidth = 0xE000,
	AlignMask = 0xF000,
};
namespace metrics {
extern sprite*			font;
extern int				padding;
}
namespace hot {
extern int				animate;
void					clear();
extern int				key; // [in] if pressed key or mouse this field has key
extern point			mouse; // current mouse coordinates
extern bool				pressed; // flag if any of mouse keys is pressed
extern int				param; // command or input event parameter
}
namespace draw {
struct surface {
	struct plugin {
		const char*		name;
		const char*		filter;
		plugin*			next;
		static plugin*	first;
		//
		plugin(const char* name, const char* filter);
		//
		virtual int		decode(unsigned char* output, const unsigned char* input, unsigned size, int& output_scanline) = 0;
		virtual bool	inspect(int& w, int& h, int& bpp, const unsigned char* input, unsigned size) = 0;
	};
	int					width;
	int					height;
	int					scanline;
	int					bpp;
	unsigned char*		bits;
	surface();
	surface(int width, int height, int bpp);
	surface(const char* url, const char* pal = 0);
	~surface();
	operator bool() const { return bits != 0; }
	void				convert(int bpp, color* pallette);
	unsigned char*		ptr(int x, int y) { return bits + y * scanline + x * (bpp / 8); }
	int					read(const char* url, color* pallette, int need_bpp = 0);
	void				resize(int width, int height, int bpp, bool alloc_memory);
	void				write(const char* url);
};
struct screenshoot : public point, public surface {
	screenshoot(bool fade = false);
	screenshoot(rect rc, bool fade = false);
	~screenshoot();
	void				blend(draw::surface& e, unsigned delay);
	void				restore();
};
struct state // Push state in stack
{
	state();
	~state();
private:
	color				fore;
	const sprite*		font; // glyph font
	color*				palt;
	struct surface*		canvas;
	rect				clip;
};
extern rect				clipping;
extern color			fore;
extern const sprite*	font; // glyph font
typedef void(*callback)();
//
int						alignedh(const rect& rc, const char* string, unsigned state);
int						aligned(int x, int width, unsigned flags, int dx);
void					blit(surface& dest, int x, int y, int width, int height, unsigned flags, surface& dc, int xs, int ys);
void					blit(surface& dest, int x, int y, int width, int height, unsigned flags, surface& source, int x_source, int y_source, int width_source, int height_source);
extern surface*			canvas;
bool					create(int width, int height);
void					execute(int id, int param = 0);
void					execute(callback proc, int param = 0);
int						getbpp();
int						getcommand();
int						getheight();
int						getwidth();
void					glyph(int x, int y, int sym, unsigned flags);
void					image(int x, int y, const sprite* e, int id, int flags, unsigned char alpha = 0xFF);
void					image(int x, int y, const sprite* e, int id, int flags, unsigned char alpha, color* pal);
int						input(bool redraw = false);
void					line(int x1, int y1, int x2, int y2); // Draw line
void					line(int x1, int y1, int x2, int y2, color c1); // Draw line
inline void				line(point p1, point p2, color c1) { line(p1.x, p1.y, p2.x, p2.y, c1); }
extern color*			palt;
void					pixel(int x, int y);
void					pixel(int x, int y, unsigned char alpha);
unsigned char*			ptr(int x, int y);
int						rawinput();
void					rawredraw();
void					rectb(rect rc); // Draw rectangle border
void					rectb(rect rc, color c1);
void					rectb(rect rc, unsigned char c1);
void					rectf(rect rc); // Draw rectangle area. Right and bottom side is one pixel less.
void					rectf(rect rc, unsigned char c1);
void					rectf(rect rc, color c1);
void					rectf(rect rc, color c1, unsigned char alpha);
void					setcaption(const char* string);
void					setclip(rect rc);
void					setclip();
void					setlayer(callback v);
void					settimer(unsigned milleseconds);
int						sysinput(bool redraw = false);
const char*				skiptr(const char* string);
void					showlayer();
void					text(int x, int y, const char* string, int count = -1, unsigned flags = 0);
int						text(rect rc, const char* string, unsigned state = 0);
int						textc(int x, int y, int width, const char* string, int count = -1, unsigned flags = 0);
int						textbc(const char* string, int width, int* string_width = 0);
int						textlb(const char* string, int index, int width, int* line_index = 0, int* line_count = 0);
int						textf(int x, int y, int width, const char** string, color c1, int* max_width);
int						texth();
int						texth(const char* string, int width);
int						textw(int sym);
int						textw(const char* string, int count = -1);
int						textw(rect& rc, const char* string);
void					write(const char* url, unsigned char* bits, int width, int height, int bpp, int scanline, color* pallette);
}
int						isqrt(int num);