#include "main.h"
#include "draw.h"
#include "log.h"

using namespace draw;

#ifdef _DEBUG

static void test_room2(int x, int y) {
	location.set(location.getindex(x, y - 2), CellWall);
	auto po = location.add(location.getindex(x, y - 1), CellCellar, Up);
	item it(Dagger); it.setpower(Rare);
	item sp(PriestScroll); sp.setpower(Bless);
	location.add(po, sp);
	location.add(po, it);
}

static void test_dungeon(resource_s type) {
	int x = 16;
	int y = 16;
	location_above.clear();
	location.clear();
	location.head.type = BRICK;
	location.level = 1;
	draw::settiles(type);
	test_room2(x, y);
	location.addmonster(Wight, location.getindex(x, y - 1), 0, Down);
	location.addmonster(Wight, location.getindex(x, y - 1), 3, Down);
	location.addmonster(Wight, location.getindex(x, y - 1), 2, Down);
	location.stat.up.index = location.getindex(x, y);
	location.stat.up.dir = Up;
	location.finish(CellPassable);
}

static void test_room(int x, int y) {
	for(int x1 = x; x1 < x + 5; x1++)
		location.set(location.getindex(x1, y), CellWall);
	for(int y1 = y; y1 < y + 5; y1++)
		location.set(location.getindex(x, y1), CellWall);
	for(int x1 = x; x1 < x + 5; x1++)
		location.set(location.getindex(x1, y + 4), CellWall);
	for(int y1 = y; y1 < y + 5; y1++)
		location.set(location.getindex(x + 4, y1), CellWall);
	location.set(location.getindex(x + 2, y + 4), CellPortal);
	location.set(location.getindex(x + 2, y), CellButton);
	location.set(location.getindex(x + 2, y), CellDoor);
	location.add(location.getindex(x + 1, y + 1), CellDecor3, Up);
	location.add(location.getindex(x + 2, y + 1), CellDoorButton, Up);
	location.add(location.getindex(x + 2, y - 1), CellDoorButton, Down);
	location.add(location.getindex(x + 1, y + 3), CellPuller, Down);
	location.add(location.getindex(x + 3, y + 3), CellTrapLauncher, Down);
	location.add(location.getindex(x + 1, y + 1), CellSecrectButton, Left);
	location.add(location.getindex(x + 3, y + 1), CellKeyHole1, Up);
	location.add(location.getindex(x + 3, y + 1), CellMessage, Right);
	auto po = location.add(location.getindex(x + 3, y + 2), CellCellar, Right);
	item it(Dagger); it.setpower(Rare);
	item sp(PriestScroll); sp.setpower(Bless);
	location.add(po, sp);
	location.add(po, it);
	location.add(location.getindex(x + 3, y + 3), CellDecor1, Right);
	location.add(location.getindex(x + 1, y + 3), CellDecor2, Left);
	location.add(location.getindex(x + 1, y + 2), CellSecrectButton, Left);
	location.set(location.getindex(x + 3, y - 1), CellWall);
	location.set(location.getindex(x + 2, y - 2), CellButton);
	location.set(location.getindex(x, y - 2), CellBarel);
	location.set(location.getindex(x + 1, y - 2), CellWeb);
	location.set(location.getindex(x + 3, y - 2), CellWebTorned);
	location.set(location.getindex(x + 5, y - 2), CellEyeColumn);
	location.set(location.getindex(x, y - 4), CellCocon);
	location.set(location.getindex(x - 1, y - 4), CellGrave);
	location.set(location.getindex(x - 1, y - 5), CellGrave);
	location.set(location.getindex(x + 2, y - 3), CellPit);
	location.set(location.getindex(x + 2, y - 4), CellButton);
	location_above.set(location.getindex(x + 2, y - 4), CellPit);
	location_above.set(location.getindex(x + 2, y - 5), CellPit);
	location.set(location.getindex(x + 1, y - 1), CellWall);
	location.set(location.getindex(x, y - 1), CellWall);
	location.set(location.getindex(x + 3, y - 3), CellWall);
	location.set(location.getindex(x + 1, y - 3), CellWall);
}

