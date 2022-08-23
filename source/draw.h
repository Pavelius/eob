#include "color.h"
#include "point.h"

#pragma once

enum input_events {
	// input events
	InputSymbol=1, InputTimer, InputKeyUp, InputUpdate, InputNoUpdate,
	// control keys
	MouseLeft = 0x80, MouseLeftDBL, MouseRight,
	MouseMove, MouseWheelUp, MouseWheelDown,
	KeyLeft, KeyRight, KeyUp, KeyDown, KeyPageUp, KeyPageDown, KeyHome, KeyEnd,
	KeyBackspace, KeyEnter, KeyDelete, KeyEscape, KeySpace, KeyTab,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	FirstKey = MouseLeft,
	FirstMouse = MouseLeft, LastMouse = MouseWheelDown,
	// support
	CommandMask = 0x000000FF,
	// misc events can be combination with previous
	Ctrl = 0x00000100,
	Alt = 0x00000200,
	Shift = 0x00000400,
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
namespace hot {
extern int				animate;
extern int				key; // [in] if pressed key or mouse this field has key
extern point			mouse; // current mouse coordinates
extern bool				pressed; // flag if any of mouse keys is pressed
extern int				param; // command or input event parameter
}
namespace draw {
typedef void(*callback)();
extern rect				clipping;
extern color			fore;
extern point			caret;
struct pma {
	char				name[4]; // Identifier of current block
	int					size; // Size of all block
	int					count; // Count of records in this block
	int					find(const char* name) const;
	const pma*			getheader(const char* name) const;
	const char*			getstring(int id) const;
};
struct sprite : pma {
	enum flagse { NoIndex = 1 };
	enum encodes { Auto, RAW, RLE, ALC, RAW8, RLE8 };
	struct frame {
		short int		sx, sy;
		short int		ox, oy;
		encodes			encode;
		unsigned		pallette;
		unsigned		offset;
		rect			getrect(int x, int y, unsigned flags) const;
	};
	struct cicle {
		short unsigned	start;
		short unsigned	count;
	};
	short int			width; // common width of all frames (if applicable)
	short int			height; // common height of all frames (if applicable)
	short int			ascend;
	short int			descend;
	short unsigned		flags; // must be zero
	unsigned			cicles; // count of anim structure
	unsigned			cicles_offset;
	frame				frames[1];
	//
	explicit operator bool() const;
	//
	frame&				add();
	frame&				addlast();
	void*				add(const void* data, int count);
	int					esize() const;
	const unsigned char* edata() const;
	int					ganim(int index, int tick);
	const frame&		get(int index) const;
	inline cicle*		gcicle(int index) { return (cicle*)offs(cicles_offset) + index; }
	inline int			gindex(int index) const { return *((short unsigned*)((cicle*)offs(cicles_offset) + cicles) + index); }
	int					glyph(unsigned sym) const;
	const unsigned char* offs(unsigned o) const { return (unsigned char*)this + o; }
	void				setup(int count, int pallette_count = 0, int cicles = 0, int cicles_indexes = 0);
	int					store(const unsigned char* p, int width, int w, int h, int ox, int oy, sprite::encodes mode, unsigned char shadow_index = 1, color* pallette = 0, int frame_index = -1, unsigned char transparent_index = 0);
	void				write(const char* url, const pma* next) const;
}; 
struct surface {
	struct plugin {
		const char*		name;
		const char*		filter;
		plugin*			next;
		static plugin*	first;
		//
		plugin(const char* name, const char* filter);
		//
		virtual bool	decode(unsigned char* output, int output_bpp, const unsigned char* input, unsigned input_size, color* input_pallette) = 0;
		virtual bool	inspect(int& w, int& h, int& bpp, const unsigned char* input, unsigned size) = 0;
	};
	int					width;
	int					height;
	int					scanline;
	int					bpp;
	unsigned char*		bits;
	surface();
	surface(int width, int height, int bpp);
	surface(const char* url, color* pallette = 0);
	~surface();
	operator bool() const { return bits != 0; }
	static unsigned char* allocator(unsigned char* bits, unsigned size);
	void				clear() { resize(0, 0, 0, true); }
	void				convert(int bpp, color* pallette);
	void				flipv();
	unsigned char*		ptr(int x, int y) { return bits + y * scanline + x * (bpp / 8); }
	bool				read(const char* url, color* pallette = 0, int need_bpp = 0);
	void				resize(int width, int height, int bpp, bool alloc_memory);
	void				write(const char* url, color* pallette);
};
struct screenshoot : point, surface {
	screenshoot(bool fade = false);
	screenshoot(rect rc, bool fade = false);
	~screenshoot();
	void				blend(draw::surface& e, unsigned delay);
	void				restore();
};
struct state {
	state();
	~state();
private:
	color				fore;
	const sprite*		font; // glyph font
	color*				palt;
	struct surface*		canvas;
	rect				clip;
};
int						alignedh(const rect& rc, const char* string, unsigned state);
int						aligned(int x, int width, unsigned flags, int dx);
void					blit(surface& dest, int x, int y, int width, int height, unsigned flags, surface& dc, int xs, int ys);
void					blit(surface& dest, int x, int y, int width, int height, unsigned flags, surface& source, int x_source, int y_source, int width_source, int height_source);
void					breakmodal(int result);
void					buttoncancel();
void					buttonok();
void					buttonparam();
extern surface*			canvas;
extern point			caret;
bool					create(int width, int height);
extern callback			domodal;
void					execute(callback proc, int param = 0);
extern const sprite*	font; // glyph font
int						getbpp();
int						getheight();
int						getresult();
int						getwidth();
void					glyph(int x, int y, int sym, unsigned flags);
void					image(int x, int y, const sprite* e, int id, int flags, unsigned char alpha = 0xFF);
void					image(int x, int y, const sprite* e, int id, int flags, unsigned char alpha, color* pal);
bool					ismodal();
void					line(int x1, int y1, int x2, int y2); // Draw line
void					line(int x1, int y1, int x2, int y2, color c1); // Draw line
inline void				line(point p1, point p2, color c1) { line(p1.x, p1.y, p2.x, p2.y, c1); }
extern color*			palt;
void					pixel(int x, int y);
void					pixel(int x, int y, unsigned char alpha);
unsigned char*			ptr(int x, int y);
void					rawinput();
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
void					settimer(unsigned milleseconds);
const char*				skiptr(const char* string);
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
namespace metrics {
extern draw::sprite*	font;
extern int				padding;
}
int						isqrt(int num);