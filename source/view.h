#include "draw.h"
#include "main.h"

#pragma once

namespace draw {
struct palspr : pma {
	unsigned char		data[18][16];
};
typedef void(*infoproc)(item*);
void					abilities(int x, int y, creature* pc);
void					avatar(int x, int y, int party_index, unsigned flags, item* current_item);
void					avatar(int x, int y, creature* pc, unsigned flags, item* current_item);
void					background(int rid);
int						buttonm(int x, int y, int width, const cmd& ev, const char* name);
int						buttont(int x, int y, int width, const cmd& ev, const char* name);
void					buttont(int x, int y, int width, const cmd& ev, const char* name, const char* name2);
void					choose(const menu* source);
int						ciclic(int range, int speed = 1);
void					closeform();
int						flatb(int x, int y, int width, unsigned flags, const char* string);
void					focusing(const rect& rc, void* v, unsigned param = 0);
inline void				focusing(const rect& rc, const cmd& v, unsigned param = 0) { focusing(rc, (void*)v.focus, param); }
rect					form(rect rc, int count = 1, bool focused = false, bool pressed = false);
infoproc				getmode();
void*					getfocus();
sprite*					gres(resource_s id);
void					greenbar(rect rc, int vc, int vm);
int						header(int x, int y, const char* text);
void					imagex(int x, int y, const sprite* res, int id, unsigned flags, int percent, unsigned char shadow);
void					initialize();
void					invertory(int x, int y, creature* pc, item* current_item);
bool					isfocus(void* ev, unsigned param = 0);
inline bool				isfocus(const cmd& v) { return isfocus((void*)v.focus); }
void					itemicn(int x, int y, item itm, unsigned char alpha = 0xFF, int spell = 0);
void					itemicn(int x, int y, item* pitm, bool invlist = false, unsigned flags = 0, void* current_item = 0);
void					logs();
bool					navigate(bool can_cancel = false);
void					openform();
void					redraw();
void					resetres();
void					setbigfont();
void					setmode(infoproc mode);
void					setsmallfont();
void					skills(int x, int y, creature* pc);
void					textb(int x, int y, const char* string, int count = -1);
int						textb(rect rc, const char* string, unsigned flags = 0);
extern unsigned			frametick;
}
namespace colors {
extern color			title;
namespace info {
extern color			text;
}
}