static void test_ladder(int x, int y) {
	location.fill(location.getindex(x, y), 3, 2, CellWall);
	location.set(location.getindex(x + 1, y), CellStairsUp);
}

static void test_dungeon2(resource_s type) {
	int x = 16;
	int y = 16;
	location_above.clear();
	location.clear();
	location.head.type = type;
	location.level = 1;
	draw::settiles(type);
	test_room(x - 2, y - 2);
	test_room(x - 3 * 3, y - 3 * 3);
	test_ladder(x + 5, y + 5);
	location.dropitem(location.getindex(x, y - 1), SwordShort, 0);
	location.dropitem(location.getindex(x, y - 1), Helm, 1);
	location.dropitem(location.getindex(x, y - 1), Helm, 2);
	location.dropitem(location.getindex(x + 1, y - 1), TheifTools, 0);
	location.dropitem(location.getindex(x - 1, y - 1), TheifTools, 1);
	location.dropitem(location.getindex(x - 1, y - 1), Staff, 1);
	location.dropitem(location.getindex(x, y), Helm, 0);
	location.dropitem(location.getindex(x - 1, y), Dagger, 0);
	location.dropitem(location.getindex(x, y), HolySymbol, 1);
	location.addmonster(AntGiant, location.getindex(x, y - 6));
	location.addmonster(ClericOldMan, location.getindex(x + 1, y - 6));
	location.addmonster(Kobold, location.getindex(x - 1, y - 6));
	location.addmonster(Kobold, location.getindex(x - 2, y - 6));
	location.stat.up.index = location.getindex(x, y + 1);
	location.stat.up.dir = Up;
	location.finish(CellPassable);
	draw::setnext(game.play);
}

static creature* add_hero(int n, gender_s gender, race_s race, class_s type, alignment_s alignment) {
	if(bsdata<creature>::source.getcount() < 4)
		bsdata<creature>::source.setcount(4);
	auto p = bsdata<creature>::elements + n;
	p->create(gender, race, type, alignment);
	return p;
}

void util_main();

//static void draw_monster(int x, int y, resource_s rs, int frame, int* overlay, unsigned flags, int percent, unsigned char darkness = 0, int pallette = 0) {
//	color pal[256];
//	auto push_pal = draw::palt;
//	auto sp = draw::gres(rs);
//	if(pallette) {
//		auto s1 = (palspr*)sp->getheader("COL");
//		if(s1) {
//			auto& fr = sp->get(frame);
//			auto pa = (color*)sp->offs(fr.pallette);
//			memcpy(pal, pa, sizeof(pal));
//			draw::palt = pal;
//			for(auto i = 0; i < 16; i++) {
//				auto i1 = s1->data[0][i];
//				if(!i1)
//					break;
//				pal[i1] = pa[s1->data[pallette][i]];
//			}
//			flags |= ImagePallette;
//		}
//	}
//	if(overlay) {
//		auto pb = overlay;
//		auto pe = overlay + 4;
//		while(pb < pe) {
//			if(pb > overlay && *pb == 0)
//				break;
//			if(percent == 1000)
//				draw::image(x, y, sp, (*pb) * 6 + frame, flags);
//			else
//				draw::imagex(x, y, sp, (*pb) * 6 + frame, flags, percent, darkness);
//			pb++;
//		}
//	} else {
//		if(percent == 1000)
//			draw::image(x, y, sp, frame, flags);
//		else
//			draw::imagex(x, y, sp, frame, flags, percent, darkness);
//	}
//	draw::palt = push_pal;
//}

//static void test_monster(resource_s rs, int overlay[4]) {
//	int percent = 1000;
//	unsigned char alpha = 0;
//	unsigned flags = 0;
//	int x = 100;
//	int y = 100;
//	int pal = 2;
//	while(ismodal()) {
//		draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
//		draw_monster(x, y, rs, 0, overlay, flags, percent, 0, pal);
//		domodal();
//		switch(hot::key) {
//		case KeyEscape:
//			return;
//		case 'A':
//			draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
//			draw_monster(x, y, rs, 4, overlay, flags, percent, 0, pal);
//			draw::redraw(); sleep(500);
//			draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
//			draw_monster(x, y, rs, 5, overlay, flags, percent, 0, pal);
//			draw::redraw(); sleep(500);
//			break;
//		case 'M':
//			flags ^= ImageMirrorH;
//			break;
//		case KeyUp:
//			percent -= 100;
//			if(percent < 100)
//				percent = 100;
//			break;
//		case KeyDown:
//			percent += 100;
//			if(percent > 1000)
//				percent = 1000;
//			break;
//		}
//	}
//}

