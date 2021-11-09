#include "main.h"
#include "draw.h"

using namespace draw;

namespace {
struct palspr : pma {
	unsigned char		data[18][16];
};
struct renderi {
	short				x, y, z;
	short				frame[4];
	const sprite*		rdata;
	indext				index;
	short unsigned		flags[4];
	creature*			pc;
	cell_s				rec;
	unsigned char		pallette;
	short				percent;
	unsigned char		alpha;
	unsigned char		zorder;
	rect				clip;
	void				paint() const;
	void clear() { memset(this, 0, sizeof(renderi)); }
};
}
extern sprite*			gres(resource_s id);
static cell_s			render_mirror1, render_mirror2;
static resource_s		render_door_type;
static int				render_flipped_wall;
static sprite*			map_tiles;
static int				disp_damage[6];
static int				disp_hits[6][2];
static renderi			disp_data[512];
const int				distance_per_level = 4;
const int				mpg = 8;
static short unsigned	indecies[18];
static draw::surface	scaler(320, 200, 32);
static draw::surface	scaler2(320, 200, 32);

namespace colors {
color					damage = color::create(255, 255, 255);
color					fire = color::create(255, 0, 0);
}

static char	pos_levels[18] = {
	3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2,
	1, 1, 1,
	0, 0, 0
};
static int wall_sizes[18] = {
	48, 48, 48, 48, 48, 48, 48,
	80, 80, 80, 80, 80,
	128, 128, 128,
	176, 176, 176
};
static short item_distances[][2] = {
	{1000, 0},
	{1000, 0},
	{100 * 1000 / 120, 0},
	{80 * 1000 / 120, 0},
	{64 * 1000 / 120, 64},
	{48 * 1000 / 120, 64},
	{40 * 1000 / 120, 128},
	{32 * 1000 / 120, 128},
};
static point item_position[18 * 4] = {
	{-16, 56}, {0, 56}, {-42, 60}, {-22, 60},
	{16, 56}, {32, 56}, {-2, 60}, {18, 60},
	{48, 56}, {64, 56}, {38, 60}, {58, 60},
	{80, 56}, {96, 56}, {78, 60}, {98, 60},
	{112, 56}, {128, 56}, {118, 60}, {138, 60},
	{144, 56}, {160, 56}, {158, 60}, {178, 60},
	{176, 56}, {192, 56}, {198, 60}, {218, 60},
	// Level 2
	{-29, 66}, {-3, 66}, {-65, 74}, {-31, 74},
	{23, 66}, {49, 66}, {3, 74}, {37, 74},
	{75, 66}, {101, 66}, {71, 74}, {105, 74},
	{127, 66}, {153, 66}, {139, 74}, {173, 74},
	{179, 66}, {205, 66}, {207, 74}, {241, 74},
	// Level 1
	{-27, 86}, {19, 86}, {-57, 98}, {1, 98},
	{65, 86}, {111, 86}, {59, 98}, {117, 98},
	{157, 86}, {203, 86}, {175, 98}, {233, 98},
	// Level 0
	{-107, 118}, {-29, 118}, {-152, 136}, {-56, 136},
	{49, 118}, {127, 118}, {40, 136}, {136, 136},
	{205, 118}, {283, 118}, {232, 136}, {328, 136},
};

static int get_tile(cell_s id, bool mirrored) {
	if(id == CellWall && mirrored) {
		if(render_flipped_wall == -1)
			return bsdata<celli>::elements[id].frame;
		return render_flipped_wall;
	}
	return bsdata<celli>::elements[id].frame;
}

static inline int get_tile_alternate(cell_s id) {
	return decor_offset + 2 * decor_frames;
}

static renderi* get_monster_disp(creature* target) {
	if(!target)
		return 0;
	for(auto& e : disp_data) {
		if(!e.rdata)
			return 0;
		if(e.pc == target)
			return &e;
	}
	return 0;
}

static int get_party_disp(creature* target, wear_s id) {
	if(!target)
		return 0;
	int pind = target->getpartyindex();
	if(pind == -1)
		return 0;
	if(id == RightHand)
		return disp_hits[pind][0];
	else if(id == LeftHand)
		return disp_hits[pind][1];
	return 0;
}

static unsigned flip_flags(indext index, direction_s direction) {
	return ((gx(index) + gy(index) + direction) & 1) != 0 ? ImageMirrorH : 0;
}

void fast_shadow(unsigned char* d, int d_scan, int width, int height, unsigned char alpha) {
	if(!alpha)
		return;
	while(height-- > 0) {
		color* d1 = (color*)d;
		color* d2 = d1 + width;
		switch(alpha) {
		case 192:
			while(d1 < d2) {
				if(d1->a != 0xFF) {
					d1->r >>= 3;
					d1->g >>= 3;
					d1->b >>= 3;
				}
				d1++;
			}
			break;
		case 128:
			while(d1 < d2) {
				if(d1->a != 0xFF) {
					d1->r >>= 2;
					d1->g >>= 2;
					d1->b >>= 2;
				}
				d1++;
			}
			break;
		case 64:
			while(d1 < d2) {
				if(d1->a != 0xFF) {
					d1->r >>= 1;
					d1->g >>= 1;
					d1->b >>= 1;
				}
				d1++;
			}
			break;
		}
		d += d_scan;
	}
}

static void fast_fill_contour(unsigned char* d, int d_scan, int width, int height, color c1) {
	while(height-- > 0) {
		color* d1 = (color*)d;
		color* d2 = d1 + width;
		while(d1 < d2) {
			if(d1->a != 0xFF)
				*d1 = c1;
			d1++;
		}
		d += d_scan;
	}
}

