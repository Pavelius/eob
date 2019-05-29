#include "view.h"

const int mpg = 8;

static void show_marker(int x, int y) {
	draw::line(x, y, x + mpg, y + mpg, colors::red);
	draw::line(x + mpg, y, x, y + mpg, colors::red);
}

static void show_camera_pos() {
	draw::state push;
	draw::fore = colors::red;
	auto direct = game::getdirection();
	auto camera = game::getcamera();
	auto x1 = gx(camera)*mpg;
	auto y1 = gy(camera)*mpg;
	auto x2 = x1 + mpg;
	auto y2 = y1 + mpg;
	auto cx = x1 + mpg / 2;
	auto cy = y1 + mpg / 2;
	switch(direct) {
	case Left:
		draw::line(x1, cy, x2, cy);
		draw::pixel(x1 + 1, cy - 1);
		draw::pixel(x1 + 1, cy + 1);
		break;
	case Right:
		draw::line(x1, cy, x2, cy);
		draw::pixel(x2 - 1, cy - 1);
		draw::pixel(x2 - 1, cy + 1);
		break;
	case Up:
		draw::line(cx, y1, cx, y2);
		draw::pixel(cx - 1, y1 + 1);
		draw::pixel(cx + 1, y1 + 1);
		break;
	case Down:
		draw::line(cx, y1, cx, y2);
		draw::pixel(cx - 1, y2 - 1);
		draw::pixel(cx + 1, y2 - 1);
		break;
	}
}

cell_s tget(const dungeon& location, int x, int y) {
	if(!location.is(location.getindex(x, y), CellExplored))
		return CellWall;
	auto t = location.get(x, y);
	switch(t) {
	case CellUnknown:
	case CellPortal:
		return CellWall;
	default:
		return t;
	}
}

static void fill_neighboard(const dungeon& location, int x, int y, cell_s* nb) {
	nb[0] = tget(location, x - 1, y);
	nb[1] = tget(location, x, y - 1);
	nb[2] = tget(location, x + 1, y);
	nb[3] = tget(location, x, y + 1);
	//
	nb[4] = tget(location, x - 1, y + 1);
	nb[5] = tget(location, x - 1, y - 1);
	nb[6] = tget(location, x + 1, y - 1);
	nb[7] = tget(location, x + 1, y + 1);
}

static void fill_side(int x, int y, int dx, color border, cell_s* nb, cell_s t1) {
	int x2 = x + mpg - 1;
	int y2 = y + mpg - 1;
	if(nb[0] == t1)
		draw::line(x + dx, y, x + dx, y2, border);
	else if(nb[1] == t1)
		draw::line(x, y + dx, x2, y + dx, border);
	else if(nb[2] == t1)
		draw::line(x2 - dx, y, x2 - dx, y2, border);
	else if(nb[3] == t1)
		draw::line(x, y2 - dx, x2, y2 - dx, border);
}

static void fill_border(int x, int y, int dx, color border, cell_s* nb, cell_s t1) {
	int x2 = x + mpg - 1;
	int y2 = y + mpg - 1;
	if(nb[0] != t1)
		draw::line(x + dx, y, x + dx, y2, border);
	if(nb[1] != t1)
		draw::line(x, y + dx, x2, y + dx, border);
	if(nb[2] != t1)
		draw::line(x2 - dx, y, x2 - dx, y2, border);
	if(nb[3] != t1)
		draw::line(x, y2 - dx, x2, y2 - dx, border);
	if(dx) {
		draw::state push;
		draw::fore = border;
		if(nb[0] == t1 && nb[1] == t1 && nb[5] != t1)
			draw::pixel(x, y);
		if(nb[1] == t1 && nb[2] == t1 && nb[6] != t1)
			draw::pixel(x2, y);
		if(nb[2] == t1 && nb[3] == t1 && nb[7] != t1)
			draw::pixel(x2, y2);
		if(nb[0] == t1 && nb[3] == t1 && nb[4] != t1)
			draw::pixel(x, y2);
	}
}