static void show_monsters() {
	static int ovr12[4] = {0, 1, 2};
	static int ovr13[4] = {0, 1, 3};
	static int ovr3[4] = {0, 2, 3};
	//test_monster(ANT, 0);
	//test_monster(GUARD1, ovr3);
	//test_monster(CLERIC2, ovr13);
	//test_monster(BLDRAGON, 0);
}

void random_heroes() {
	creature* p;
	//
	p = add_hero(0, Male, Human, Paladin, LawfulGood);
	p->set({SwordLong, Fire}, RightHand);
	p->equip({GreenRing, Regeneration});
	p->equip({Bracers, Strenght});
	//
	p = add_hero(1, Male, Dwarf, Fighter, LawfulGood);
	p->set({AxeBattle, OfSharpness}, RightHand);
	p->equip({BlueRing, Invisibility});
	//
	p = add_hero(2, Female, Elf, MageTheif, ChaoticGood);
	//p->setknown(Identify);
	p->equip({RedRing, OfWizardy});
	//item s1 = {MageScroll, ReadLanguagesSpell};
	//item s2 = {MageScroll, MagicMissile};
	//s1.setidentified(1);
	//s2.setidentified(1);
	//p->add(s1);
	//p->add(s2);
	p->equip({RedRing, ResistFire});
	p->set({Staff, BurningHands}, RightHand);
	//
	p = add_hero(3, Male, Dwarf, Cleric, LawfulGood);
	p->add({GreenPotion, BonusExperience, 2});
	p->add({GreenPotion, BonusExperience, 3});
}

void debug_dungeon2() {
	location.clear();
	location_above.clear();
	random_heroes();
	test_dungeon2(BRICK);
	draw::settiles(location.head.type);
	game.setcamera(location.getindex(16, 16), Up);
	for(unsigned i = 0; i < 4; i++) {
		auto p = (creature*)bsdata<creature>::source.ptr(i);
		p->random_equipment(0);
		party.add(p);
	}
	setnext(game.play);
}

#endif // DEBUG

static void read_adventures() {
	char temp[260]; stringbuilder sb(temp);
	for(auto& e : bsdata<adventurei>()) {
		sb.clear();
		sb.add("company/%1.txt", e.id);
		e.read(temp);
	}
}

static bool reading() {
	game.readc("company/Basic.txt");
	read_adventures();
	return log::geterrors() == 0;
}

static void clear_all() {
	bsdata<variablei>::source.clear();
	bsdata<adventurei>::source.clear();
	location.clear();
	location_above.clear();
	game.clear();
	game.clearfiles();
	party.clear();
}

void gamei::newgame() {
	srand(clock());
	//debug_dungeon2();
	//return;
	clear_all();
	if(!reading())
		return;
	for(unsigned i = 0; i < 4; i++)
		bsdata<creature>::elements[i].clear();
#ifdef _DEBUG
	random_heroes();
#else
	creature::view_party();
#endif
	if(bsdata<creature>::source.getcount() < 4) {
		for(unsigned i = 0; i < 4; i++)
			bsdata<creature>::source.add();
	}
	for(unsigned i = 0; i < 4; i++) {
		auto p = (creature*)bsdata<creature>::source.ptr(i);
		p->random_equipment(0);
		party.add(p);
	}
	//if(game.intro)
	//	answers::message(game.intro);
	draw::resetres();
	game.passtime(12 * 60);
	game.write();
	//answers::last_image.set(BUILDNGS, 6);
	setnext(game.playcity);
}

int main(int argc, char* argv[]) {
#ifdef _DEBUG
	util_main();
#endif
	if(!reading())
		return -1;
	draw::initialize();
	fore = colors::white;
	setbigfont();
	setnext(mainmenu);
	application();
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}