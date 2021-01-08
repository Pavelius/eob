#include "view.h"

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
	location.set(location.getindex(x-1, y - 4), CellGrave);
	location.set(location.getindex(x-1, y - 5), CellGrave);
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
}

static creature* add_hero(int n, gender_s gender, race_s race, class_s type, alignment_s alignment) {
	if(bsdata<creature>::source.getcount() < 4)
		bsdata<creature>::source.setcount(4);
	auto p = bsdata<creature>::elements + n;
	p->create(gender, race, type, alignment);
	party.add(p);
	return p;
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
		case 'A':
			draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
			draw_monster(x, y, rs, 4, overlay, flags, percent, 0, pal);
			draw::redraw(); sleep(500);
			draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::blue);
			draw_monster(x, y, rs, 5, overlay, flags, percent, 0, pal);
			draw::redraw(); sleep(500);
			break;
		case 'M':
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

static void show_monsters() {
	static int ovr12[4] = {0, 1, 2};
	static int ovr13[4] = {0, 1, 3};
	static int ovr3[4] = {0, 2, 3};
	//test_monster(ANT, 0);
	//test_monster(GUARD1, ovr3);
	//test_monster(CLERIC2, ovr13);
	//test_monster(BLDRAGON, 0);
}

static bool test_variant() {
	answers aw;
	variant v1 = LawfulEvil;
	aw.add((int)v1, "Test alignment");
	variant v2 = aw.elements[0].id;
	return v1 == v2;
}

static bool test_richtexti() {
	richtexti ei;
	auto p = "#NPC 12\nWhen you arrive to the bank, test this.\nAnd then test this.\n#NPC 11\nFinally try to understand.";
	ei.load(p);
	if(ei.images[0].frame != 12)
		return false;
	if(ei.images[2].frame != 11)
		return false;
	textable ta;
	ei.save(ta);
	auto p1 = ta.getname();
	return strcmp(p, p1) == 0;
}

void random_heroes() {
	creature* p;
	//
	p = add_hero(0, Male, Human, Paladin, LawfulGood);
	p->set({SwordLong, Fire}, RightHand);
	p->equip({GreenRing, OfRegeneration});
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
	p->add({GreenPotion, OfAdvise});
	p->add({GreenPotion, OfAdvise});
}