void view_dungeon_reset() {
	memset(disp_damage, 0, sizeof(disp_damage));
	memset(disp_data, 0, sizeof(disp_data));
	memset(disp_hits, 0, sizeof(disp_hits));
	map_tiles = 0;
}

direction_s get_absolute_direction(direction_s d, direction_s d1) {
	switch(d) {
	case Up:
		return d1;
	case Left:
		switch(d1) {
		case Left: return Up;
		case Right: return Down;
		case Up: return Right;
		case Down: return Left;
		}
	case Right:
		switch(d1) {
		case Right: return Up;
		case Left: return Down;
		case Up: return Left;
		case Down: return Right;
		}
	case Down:
		switch(d1) {
		case Down: return Up;
		case Up: return Down;
		case Right: return Left;
		case Left: return Right;
		}
	default:
		return Center;
	}
}

static int get_dungeon_floor(dungeoni* pd, int x, int y, direction_s dir) {
	if(x < 0 || y < 0 || x >= mpx || y >= mpy)
		return -1;
	auto index = pd->getindex(x, y);
	int t = pd->get(index);
	if(t == CellButton)
		return pd->is(index, CellActive) ? 2 : 1;
	return -1;
}

static int get_compass_index(direction_s d) {
	switch(d) {
	case Right: return 1; // East
	case Down: return 2; // South
	case Left: return 3; // West
	default: return 0; // North
	}
}

static void compass() {
	draw::state push;
	draw::fore = colors::white;
	auto d = game.getdirection();
	auto i = get_compass_index(d);
	draw::image(114, 132, gres(COMPASS), i, 0);
	draw::image(79, 158, gres(COMPASS), 4 + i, 0);
	draw::image(150, 158, gres(COMPASS), 8 + i, 0);
	draw::logs();
}

static void render_player_damage(int x, int y, int hits, unsigned counter) {
	draw::state push;
	char temp[32];
	sznum(temp, hits);
	draw::image(x, y - 1, gres(THROWN), 0, (counter % 2) ? ImageMirrorH : 0);
	draw::fore = colors::damage;
	draw::text(x - draw::textw(temp) / 2, y - 3, temp);
}

static void render_player_attack(int x, int y, int hits) {
	static int side;
	draw::state push;
	char temp[32];
	if(hits == -1)
		zprint(temp, "miss");
	else
		sznum(temp, hits);
	draw::image(x, y - 1, gres(THROWN), 1, ((side++) % 2) ? ImageMirrorH : 0);
	draw::fore = colors::damage;
	draw::text(x - draw::textw(temp) / 2, y - 3, temp);
}

static unsigned char getalpha(unsigned f, unsigned range) {
	if(f < range / 2)
		return f * 2;
	return (range - f) * 2;
}

void creature::view_portrait(int x, int y) const {
	static struct elementi {
		spell_s		v;
		color		c;
	} variants[] = {{Poison, colors::green},
		{Disease, colors::green.mix(colors::red)},
		{HoldPerson, colors::red},
		{Sleep, colors::blue}
	};
	if(isinvisible() || is(Blur))
		image(x, y, gres(PORTM), getavatar(), 0, 128);
	else
		image(x, y, gres(PORTM), getavatar(), 0);
	adat<elementi*, sizeof(variants) / sizeof(variants[0])> source;
	for(auto& e : variants) {
		if(is(e.v))
			source.add(&e);
	}
	auto variant_count = source.getcount();
	if(variant_count > 0) {
		auto range = 64;
		auto pv = source.data[(frametick / range) % variant_count];
		rectf({x, y, x + 31, y + 31}, pv->c, getalpha(frametick % range, range));
	}
	int pind = this->getpartyindex();
	if(pind != -1) {
		auto v = disp_damage[pind];
		if(v)
			render_player_damage(x + 16, y + 16, v, v % 2);
	}
}

static void handicn(int x, int y, creature* pc, wear_s id, void* current_item) {
	unsigned state = 0;
	auto itm = pc->getitem(id);
	if(*itm && (!pc->isuse(*itm) || !pc->isallow(*itm, id)))
		state |= Disabled;
	draw::itemicn(x, y, itm, false, state, current_item);
	auto p = get_party_disp(pc, id);
	if(p)
		render_player_attack(x, y + 2, p);
}

void draw::avatar(int x, int y, int party_index, unsigned flags, void* current_item) {
	auto p = party[party_index];
	if(!p)
		return;
	avatar(x, y, p, flags, current_item);
}

