#include "view.h"

using namespace draw;

callback		next_proc;

static sitei first_adventure[] = {{{BRICK, {Kobold, Leech}, {KeySilver, KeyCooper}, Human}, 2, 5, 0},
{{BRICK, {Skeleton, Zombie}, {KeySilver, KeyCooper}, Human}, 2, 0, 10},
{{BRICK, {Zombie, Ghoul}, {KeySilver, KeyCooper}, Human}, 1, 0, 10},
{}};

#ifdef _DEBUG

static void test_room2(int x, int y) {
	location.set(location.getindex(x, y - 2), CellWall);
	auto po = location.setoverlay(location.getindex(x, y - 1), CellCellar, Up);
	location.add(po, Bless);
	location.add(po, item(Dagger, 50, 0, 10));
}

static void test_dungeon(resource_s type) {
	int x = 16;
	int y = 16;
	location.head.type = BRICK;
	location.level = 1;
	location_above.clear();
	location.clear();
	draw::settiles(type);
	test_room2(x, y);
	location.addmonster(Ghoul, location.getindex(x, y - 1), 0, Down);
	location.addmonster(Zombie, location.getindex(x, y - 1), 1, Down);
	location.addmonster(Skeleton, location.getindex(x, y - 1), 2, Down);
	location.addmonster(Skeleton, location.getindex(x, y - 1), 3, Down);
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
	location.setoverlay(location.getindex(x + 2, y + 1), CellDoorButton, Up);
	location.setoverlay(location.getindex(x + 2, y - 1), CellDoorButton, Down);
	location.setoverlay(location.getindex(x + 1, y + 3), CellPuller, Down);
	location.setoverlay(location.getindex(x + 3, y + 3), CellTrapLauncher, Down);
	location.setoverlay(location.getindex(x + 1, y + 1), CellSecrectButton, Left);
	location.setoverlay(location.getindex(x + 3, y + 1), CellKeyHole1, Up);
	location.setoverlay(location.getindex(x + 3, y + 1), CellMessage, Right);
	auto po = location.setoverlay(location.getindex(x + 3, y + 2), CellCellar, Right);
	location.add(po, Bless);
	location.add(po, item(Dagger, 50, 0, 10));
	location.setoverlay(location.getindex(x + 1, y + 2), CellSecrectButton, Left);
	location.set(location.getindex(x + 3, y - 1), CellWall);
	location.set(location.getindex(x + 2, y - 2), CellButton);
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
	location.head.type = BRICK;
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
}

static void random_heroes() {
	game::party[0] = creature::newhero();
	game::party[0]->create(Male, Elf, Ranger, ChaoticGood);
	game::party[1] = creature::newhero();
	game::party[1]->create(Male, Human, Fighter, LawfulGood);
	game::party[2] = creature::newhero();
	game::party[2]->create(Female, Elf, Mage, LawfulNeutral);
	game::party[3] = creature::newhero();
	game::party[3]->create(Male, Dwarf, ClericTheif, ChaoticNeutral);
	//
	game::party[0]->equip(item(SwordLong, OfFire));
	game::party[0]->equip(item(GreenRing, OfRegeneration));
	game::party[0]->add(item(KeySilver, 20, 0, 0));
	game::party[0]->add(item(PotionGreen, OfPoison));
	game::party[1]->add(item(SwordLong, OfDamage, 2));
	game::party[1]->equip(item(BlueRing, OfInvisibility));
	game::party[1]->add(item(Bow, 20, 0, 0));
	game::party[2]->add(item(MagicWand, 20, 0, 0));
	game::party[2]->add(ShokingGrasp);
	game::party[2]->equip(item(BlueRing, OfWizardy));
	game::party[2]->equip(item(BlueRing, OfMagicResistance, 1));
	game::party[3]->add(PriestScroll);
	game::party[3]->add(item(DungeonMap, 20, 0, 0));
	//
	item artifact(SwordShort, 100, 5, 100);
	artifact.setidentified(1);
	game::party[3]->add(artifact);
}

void util_main();