static void random_events() {
	auto e1 = (eventi*)bsdata<eventi>::source.add();
	e1->clear();
	e1->setname("#NPC 56\nIn middle of day you see a group of dwarven worker, cornered their human boss. They demand more pays for work. Boss don't agreed. You think that the boss will be beaten a for a few moments.");
	e1->ask[0].setname("Talk to dwarven workers and convice them not demand more pay.");
	e1->ask[1].setname("Talk to human boos and convice him to pay more to this poor dwarfs.");
	e1->results[0].setname("Argessive dwarven workers calm down when see, that dwarf from party talk to them. By short conversation you convice them to do work. After all you make agreement for small raising payments and all go to work.");
	e1->results[0].actions[0] = Case1;
	e1->results[0].actions[1] = Dwarf;
	e1->results[0].actions[2] = GainProsperty;
	e1->results[1].setname("Beside all of you efforts, workers refuse talk with you, calling you \"part of a problem\". After all strike get dowm, but no one is satisfied it result.");
	e1->results[1].actions[0] = Case1;
	e1->results[1].actions[1] = Reshufle;
	e1->results[2].setname("\"I can't pay more\" - sad boss. But after short coversation with your fighter when he \"explain\" to boss that he can pay more, boss agreed. All is satisfied.");
	e1->results[2].actions[0] = Case2;
	e1->results[2].actions[1] = Fighter;
	e1->results[2].actions[2] = GainProsperty;
	e1->results[3].setname("You can't convice boss - only anger him. After all dwarven agreed back to work, but probles is not be solved.");
	e1->results[3].actions[0] = Case2;
	e1->results[3].actions[1] = Reshufle;
	e1->set(eventi::Start);
	e1 = e1 = (eventi*)bsdata<eventi>::source.add();
	e1->clear();
	e1->setname("#SCENES 0\nYou decide to unwind at the local tavern, but just as you are starting to relax, a bear of a man crashes into your table, scattering your drinks across the floor.\n"
		"Towering over him is a massive dwarf. \"What did you say about my bread?\" the dwarf shouts. The man stands up and brushes shards of glass from his tunic. \"I said the sight of it makes me want to vomit!\"\nThe dwarf roars and charges into the man, crashing through more tables in the process. At this, the entire tavern erupts into violence. After all, when a man is deep into his drink, the last thing you want to do is knock that drink over.");
	e1->ask[0].setname("Join the fray! These insults will not go unanswered!");
	e1->ask[1].setname("Do your best to stop the fighting. This is a respectable establishment.");
	e1->results[0].setname("Nothing like busting some drunken skulls to lift one's spirits. It turns out to be a great way to unwind. Unfortunately, the proprietor of the local tavern doesn't exactly see it that way, and he sullenly asks for compensation for the damage you caused.");
	e1->results[0].actions[0] = Case1;
	e1->results[0].actions[1] = Gain100Exp;
	e1->results[0].actions[2] = Lose20GPorReputation;
	e1->results[1].setname("After restraining the enraged dwarf and offering to replace the drinks of a few of the more belligerent patrons, you calm the place down a bit. Some of the non-human patrons are understandably on edge, but the proprietor thanks you for your efforts and reimburses you for the drinks.");
	e1->results[1].actions[0] = Case2;
	e1->results[1].actions[1] = GainReputation;
	e1->set(eventi::Start);
	e1 = e1 = (eventi*)bsdata<eventi>::source.add();
	e1->clear();
	e1->setname("#SCENES 2\nAs the daylight fades, you find yourselves wandering through a half-crowded market street, browsing wares.\n#NPC 71\n\"Hey! Over here!\" You turn in the direction of the voice to see a filthy halfling gesturing from a dark alley.\n\"Yeah, you grim - looking chaps. I have something you might be interested in.\"\nThe halfling holds out a piece of metal covered in sludge. \"Found this in the sewer. Writing on it I don't understand, but I know it's valuable. You can have it for ten gold!\"");
	e1->ask[0].setname("Pay for the thing. You never know.");
	e1->ask[1].setname("Refuse to pay. Never trust a rogue halfling.");
	e1->results[0].setname("You hand over the gold and take hold of the piece of garbage. Amidst troubling brown smears you see a lot of meaningless scratches likely made by rats and bugs. Oh well. Sometimes the long shot doesn\'t pay off.");
	e1->results[0].actions[0] = Case1;
	e1->results[0].actions[1] = Pay10;
	e1->results[1].setname("\"Bah! You don't have enough. Come back when you do!\"");
	e1->results[1].actions[0] = Case1;
	e1->results[1].actions[1] = Reshufle;
	e1->results[2].setname("You laugh and gesture the halfling away. You can recognize a low - life swindler when you see one. And that piece of garbage was just...foul.");
	e1->results[2].actions[0] = Case2;
}