void draw::avatar(int x, int y, creature* pc, unsigned flags, void* current_item) {
	draw::state push;
	fore = colors::black;
	setsmallfont();
	auto pn = pc->getname();
	image(x, y, gres(INVENT), 1, 0);
	image(x, y + 24, gres(INVENT), 2, 0);
	text({x + 1, y + 2, x + 62, y + 2 + draw::texth()}, pn, AlignCenterCenter);
	pc->view_portrait(x + 1, y + 9);
	// State show
	rect rc = {x, y, x + 62, y + 49};
	for(auto id = Bless; id <= LastSpellAbility; id = (spell_s)(id + 1)) {
		if(!pc->is(id))
			continue;
		switch(id) {
		case DetectEvil:
		case DetectMagic:
		case Invisibility:
		case Blur:
		case ReadLanguagesSpell:
			continue;
		case HoldPerson:
			draw::rectb(rc, colors::red);
			break;
		case Bless:
		case ProtectionFromEvil:
			draw::rectb(rc, colors::yellow);
			break;
		default:
			draw::rectb(rc, colors::green);
			break;
		}
		break;
	}
	handicn(x + 32 + 16, y + 16, pc, RightHand, current_item);
	handicn(x + 32 + 16, y + 16 + 16, pc, LeftHand, current_item);
	int hp = pc->gethits();
	int mhp = pc->gethitsmaximum();
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1i of %2i", hp, mhp);
	text({x + 2, y + 43, x + 2 + 61, y + 43 + draw::texth()}, temp, AlignCenterCenter);
	//greenbar({x + 2, y + 42, x + 62 - 1, y + 43 + 5}, hp, mhp);
	if(flags & Disabled)
		rectf({x, y, x + 64, y + 50}, colors::black, 128);
	if(flags & Checked)
		rectb({x, y, x + 62, y + 49}, colors::white.mix(colors::black, ciclic(63) * 4));
}

static unsigned get_hero_flags(int party_index) {
	auto p = party[party_index];
	if(!p)
		return Disabled;
	return (p->gethits() <= 0) ? Disabled : 0;
}

static void show_portraits(int x, int y, void* current_item) {
	avatar(x, y, 0, get_hero_flags(0), current_item);
	draw::avatar(x + 72, y, 1, get_hero_flags(1), current_item);
	draw::avatar(x, y + 52, 2, get_hero_flags(2), current_item);
	draw::avatar(x + 72, y + 52, 3, get_hero_flags(3), current_item);
}

static void render_players_info(void* current_item) {
	auto m = draw::getmode();
	if(!current_item)
		m = 0;
	if(m)
		m(current_item);
	else
		show_portraits(184, 2, current_item);
}

void draw::animation::clear() {
	memset(disp_damage, 0, sizeof(disp_damage));
	memset(disp_hits, 0, sizeof(disp_hits));
}

void draw::animation::damage(creature* target, int hits) {
	if(target->ishero()) {
		int pind = target->getpartyindex();
		if(pind != -1) {
			disp_damage[pind] = hits;
			render();
			disp_damage[pind] = 0;
		}
	} else if(target->is(StaticObject))
		render();
	else {
		auto e = get_monster_disp(target);
		if(e) {
			short unsigned flags[4];
			memcpy(flags, e->flags, sizeof(e->flags));
			for(auto i = 0; i < 4; i++)
				e->flags[i] |= ImageColor;
			render();
			memcpy(e->flags, flags, sizeof(e->flags));
		}
	}
}

// If hits == -1 the attack is missed
void draw::animation::attack(creature* attacker, wear_s slot, int hits) {
	if(attacker->ishero()) {
		auto pind = attacker->getpartyindex();
		if(pind != -1) {
			auto sdr = (pind == 0 || pind == 2) ? Left : Right;
			auto sht = bsdata<itemi>::elements[attacker->get(slot).gettype()].image.shoot;
			if(sht)
				draw::animation::thrown(attacker->getindex(), attacker->getdirection(), sht, sdr, 50, true);
			disp_hits[pind][((slot == RightHand) ? 0 : 1)] = hits;
		}
	} else {
		auto p = get_monster_disp(attacker);
		if(p) {
			attacker->setframe(p->frame, 4);
			render();
			attacker->setframe(p->frame, 5);
		}
	}
}

bool draw::settiles(resource_s type) {
	map_tiles = gres(type);
	render_flipped_wall = 1 * walls_frames;
	render_door_type = BRICK;
	render_mirror1 = CellSecrectButton;
	render_mirror2 = CellPassable;
	switch(type) {
	case FOREST:
		render_flipped_wall = -1;
		break;
	case BLUE:
		render_door_type = BLUE;
		break;
	case DROW:
		render_door_type = DROW;
		break;
	case GREEN:
		render_door_type = GREEN;
		break;
	case BRICK:
		render_mirror1 = CellPassable;
		break;
	}
	return true;
}

static dungeoni::overlayi* add_wall_decor(renderi* p, indext index, direction_s dir, int n, bool flip, bool use_flip) {
	if(n == -1)
		return 0;
	auto bd = to(game.getdirection(), dir);
	auto index_start = to(index, bd);
	if(index_start == Blocked)
		return 0;
	auto t1 = location.get(index_start);
	if(t1 == CellWall || t1 == CellStairsUp || t1 == CellStairsDown || t1 == CellPortal || t1 == CellDoor)
		return 0;
	auto povr = location.getoverlay(index_start, to(bd, Down));
	auto tile = location.gettype(povr);
	if(tile < CellPuller)
		return 0;
	auto frame = get_tile(tile, false);
	if(frame == -1)
		return 0;
	if(tile == CellPuller) {
		if(location.isactive(povr))
			frame += decor_frames;
	}
	p->frame[1] = frame + n;
	if(use_flip && flip && (render_mirror1 == tile || render_mirror2 == tile))
		p->flags[1] ^= ImageMirrorH;
	return povr;
}

static int get_throw_index(item_s type) {
	switch(type) {
	case Spear: return 6;
	case Dart: return 8;
	case Dagger: return 10;
	default: return 12;
	}
}

static void fill_item_sprite(renderi* p, item_s type, int frame = 0) {
	if(bsdata<itemi>::elements[type].image.size == 1)
		p->rdata = gres(ITEMGL);
	else
		p->rdata = gres(ITEMGS);
	p->frame[frame] = bsdata<itemi>::elements[type].image.ground;
}

