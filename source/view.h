#pragma once

#include "draw.h"
#include "main.h"
#include "menu.h"
#include "screenshoot.h"
#include "sprite.h"

namespace draw {
typedef void(*infoproc)(item*);
void				abilities(int x, int y, creature* pc);
void				avatar(int x, int y, creature* pc, unsigned flags, item* current_item);
void				background(int rid);
int					button(int x, int y, int width, int id, unsigned flags, const char* name);
int					ciclic(int range, int speed = 1);
int					flatb(int x, int y, int width, int id, unsigned flags, const char* string);
void				focusing(const rect& rc, item* pid);
rect				form(rect rc, int count = 1);
void				generation();
unsigned			getfstate(int id, int focus);
infoproc			getmode();
item*				getnext(item* pitm, int key);
sprite*				gres(resource_s id);
void				greenbar(rect rc, int vc, int vm);
int					header(int x, int y, const char* text);
void				imagex(int x, int y, const sprite* res, int id, unsigned flags, int percent, unsigned char shadow);
void				initialize();
void				invertory(int x, int y, creature* pc, item* current_item);
void				itemicn(int x, int y, item itm, unsigned char alpha = 0xFF, int spell = 0);
void				itemicn(int x, int y, item* pitm, bool invlist = false, unsigned flags = 0, void* current_item = 0);
int					linetext(int x, int y, int width, int id, unsigned flags, const char* name);
void				linetext(int x, int y, int width, int id, unsigned flags, const char* name, const char* name2);
void				logs();
command_s			mainmenu();
void				portrait(int x, int y, creature* pc);
void				redraw();
void				resetres();
void				setbigfont();
void				setmode(infoproc mode);
void				setsmallfont();
void				skills(int x, int y, creature* pc);
void				textb(int x, int y, const char* string, int count = -1);
int					textb(rect rc, const char* string, unsigned flags = 0);
extern unsigned		frametick;
}
namespace colors {
extern color		title;
namespace info {
extern color		text;
}
}
const int			walls_frames = 9;
const int			walls_count = 6;
const int			door_offset = 1 + walls_frames * walls_count;
const int			decor_offset = door_offset + 9;
const int			decor_count = 19;
const int			decor_frames = 10;
const int			scrx = 22 * 8;
const int			scry = 15 * 8;