static void render_automap(const dungeon& location, bool fog_of_war) {
	draw::state push;
	cell_s nb[8];
	color cpass = color::create(196, 132, 72);
	color cwall = color::create(156, 104, 54);
	color bwall = color::create(100, 64, 24);
	color bpass = color::create(176, 120, 64);
	color bpits = bpass.darken();
	color cdoor = color::create(140, 88, 48);
	draw::rectf({0, 0, 320, 200}, cpass);
	for(int y = 0; y < mpy; y++) {
		for(int x = 0; x < mpx; x++) {
			if(fog_of_war && !location.is(location.getindex(x, y), CellExplored))
				continue;
			auto x1 = x * mpg;
			auto y1 = y * mpg;
			auto index = location.getindex(x, y);
			fill_neighboard(location, x, y, nb);
			switch(location.get(x, y)) {
			case CellUnknown:
				draw::rectf({x1, y1, x1 + mpg, y1 + mpg}, cwall);
				break;
			case CellWall:
				draw::rectf({x1, y1, x1 + mpg, y1 + mpg}, cwall);
				fill_border(x1, y1, 1, cdoor, nb, CellWall);
				fill_border(x1, y1, 0, bwall, nb, CellWall);
				break;
			case CellPit:
				draw::rectf({x1 + 1, y1 + 1, x1 + mpg - 2, y1 + mpg - 2}, cwall);
				draw::rectb({x1 + 1, y1 + 1, x1 + mpg - 2, y1 + mpg - 2}, bpits);
				break;
			case CellButton:
				draw::rectf({x1 + 1, y1 + 1, x1 + mpg - 2, y1 + mpg - 2}, bpass);
				draw::rectb({x1 + 1, y1 + 1, x1 + mpg - 2, y1 + mpg - 2}, bpits);
				break;
			case CellDoor:
				if(nb[0] == CellWall && nb[2] == CellWall) {
					auto yc = y1 + mpg / 2 - 1;
					draw::line(x1, yc, x1 + mpg, yc, cdoor); yc++;
					draw::line(x1, yc, x1 + mpg, yc, cdoor);
				} else {
					auto xc = x1 + mpg / 2 - 1;
					draw::line(xc, y1, xc, y1 + mpg, cdoor); xc++;
					draw::line(xc, y1, xc, y1 + mpg, cdoor);
				}
				break;
			case CellPortal:
				draw::rectf({x1, y1, x1 + mpg, y1 + mpg}, cwall);
				fill_border(x1, y1, 1, cdoor, nb, CellWall);
				fill_border(x1, y1, 0, bwall, nb, CellWall);
				fill_side(x1, y1, 2, bwall, nb, CellPassable);
				break;
			case CellStairsUp:
			case CellStairsDown:
				if(nb[0] == CellWall && nb[2] == CellWall) {
					auto yc = y1 + 1;
					draw::line(x1, yc, x1 + mpg, yc, cdoor); yc += 2;
					draw::line(x1, yc, x1 + mpg, yc, cdoor); yc += 2;
					draw::line(x1, yc, x1 + mpg, yc, cdoor); yc += 2;
				} else {
					auto xc = x1 + 1;
					draw::line(xc, y1, xc, y1 + mpg, cdoor); xc += 2;
					draw::line(xc, y1, xc, y1 + mpg, cdoor); xc += 2;
					draw::line(xc, y1, xc, y1 + mpg, cdoor); xc += 2;
				}
				break;
			}
		}
	}
}

void game::action::pause() {
	while(draw::ismodal()) {
		draw::domodal();
		switch(hot::key) {
		case KeyEscape:
			return;
		}
	}
}

void game::action::automap(dungeon& location, bool fow) {
	render_automap(location, fow);
	show_camera_pos();
	pause();
}

void draw::animation::appear(dungeon& location, short unsigned index, int radius) {
	render_automap(location, true);
	draw::screenshoot before;
	int x0 = gx(index);
	int y0 = gy(index);
	for(int y = y0 - radius; y < y0 + radius; y++) {
		if(y < 0 || y >= mpy)
			continue;
		for(int x = x0 - radius; x < x0 + radius; x++) {
			if(x < 0 || x >= mpx)
				continue;
			location.set(location.getindex(x, y), CellExplored);
		}
	}
	render_automap(location, true);
	show_marker(x0*mpg, y0*mpg);
	show_camera_pos();
	draw::screenshoot after;
	before.blend(after, 2000);
	game::action::pause();
}