static void fill_sprite(renderi* p, item_s type, direction_s drs) {
	switch(type) {
	case FireThrown: case LightingThrown: case IceThrown: case MagicThrown:
		p->frame[0] = (type - FireThrown) + 2;
		p->rdata = gres(THROWN);
		break;
	case Spear:case Dart: case Dagger: case Arrow:
		p->frame[0] = get_throw_index(type);
		p->rdata = gres(THROWN);
		if(drs == Right)
			p->flags[0] |= ImageMirrorH;
		break;
	default:
		fill_item_sprite(p, type);
		break;
	}
}

static renderi* add_cellar_items(renderi* p, int i, dungeoni::overlayi* povr) {
	if(!povr)
		return p;
	if(povr->type == CellCellar) {
		const int dy1 = 8;
		const int dy2 = 16;
		const int dy3 = 17;
		static point positions[18] = {{scrx / 2 - 48 * 3, scry / 2},
			{scrx / 2 - 48 * 2, scry / 2 - dy3},
			{scrx / 2 - 48 * 1, scry / 2 - dy3},
			{scrx / 2, scry / 2 - dy3},
			{scrx / 2 + 48 * 1, scry / 2 - dy3},
			{scrx / 2 + 48 * 2, scry / 2 - dy3},
			{scrx / 2 + 48 * 3, scry / 2 - dy3},
			// Level 2
			{scrx / 2 - 80 * 2, scry / 2 - dy2},
			{scrx / 2 - 80 * 1, scry / 2 - dy2},
			{scrx / 2, scry / 2 - dy2},
			{scrx / 2 + 80 * 1, scry / 2 - dy2},
			{scrx / 2 + 80 * 2, scry / 2 - dy2},
			// Level 1
			{scrx / 2 - 128, scry / 2 - dy1},
			{scrx / 2, scry / 2 - dy1},
			{scrx / 2 + 128, scry / 2 - dy1},
			// Level 0
			{scrx / 2, scry / 2},
			{scrx / 2, scry / 2},
			{scrx / 2, scry / 2},
		};
		item* result[4];
		auto item_count = location.getitems(result, zendof(result), povr);
		auto d = pos_levels[i] * 2;
		for(unsigned n = 0; n < item_count; n++) {
			p++;
			p->clear();
			p->x = positions[i].x + n;
			p->y = positions[i].y + n / 2;
			p->z = pos_levels[i] * distance_per_level;
			p->zorder = 2;
			p->index = povr->index;
			p->percent = item_distances[d][0];
			p->alpha = (unsigned char)item_distances[d][1];
			fill_item_sprite(p, result[n]->gettype());
		}
	}
	return p;
}

