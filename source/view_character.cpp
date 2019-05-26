#include "view.h"

using namespace draw;

static const char* get_race(char* result, const char* result_maximum, creature* p) {
	return szprint(result, result_maximum, "%1 %2", getstr(p->getrace()), getstr(p->getgender()));
}

static int number(int x, int y, const char* name, int v1) {
	char temp[32];
	text(x, y, name);
	zprint(temp, "%1i", v1);
	text(x + 6 * 4, y, temp);
	return 7;
}

static int number(int x, int y, const char* name, int v1, int v2) {
	char temp[32];
	text(x, y, name);
	zprint(temp, "%1i/%2i", v1, v2);
	text(x + 6 * 4, y, temp);
	return 7;
}

static int number(int x, int y, const char* name, const combati& v) {
	char temp[64];
	text(x, y, name);
	v.damage.range(temp, zendof(temp));
	text(x + 6 * 4, y, temp);
	return 7;
}

static void sheet_head(int x, int y, creature* pc) {
	char temp[260]; pc->getname(temp, zendof(temp));
	image(x, y, gres(INVENT), 0, 0);
	pc->view_portrait(x + 4, y + 3);
	text({x + 38, y + 6, x + 38 + 82, y + 6 + draw::texth()}, temp, AlignCenterCenter);
	greenbar({x + 70, y + 16, x + 135, y + 16 + 5}, pc->gethits(), pc->gethitsmaximum());
	greenbar({x + 70, y + 25, x + 135, y + 25 + 5}, 20, 25);
}

static void blanksheet(int x, int y, creature* pc) {
	sheet_head(x, y, pc);
	rectf({x + 2, y + 36, 319, 166}, color::create(164, 164, 184));
	line(274, y + 35, 319, y + 35, color::create(208, 208, 216));
	line(319, 140, 319, 166, color::create(208, 208, 216));
	line(300, 166, 318, 166, color::create(88, 88, 116));
}

static void show_quiver(int x, int y, creature* pc) {
	draw::state push;
	draw::fore = colors::white;
	auto pi = pc->getitem(Quiver);
	if(pi && *pi) {
		char temp[32];
		draw::text(x + 50, y + 65, sznum(temp, pi->getcharges()));
	}
}

void draw::invertory(int x, int y, creature* pc, item* current_item) {
	const int dx = 18;
	const int dy = 18;
	sheet_head(x, y, pc);
	for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i + 1)) {
		int x1 = x + 11 + ((i - Backpack) % 2)*dx;
		int y1 = y + 48 + ((i - Backpack) / 2)*dy;
		itemicn(x1, y1, pc->getitem(i), true, 0, current_item);
	}
	itemicn(x + 55, y + 64, pc->getitem(Quiver), true, 0, current_item);
	itemicn(x + 128, y + 63, pc->getitem(Head), true, 0, current_item);
	itemicn(x + 117, y + 83, pc->getitem(Neck), true, 0, current_item);
	itemicn(x + 54, y + 84, pc->getitem(Body), true, 0, current_item);
	itemicn(x + 55, y + 104, pc->getitem(Elbow), true, 0, current_item);
	itemicn(x + 130, y + 102, pc->getitem(FirstBelt), true, 0, current_item);
	itemicn(x + 130, y + 120, pc->getitem(SecondBelt), true, 0, current_item);
	itemicn(x + 130, y + 138, pc->getitem(LastBelt), true, 0, current_item);
	itemicn(x + 60, y + 124, pc->getitem(RightHand), true, 0, current_item);
	itemicn(x + 54, y + 140, pc->getitem(RightRing), true, 0, current_item);
	itemicn(x + 66, y + 140, pc->getitem(LeftRing), true, 0, current_item);
	itemicn(x + 108, y + 124, pc->getitem(LeftHand), true, 0, current_item);
	itemicn(x + 107, y + 145, pc->getitem(Legs), true, 0, current_item);
	show_quiver(x, y, pc);
}

void draw::abilities(int x, int y, creature* pc) {
	char temp[260];
	blanksheet(x, y, pc);
	state push;
	fore = colors::info::text;
	int x1 = x + 4;
	int y1 = y + 54;
	header(x1, y + 42, "Character info");
	text(x1, y1, getstr(pc->getclass())); y1 += draw::texth();
	text(x1, y1, getstr(pc->getalignment())); y1 += draw::texth();
	text(x1, y1, get_race(temp, zendof(temp), pc)); y1 += draw::texth() * 2;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1)) {
		char temp[16];
		int x = x1;
		int y = y1 + i * 7;
		text(x, y, getstr(i));
		sznum(temp, pc->get(i));
		text(x + 6 * 4, y, temp);
	}
	combati e = {}; pc->get(e);
	x1 = x + 7 * 10 + 4;
	y1 += number(x1, y1, "AC", 10 - pc->getac());
	y1 += number(x1, y1, "ATT", 20 - e.bonus);
	y1 += number(x1, y1, "DAM", e);
	y1 += number(x1, y1, "SPD", pc->getspeed());
	y1 += number(x1, y1, "HP", pc->gethits(), pc->gethitsmaximum());
	x1 = x + 4;
	y1 = y + 136;
	header(x1, y1, "Class");
	header(x1 + 6 * 7, y1, "Lev");
	header(x1 + 6 * 11, y1, "Exp");
	y1 += 8;
	auto cls = pc->getclass();
	auto exp = pc->getexperience() / bsmeta<classi>::elements[cls].classes.count;
	for(int i = 0; i < 3; i++) {
		char temp[16];
		auto m = pc->getclass(cls, i);
		if(!m)
			continue;
		text(x1, y1, getstr(m));
		sznum(temp, pc->get(m));
		text(x1 + 6 * 8, y1, temp);
		sznum(temp, (int)exp);
		text(x1 + 6 * 11, y1, temp);
		y1 += 7;
	}
}

void draw::skills(int x, int y, creature* pc) {
	blanksheet(x, y, pc);
	state push;
	fore = colors::info::text;
	int x1 = x + 4;
	int y1 = y + 54;
	header(x1, y + 42, "Skills");
	for(auto i = SaveVsParalization; i <= LastSkill; i = (skill_s)(i + 1)) {
		int value = pc->get(i);
		if(value <= 0)
			continue;
		char temp[16];
		text(x1, y1, getstr(i));
		zprint(temp, "%1i%%", value);
		text(x1 + 6 * 19, y1, temp);
		y1 += 7;
	}
}