#include "command.h"
#include "draw.h"
#include "main.h"

using namespace draw;

struct render_control {
	item*				pid;
	rect				rc;
};

static render_control	objects[48];
static render_control*	current;

static render_control* getby(item* pid) {
	for(auto& e : objects) {
		if(!e.pid)
			return 0;
		if(e.pid == pid)
			return &e;
	}
	return 0;
}

static render_control* getfirst() {
	for(auto& e : objects) {
		if(!e.pid)
			return 0;
		return &e;
	}
	return 0;
}

static render_control* getlast() {
	auto p = objects;
	for(auto& e : objects) {
		if(!e.pid)
			break;
		p = &e;
	}
	return p;
}

static point center(const rect& rc) {
	return{(short)(rc.x1 + rc.width() / 2), (short)(rc.y1 + rc.height() / 2)};
}

static int distance(point p1, point p2) {
	int dx = p1.x - p2.x;
	int dy = p1.y - p2.y;
	return isqrt(dx*dx + dy * dy);
}

static render_control* getnextfocus(item* pid, int key) {
	if(!key)
		return 0;
	auto pc = getby(pid);
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
	point p1 = center(pe->rc);
	while(true) {
		pc += inc;
		if(pc > pl)
			pc = objects;
		else if(pc < objects)
			pc = pl;
		if(pe == pc) {
			if(r1)
				return r1;
			return pe;
		}
		point p2 = center(pc->rc);
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

COMMAND(hot_clear) {
	current = objects;
}

void draw::focusing(const rect& rc, item* pid) {
	if(!current
		|| current >= objects + sizeof(objects) / sizeof(objects[0]))
		current = objects;
	current[0].rc = rc;
	current[0].pid = pid;
	current++;
	current[0].pid = 0;
}

item* draw::getnext(item* pitem, int key) {
	auto p = getnextfocus(pitem, key);
	if(p)
		return p->pid;
	return pitem;
}