static renderi* create_wall(renderi* p, int i, indext index, int frame, cell_s rec, bool flip) {
	int n;
	// Walls render
	// | |_  5 4 7
	// | |_  6 3 8
	//   |_    2 9
	//   |     1
	static char walls_front[18] = {7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8,
		9, 9, 9,
		0, 0, 0,
	};
	static int wall_width[18] = {48, 48, 48, 48, 48, 48, 48,
		80, 80, 80, 80, 80,
		128, 128, 128,
		0, 0, 0,
	};
	static char walls_left[18] = {0, 5, 4, 0, 0, 0, 0,
		6, 3, 0, 0, 0,
		2, 0, 0,
		1, 0, 0,
	};
	static char walls_right[18] = {0, 0, 0, 0, 4, 5, 0,
		0, 0, 0, 3, 6,
		0, 0, 2,
		0, 0, 1,
	};
	static point wall_position[18] = {{scrx / 2 - 48 * 3, scry / 2},
		{scrx / 2 - 48 * 2, scry / 2},
		{scrx / 2 - 48 * 1, scry / 2},
		{scrx / 2, scry / 2},
		{scrx / 2 + 48 * 1, scry / 2},
		{scrx / 2 + 48 * 2, scry / 2},
		{scrx / 2 + 48 * 3, scry / 2},
		// Level 2
		{scrx / 2 - 80 * 2, scry / 2},
		{scrx / 2 - 80 * 1, scry / 2},
		{scrx / 2, scry / 2},
		{scrx / 2 + 80 * 1, scry / 2},
		{scrx / 2 + 80 * 2, scry / 2},
		// Level 1
		{scrx / 2 - 128, scry / 2},
		{scrx / 2, scry / 2},
		{scrx / 2 + 128, scry / 2},
		// Level 0
		{scrx / 2, scry / 2},
		{scrx / 2, scry / 2},
		{scrx / 2, scry / 2},
	};
	// Decors render
	// 9 7 3 7 9
	// 8 6 2 6 8
	// 8 5 1 5 8
	//   4 0 4
	//     ^ - party pos.
	static char decor_front[18] = {
		3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2,
		1, 1, 1,
		0, 0, 0,
	};
	static char decor_right[18] = {
		-1, -1, -1, -1, 7, 9, 9,
		-1, -1, -1, 6, 8,
		-1, -1, 5,
		-1, -1, 4,
	};
	static char decor_left[18] = {
		9, 9, 7, -1, -1, -1, -1,
		8, 6, -1, -1, -1,
		5, -1, -1,
		4, -1, -1,
	};
	bool enable;
	auto cd = game.getdirection();
	if(frame == -1)
		return p;
	// Front
	n = walls_front[i];
	if(n) {
		p->clear();
		p->x = wall_position[i].x;
		p->y = scry / 2;
		p->z = pos_levels[i] * distance_per_level;
		p->frame[0] = n + frame;
		p->rdata = map_tiles;
		p->index = index;
		p->rec = rec;
		auto front_wall = p;
		if(rec == CellDoor && i < 15) {
			p++;
			p->clear();
			p->x = wall_position[i].x;
			p->y = scry / 2;
			p->z = pos_levels[i] * distance_per_level;
			p->zorder = 1;
			p->rdata = map_tiles;
			p->index = index;
			auto e1 = map_tiles->get(door_offset + pos_levels[i] - 1);
			auto e2 = map_tiles->get(door_offset + 6 + pos_levels[i] - 1);
			auto po = location.getoverlay(to(indecies[i], to(game.getdirection(), Down)), game.getdirection());
			switch(render_door_type) {
			case BLUE:
				if(location.is(index, CellActive)) {
					if(po) {
						auto w = e1.sx - (e2.sx * 3) / 2;
						p->x -= w;
						p->zorder = 2;
						p->frame[0] = door_offset + 6 + pos_levels[i] - 1;
						p++; memcpy(p, p - 1, sizeof(p[0]));
						p->x += 2 * w;
						p->flags[0] = ImageMirrorH;
						p++;
					}
					return p;
				} else {
					auto p1 = p;
					p->frame[0] = door_offset + pos_levels[i] - 1;
					p++; memcpy(p, p - 1, sizeof(p[0]));
					p->flags[0] = ImageMirrorH;
					p->frame[0] = door_offset + pos_levels[i] - 1;
					if(po) {
						p1[0].frame[1] = door_offset + 6 + pos_levels[i] - 1;
						p1[1].frame[1] = door_offset + 6 + pos_levels[i] - 1;
						p1[1].flags[1] = ImageMirrorH;
					}
				}
				break;
			case DROW:
				p->frame[0] = door_offset + pos_levels[i] - 1;
				if(location.is(index, CellActive)) {
					auto x = wall_position[i].x - e1.ox;
					auto y = wall_position[i].y - e1.oy;
					p->y = p->y - e1.sy + e1.sy / 10;
					p->clip.set(x, y, x + e1.sx, y + e1.sy);
				}
				front_wall->frame[1] = door_offset + pos_levels[i] - 1 + 3;
				if(po)
					front_wall->frame[2] = door_offset + 6 + pos_levels[i] - 1;
				break;
			case GREEN:
				// Drop down door
				if(location.is(index, CellActive))
					p--;
				else
					p->frame[0] = door_offset + pos_levels[i] - 1;
				if(po)
					front_wall->frame[1] = door_offset + 6 + pos_levels[i] - 1;
				break;
			default:
				// Drop down door
				p->frame[0] = door_offset + pos_levels[i] - 1;
				if(location.is(index, CellActive)) {
					auto x = wall_position[i].x - e1.ox;
					auto y = wall_position[i].y - e1.oy;
					p->y = p->y - e1.sy + e1.sy / 10;
					p->clip.set(x, y, x + e1.sx, y + e1.sy);
				}
				if(po)
					front_wall->frame[1] = door_offset + 6 + pos_levels[i] - 1;
				break;
			}
		} else {
			if(i == 13) {
				auto t1 = to(index, to(cd, Down));
				if(location.get(t1) == CellDoor) {
					p->frame[1] = decor_offset + 1;
					if(flip)
						p->flags[1] = ImageMirrorH;
				}
			}
			auto povr = add_wall_decor(p, index, Down, decor_front[i], flip, true);
			p = add_cellar_items(p, i, povr);
		}
		p++;
	}
	// Left
	n = walls_left[i];
	enable = true;
	if((rec == CellStairsUp || rec == CellStairsDown) && i != 15)
		enable = false;
	if(n && enable) {
		p->clear();
		p->x = wall_position[i].x;
		if(n == 5 || n == 6)
			p->x += wall_width[i] * 2;
		else
			p->x += wall_width[i];
		p->y = scry / 2;
		p->z = pos_levels[i] * distance_per_level + 2;
		p->frame[0] = n + frame;
		p->rdata = map_tiles;
		p->index = index;
		p->rec = rec;
		add_wall_decor(p, index, Right, decor_left[i], flip, false);
		p++;
	}
	// Right
	n = walls_right[i];
	enable = true;
	if((rec == CellStairsUp || rec == CellStairsDown) && i != 17)
		enable = false;
	if(n && enable) {
		p->clear();
		p->x = wall_position[i].x;
		if(n == 5 || n == 6)
			p->x -= wall_width[i] * 2;
		else
			p->x -= wall_width[i];
		p->y = scry / 2;
		p->z = pos_levels[i] * distance_per_level + 2;
		p->flags[0] = ImageMirrorH;
		p->frame[0] = n + frame;
		p->rdata = map_tiles;
		p->index = index;
		p->rec = rec;
		p->flags[1] = ImageMirrorH;
		add_wall_decor(p, index, Left, decor_right[i], flip, false);
		p++;
	}
	return p;
}

