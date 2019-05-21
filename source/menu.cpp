#include "crt.h"
#include "draw.h"
#include "menu.h"

const menu* menu::find(int id) const {
	for(auto p = this; p->id; p++) {
		if(p->id == id)
			return p;
	}
	return 0;
}

int menu::getnextid(int focus, int direction) const {
	auto p = find(focus);
	if(!p)
		return focus;
	switch(direction) {
	case KeyUp:
		if(p > this)
			return p[-1].id;
		return this[zlen(this) - 1].id;
	case KeyDown:
		if(p[1].id)
			return p[1].id;
		return id;
	default:
		return focus;
	}
}