void random_company() {
	static sitei collectors[] = {{{BRICK, {Kobold, Leech}, {KeySilver, KeyCooper}, StoneOrb, Human}, 2, {5}},
	{{BRICK, {Skeleton, Zombie}, {KeySilver, KeyCooper}, StoneDagger, Human}, 2, {10}},
	{{BRICK, {Zombie, Ghoul}, {KeySilver, KeyCooper}, {}, Human}, 1, {10}, {Wight}}
	};
	static sitei dead_crypt[] = {{{BRICK, {Skeleton, Leech}, {KeySilver, KeyCooper}, StoneOrb, Human}, 2, {5}},
	{{BRICK, {Skeleton, Zombie}, {KeySilver, KeyCooper}, StoneDagger, Human}, 1, {10}},
	};
	game.clear();
	game.setname("Western heartlands");
	game.start = 0;
	game.pixels_per_day = 120;
	auto pa = (adventurei*)bsdata<adventurei>::source.add();
	pa->setname("Flooded collectors");
	pa->position = {614, 294};
	pa->rumor_activate.setname("Since last month people of Baldur's gate myserious disapears. Some old beggar tells story about flooded collectors and monsters, who dwelve in it. Ask city guards about flooded collectors and maybe you get a job.");
	pa->history[0].setname("Years ago we found this place. It's perfect place, fresh food is always on ground and some times adventurers leak there and get rumor from outside.");
	pa->history[1].setname("Our master want answers. What lie up ground? Big city? How it big and how it reach? Adventurers tell some information but we need more. Master need more!");
	pa->history[2].setname("This leech is ugly disasters. It come from underground sea, where it hunt a blind fish. But how it get there? Some where must be hole from where it come here.");
	pa->activate();
	memcpy(pa->levels, collectors, sizeof(collectors));
	auto ps = (settlementi*)bsdata<settlementi>::source.add();
	ps->setname("Baldur's gate");
	ps->position = {495, 404};
	ps->prosperty = 50;
	ps->buildings.add(WizardTower);
	ps->buildings.add(Tavern);
	ps->buildings.add(Temple);
	ps->buildings.add(Stable);
	ps->buildings.add(Armory);
	ps = (settlementi*)bsdata<settlementi>::source.add();
	ps->setname("Upper Chionthar");
	ps->buildings.add(Tavern);
	ps->position = {623, 285};
	ps->prosperty = 7;
	ps = (settlementi*)bsdata<settlementi>::source.add();
	ps->setname("Ulgoth's Beard");
	ps->position = {185, 279};
	ps->prosperty = 10;
	ps->buildings.add(Brothel);
	ps->buildings.add(Temple);
	ps = (settlementi*)bsdata<settlementi>::source.add();
	ps->setname("Lower Chionthar");
	ps->buildings.add(Inn);
	ps->position = {829, 386};
	ps->prosperty = 3;
	ps = (settlementi*)bsdata<settlementi>::source.add();
	ps->setname("Friendly arm");
	ps->position = {1041, 740};
	ps->buildings.add(Tavern);
	ps->buildings.add(Inn);
	ps->prosperty = 5;
	ps = (settlementi*)bsdata<settlementi>::source.add();
	ps->setname("Dwain hunter");
	ps->position = {1108, 449};
	ps->buildings.add(Tavern);
	ps->prosperty = 3;
	random_events();
}

void debug_dungeon2() {
	location.clear();
	location_above.clear();
	random_heroes();
	//test_dungeon(BRICK);
	test_dungeon2(BRICK);
	draw::settiles(location.head.type);
	game.setcamera(location.getindex(16, 16), Up);
}

#endif // DEBUG

void editor() {
	auto push_font = font;
	setsmallfont();
	random_heroes();
	random_company();
	game.writetext("test.json", Event);
	if(true) {
		//game.companyi::read("default");
		game.addgold(200);
		game.jumpto(bsdata<settlementi>::elements);
		//bsdata<eventi>::elements[2].play();
		game.passtime(3 * 24 * 60 + xrand(8 * 60, 13 * 60));
		game.write();
		game.play();
	} else {
		//game.companyi::read("default");
		edit("Company", &game, dginf<companyi>::meta, false);
		//game.companyi::write("default");
	}
	font = push_font;
}

int main(int argc, char* argv[]) {
	srand(clock());
#ifdef _DEBUG
	if(!test_variant())
		return -1;
	if(!test_richtexti())
		return -1;
	util_main();
#endif // _DEBUG
	draw::initialize();
	fore = colors::white;
	setbigfont();
	setnext(mainmenu);
	application();
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}