static renderi* create_floor(renderi* p, int i, indext index, cell_s rec, bool flip) {
	static short floor_pos[18] = {
		scrx / 2 - 42 * 3, scrx / 2 - 42 * 2, scrx / 2 - 42, scrx / 2, scrx / 2 + 42, scrx / 2 + 42 * 2, scrx / 2 + 42 * 3,
		scrx / 2 - 64 * 2, scrx / 2 - 64, scrx / 2, scrx / 2 + 64, scrx / 2 + 64 * 2,
		scrx / 2 - 98, scrx / 2, scrx / 2 + 98,
		scrx / 2 - 176, scrx / 2, scrx / 2 + 176,
	};
	static char floor_frame[18] = {3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2,
		1, 1, 1,
		0, 0, 0,
	};
	auto frame = get_tile(rec, false);
	if(frame != -1) {
		p->clear();
		p->x = floor_pos[i];
		p->y = scry / 2;
		p->z = pos_levels[i] * distance_per_level + 1;
		if(flip)
			p->flags[0] = ImageMirrorH;
		if(rec == CellButton && location.is(index, CellActive))
			frame = get_tile_alternate(rec);
		p->frame[0] = floor_frame[i] + frame;
		if(bsdata<celli>::elements[rec].resource)
			p->rdata = gres(bsdata<celli>::elements[rec].resource);
		else
			p->rdata = map_tiles;
		p->index = index;
		p->rec = rec;
		p++;
	}
	return p;
}

static int get_x_from_line(int y, int x1, int y1, int x2, int y2) {
	return ((y - y1) * (x2 - x1)) / (y2 - y1) + x1;
}

static renderi* create_thrown(renderi* p, int i, int ps, item_s rec, direction_s dr) {
	static int height_sizes[8] = {120, 96, 71, 64, 48, 40, 30, 24};
	p->clear();
	int m = pos_levels[i];
	int d = pos_levels[i] * 2 + (1 - ps / 2);
	int h = height_sizes[d] / 6 - height_sizes[d];
	switch(dr) {
	case Left:
		p->y = 24 + d * 2;
		//p->y = 30 + d * 2;
		p->x = get_x_from_line(p->y, (176 - 72) / 2 + 14, 24, (176 - 32) / 2 + 6, 40);
		break;
	case Right:
		p->y = 24 + d * 2;
		//p->y = 30 + d * 2;
		p->x = get_x_from_line(p->y, (176 - 72) / 2 + 72 - 14, 24, (176 - 32) / 2 + 32 - 6, 40);
		break;
	default:
		p->x = 176 / 2;
		p->y = 40 + d;
		break;
	}
	p->z = pos_levels[i] * distance_per_level + (1 - ps / 2);
	p->index = indecies[i];
	fill_sprite(p, rec, dr);
	p->percent = item_distances[d][0];
	p->alpha = (unsigned char)item_distances[d][1];
	p++;
	p->rdata = 0;
	return p;
}

static renderi* create_items(renderi* p, int i, indext index, direction_s dr) {
	item* result[64];
	int item_count = location.getitems(result, zendof(result), index);
	for(int n = 0; n < item_count; n++) {
		auto it = *result[n];
		int ps = game.getside(location.getitemside(result[n]), dr);
		int d = pos_levels[i] * 2 + (1 - ps / 2);
		p->clear();
		p->x = item_position[i * 4 + ps].x;
		p->y = item_position[i * 4 + ps].y;
		p->z = pos_levels[i] * distance_per_level + 1 + (1 - ps / 2);
		p->index = index;
		p->percent = item_distances[d][0];
		p->alpha = (unsigned char)item_distances[d][1];
		fill_item_sprite(p, it.gettype());
		p++;
	}
	return p;
}

static renderi* create_monsters(renderi* p, int i, indext index, direction_s dr, bool flip) {
	creature* result[4]; location.getmonsters(result, index, dr);
	for(int n = 0; n < 4; n++) {
		auto pc = result[n];
		if(!pc)
			continue;
		auto size = pc->getsize();
		auto dir = get_absolute_direction(dr, pc->getdirection());
		int d = pos_levels[i] * 2 - (n / 2);
		p->clear();
		if(size == Large) {
			p->x = item_position[i * 4 + 0].x + (item_position[i * 4 + 1].x - item_position[i * 4 + 0].x) / 2;
			p->y = item_position[i * 4 + 0].y + (item_position[i * 4 + 3].y - item_position[i * 4 + 0].y) / 2;
			p->z = pos_levels[i] * distance_per_level - 1;
			d = pos_levels[i] * 2 - 1;
		} else {
			p->x = item_position[i * 4 + n].x;
			p->y = item_position[i * 4 + n].y;
			p->z = pos_levels[i] * distance_per_level + 1 - (n / 2);
		}
		p->percent = item_distances[d][0];
		p->alpha = (unsigned char)item_distances[d][1];
		p->rdata = gres(pc->getres());
		if(!p->rdata)
			continue;
		p->pc = pc;
		p->pallette = pc->getpallette();
		unsigned flags = 0;
		// Анимируем активных монстров
		if(((p->x + draw::frametick) / 16) % 2) {
			p->x++;
			p->y++;
		}
		switch(dir) {
		case Left:
			pc->setframe(p->frame, flip ? 2 : 1);
			break;
		case Right:
			pc->setframe(p->frame, flip ? 2 : 1);
			flags |= ImageMirrorH;
			break;
		case Up:
			pc->setframe(p->frame, 3);
			if(flip)
				flags ^= ImageMirrorH;
			break;
		case Down:
			pc->setframe(p->frame, 0);
			if(flip)
				flags ^= ImageMirrorH;
			break;
		}
		for(int i = 0; i < 4; i++)
			p->flags[i] = flags;
		p++;
	}
	return p;
}