static void draw_monster(int x, int y, resource_s rs, int frame, int* overlay, unsigned flags, int percent, unsigned char darkness = 0, int pallette = 0) {
	color pal[256];
	auto push_pal = draw::palt;
	auto sp = draw::gres(rs);
	if(pallette) {
		auto s1 = (palspr*)sp->getheader("COL");
		if(s1) {
			auto& fr = sp->get(frame);
			auto pa = (color*)sp->offs(fr.pallette);
			memcpy(pal, pa, sizeof(pal));
			draw::palt = pal;
			for(auto i = 0; i < 16; i++) {
				auto i1 = s1->data[0][i];
				if(!i1)
					break;
				pal[i1] = pa[s1->data[pallette][i]];
			}
			flags |= ImagePallette;
		}
	}
	if(overlay) {
		auto pb = overlay;
		auto pe = overlay + 4;
		while(pb < pe) {
			if(pb > overlay && *pb == 0)
				break;
			if(percent == 1000)
				draw::image(x, y, sp, (*pb) * 6 + frame, flags);
			else
				draw::imagex(x, y, sp, (*pb) * 6 + frame, flags, percent, darkness);
			pb++;
		}
	} else {
		if(percent == 1000)
			draw::image(x, y, sp, frame, flags);
		else
			draw::imagex(x, y, sp, frame, flags, percent, darkness);
	}
	draw::palt = push_pal;
}

static void test_monster(resource_s rs, int overlay[4]) {
	int percent = 1000;
	unsigned char alpha = 0;
	unsigned flags = 0;
	int x = 100;
	int y = 100;
	int pal = 2;
	while(ismodal()) {
		draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
		draw_monster(x, y, rs, 0, overlay, flags, percent, 0, pal);
		domodal();
		switch(hot::key) {
		case KeyEscape:
			return;
		case Alpha + 'A':
			draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
			draw_monster(x, y, rs, 4, overlay, flags, percent, 0, pal);
			draw::redraw(); sleep(500);
			draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
			draw_monster(x, y, rs, 5, overlay, flags, percent, 0, pal);
			draw::redraw(); sleep(500);
			break;
		case Alpha + 'M':
			flags ^= ImageMirrorH;
			break;
		case KeyUp:
			percent -= 100;
			if(percent < 100)
				percent = 100;
			break;
		case KeyDown:
			percent += 100;
			if(percent > 1000)
				percent = 1000;
			break;
		}
	}
}
#endif // DEBUG

static void newgame() {
	game::setcamera(Blocked);
	creature::view_party();
	draw::resetres();
	dungeon::create(1, first_adventure);
	game::write();
	game::enter(1, 1);
	setnext(adventure);
}

static void main_new_game() {
	setnext(newgame);
}

static void option_new_game() {
	if(!dlgask("Are you really want to start new game?"))
		return;
	setnext(newgame);
}

static void memorize_spells() {
	game::action::preparespells(Mage);
}

static void pray_for_spells() {
	game::action::preparespells(Cleric);
}

static void option_save_game() {
	game::write();
	setnext(adventure);
}

static void quit_game() {
	if(!dlgask("Are you really want to quit game?"))
		return;
	exit(0);
}

static void load_game() {
	draw::resetres();
	if(game::read())
		setnext(adventure);
	else {
#ifdef _DEBUG
		if(true) {
			game::setcamera(Blocked);
			dungeon::create(1, first_adventure);
			random_heroes();
			game::write();
			game::enter(1, 1);
		} else {
			random_heroes();
			test_dungeon(BRICK);
			draw::settiles(location.head.type);
			game::setcamera(location.getindex(16, 16), Up);
		}
		setnext(adventure);
#endif
	}
}

static void settings() {}

void draw::options() {
	static menu elements[] = {{pray_for_spells, "Pray for spells"},
	{memorize_spells, "Memorize spells"},
	{creature::scriblescrolls, "Scrible scrolls"},
	{option_new_game, "New game"},
	{load_game, "Load game"},
	{option_save_game, "Save game"},
	{settings, "Settings"},
	{quit_game, "Quit game"},
	{}};
	chooseopt(elements);
}

void draw::mainmenu() {
	static draw::menu source[] = {{main_new_game, "Create New Game"},
	{load_game, "Load Saved game"},
	{quit_game, "Exit game"},
	{}};
	choose(source);
}

void draw::setnext(void(*v)()) {
	next_proc = v;
}

int main(int argc, char* argv[]) {
	srand(clock());
	//srand(2112);
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	draw::initialize();
#ifdef _DEBUG
	static int ovr12[4] = {0, 1, 2};
	static int ovr13[4] = {0, 1, 3};
	static int ovr3[4] = {0, 2, 3};
	//test_monster(ANT, 0);
	//test_monster(GUARD1, ovr3);
	//test_monster(CLERIC2, ovr3)
	//test_monster(BLDRAGON, 0);;
#endif
	next_proc = mainmenu;
	while(next_proc) {
		auto p = next_proc;
		next_proc = 0; p();
	}
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}