static void prepare_draw(indext index, direction_s dr) {
	static char offset_north[18][2] = {{-3, -3}, {-2, -3}, {-1, -3}, {0, -3}, {1, -3}, {2, -3}, {3, -3},
		{-2, -2}, {-1, -2}, {0, -2}, {1, -2}, {2, -2},
		{-1, -1}, {0, -1}, {1, -1},
		{-1, 0}, {0, 0}, {1, 0}
	};
	static char offset_west[18][2] = {{-3, 3}, {-3, 2}, {-3, 1}, {-3, 0}, {-3, -1}, {-3, -2}, {-3, -3},
		{-2, 2}, {-2, 1}, {-2, 0}, {-2, -1}, {-2, -2},
		{-1, 1}, {-1, 0}, {-1, -1},
		{0, 1}, {0, 0}, {0, -1}
	};
	static char offset_south[18][2] = {{3, 3}, {2, 3}, {1, 3}, {0, 3}, {-1, 3}, {-2, 3}, {-3, 3},
		{2, 2}, {1, 2}, {0, 2}, {-1, 2}, {-2, 2},
		{1, 1}, {0, 1}, {-1, 1},
		{1, 0}, {0, 0}, {-1, 0}
	};
	static char offset_east[18][2] = {{3, -3}, {3, -2}, {3, -1}, {3, 0}, {3, 1}, {3, 2}, {3, 3},
		{2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2},
		{1, -1}, {1, 0}, {1, 1},
		{0, -1}, {0, 0}, {0, 1}
	};
	int x = gx(index);
	int y = gy(index);
	char *offsets;
	switch(dr) {
	case Up:
		offsets = (char*)offset_north;
		break;
	case Down:
		offsets = (char*)offset_south;
		break;
	case Left:
		offsets = (char*)offset_west;
		break;
	case Right:
		offsets = (char*)offset_east;
		break;
	default:
		return;
	}
	auto p = disp_data;
	// walls
	for(int i = 0; i < 18; i++) {
		int x1 = x + offsets[i * 2 + 0];
		int y1 = y + offsets[i * 2 + 1];
		bool mr = ((x1 + y1 + game.getdirection()) & 1) != 0;
		if(x1 < 0 || y1 < 0 || x1 >= mpx || y1 >= mpy) {
			p = create_wall(p, i, Blocked, get_tile(CellWall, mr), CellWall, !mr);
			continue;
		}
		auto index = location.getindex(x1, y1);
		auto tile = location.get(index);
		auto tilt = location.gettype(tile);
		indecies[i] = index;
		if(tilt != CellWall && tilt != CellStairsUp && tilt != CellStairsDown) {
			if(tilt != CellDoor) {
				if(location_above.get(index) == CellPit)
					p = create_floor(p, i, index, CellPitUp, mr);
			}
			p = create_items(p, i, index, dr);
			p = create_monsters(p, i, index, dr, mr);
		}
		auto& et = bsdata<celli>::elements[tile];
		if(et.flags.is(LookWall))
			p = create_wall(p, i, index, get_tile(tile, mr), tile, mr);
		else if(et.flags.is(LookObject))
			p = create_floor(p, i, index, tile, mr);
	}
	p->rdata = 0;
}

bool dungeoni::isvisible(indext index) {
	for(auto i : indecies) {
		if(i == index)
			return true;
	}
	return false;
}

static int compare_drawable(const void* p1, const void* p2) {
	renderi* e1 = *((renderi**)p1);
	renderi* e2 = *((renderi**)p2);
	if(e1->z != e2->z)
		return e2->z - e1->z;
	else if(e1->zorder != e2->zorder)
		return e1->zorder - e2->zorder;
	else if(e1->y != e2->y)
		return e1->y - e2->y;
	else if(e1->x != e2->x)
		return e1->x - e2->x;
	return e2 - e1;
}

static void imagex(int x, int y, const sprite* res, int id, unsigned flags, int percent, unsigned char shadow) {
	const sprite::frame& f = res->get(id);
	int sx = f.sx;
	int sy = f.sy;
	int ssx = f.sx * percent / 1000;
	int ssy = f.sy * percent / 1000;
	int sox = f.ox * percent / 1000;
	int soy = f.oy * percent / 1000;
	unsigned flags_addon = (flags & ImagePallette);
	if(true) {
		draw::state push;
		draw::canvas = &scaler;
		draw::fore.r = draw::fore.g = draw::fore.b = 0; draw::fore.a = 0xFF;
		draw::rectf({0, 0, sx, sy});
		if(flags & ImageMirrorH)
			draw::image(sx, 0, res, id, ImageMirrorH | ImageNoOffset | flags_addon);
		else
			draw::image(0, 0, res, id, ImageNoOffset | flags_addon);
	}
	blit(scaler2, 0, 0, ssx, ssy, 0, scaler, 0, 0, sx, sy);
	fast_shadow(scaler2.bits, scaler2.scanline, ssx, ssy, shadow);
	if(flags & ImageColor)
		fast_fill_contour(scaler2.bits, scaler2.scanline, ssx, ssy, colors::white);
	if(flags & ImageMirrorH)
		blit(*draw::canvas, x - ssx + sox, y - soy, ssx, ssy, ImageTransparent, scaler2, 0, 0);
	else
		blit(*draw::canvas, x - sox, y - soy, ssx, ssy, ImageTransparent, scaler2, 0, 0);
}

void add_color_data(pma* result, const unsigned char* bitdata) {
	palspr* epr = static_cast<palspr*>(result);
	epr->name[0] = 'C';
	epr->name[1] = 'O';
	epr->name[2] = 'L';
	epr->name[3] = 0;
	epr->size = sizeof(epr);
	epr->count = 18;
	for(auto x = 0; x < 18; x++) {
		for(auto y = 0; y < 16; y++)
			epr->data[x][y] = bitdata[(200 - 16 + y) * 320 + (320 - 18 + x)];
	}
}

void renderi::paint() const {
	color pal[256];
	auto push_pal = palt;
	unsigned flags_addon = 0;
	if(pallette) {
		auto s1 = (palspr*)rdata->getheader("COL");
		if(s1) {
			auto& fr = rdata->get(0);
			auto pa = (color*)rdata->offs(fr.pallette);
			memcpy(pal, pa, sizeof(pal));
			draw::palt = pal;
			for(auto i = 0; i < 16; i++) {
				auto i1 = s1->data[0][i];
				if(!i1)
					break;
				pal[i1] = pa[s1->data[pallette][i]];
			}
			flags_addon |= ImagePallette;
		}
	}
	for(int i = 0; i < 4; i++) {
		if(i && !frame[i])
			break;
		if(!percent && (flags[i] & ImageColor) == 0)
			image(x, y, rdata, frame[i], flags[i] | flags_addon);
		else
			imagex(x, y, rdata, frame[i], flags[i] | flags_addon, percent, alpha);
	}
	palt = push_pal;
}

static void render_screen() {
	// Points of view like this:
	//
	// A|B|C|D|E|F|G
	//   - - - - -
	//   H|I|J|K|L
	//     - - -
	//     M|N|O
	//     - - -
	//     P|^|Q
	//
	// '^' in the picture is the party position, facing north.
	// '|' and '-' in the above picture resembled walls.
	// There are a total of 25 different wall positions.
	// To render all the walls correctly 17 maze positions must be read(A-Q).
	//
	// Walls render
	// | |_  4 3 6
	// | |_  5 2 7
	//   |_    1 8
	//   |     0
	// Decors render
	// 9 7 3 7 9
	// 8 6 2 6 8
	// 8 5 1 5 8
	//   4 0 4
	//     ^ - party pos.
	// Size by levels:
	// Each size (S) covert front (F) left side (L) and right side (R) size of lower level.
	// So FS1 = LS2 + FS2 + RS2
	// 4 - 32x24, side has 8, only walls and items
	// 3 - 48x37, side has 16
	// 2 - 80x59, side has 24
	// 1 - 128x96, side has 24
	// 0 - 176x120 - background
	draw::state push;
	renderi* zorder[512];
	unsigned flags = flip_flags(game.getcamera(), game.getdirection());
	//draw::image(scrx/2((flags&ImageMirrorH) != 0) ? 22 * 8 : 0, 0, map_tiles, 0, flags);
	draw::image(scrx / 2, scry / 2, map_tiles, 0, flags);
	draw::setclip({0, 0, scrx, scry});
	renderi** pz = zorder;
	for(renderi* p = disp_data; p->rdata; p++)
		*pz++ = p;
	qsort(zorder, pz - zorder, sizeof(zorder[0]), compare_drawable);
	for(auto p1 = zorder; p1 < pz; p1++) {
		renderi* p = *p1;
		if(p->clip) {
			draw::state push;
			draw::setclip(p->clip);
			p->paint();
		} else
			p->paint();
	}
}

void draw::animation::update() {
	prepare_draw(game.getcamera(), game.getdirection());
}

renderi* get_last_disp() {
	for(auto& e : disp_data) {
		if(!e.rdata)
			return &e;
	}
	return 0;
}

static int get_index_pos(short unsigned index) {
	for(int i = 0; i < 18; i++) {
		if(indecies[i] == index)
			return i;
	}
	return -1;
}

int draw::animation::thrownstep(indext index, direction_s dr, item_s itype, direction_s sdr, int wait) {
	index = to(index, dr);
	if(index == Blocked)
		return index;
	int i = get_index_pos(index);
	if(i == -1)
		return index;
	auto p = get_last_disp();
	dr = to(dr, Down);
	int inc, side;
	switch(dr) {
	case Up:
		side = 2;
		inc = -2;
		break;
	default:
		side = 0;
		inc = 2;
		break;
	}
	create_thrown(p, i, side, itype, sdr);
	draw::animation::render(wait);
	int x = gx(index);
	int y = gy(index);
	if(x < 0 || x >= mpx || y < 0 || y >= mpy)
		return 0;
	if(!location.isblocked(index)) {
		create_thrown(p, i, side + inc, itype, sdr);
		draw::animation::render(wait);
	}
	p->rdata = 0;
	return index;
}

int draw::animation::thrown(indext index, direction_s dr, item_s type, direction_s sdr, int wait, bool block_monsters) {
	for(int i = 0; i < 3; i++) {
		int i2 = thrownstep(index, dr, type, sdr, wait);
		if(i2 == Blocked || location.isblocked(i2))
			break;
		index = i2;
		if(block_monsters) {
			creature* source[4]; location.getmonsters(source, i2, dr);
			if(source[0] || source[1] || source[2] || source[3])
				return i2;
		}
	}
	return index;
}

void draw::animation::render(int pause, bool show_screen, void* current_item, const imagei* pi, bool show_back) {
	if(show_screen)
		background(PLAYFLD);
	if(show_screen)
		render_screen();
	if(pi) {
		image(0, 0, gres(BORDER), 0, 0);
		image(8, 8, gres(pi->res), pi->frame, 0);
	}
	compass();
	render_players_info(current_item);
	if(pause) {
		redraw();
		sleep(pause);
	}
}