#include "view.h"

using namespace draw;

BSDATA(resourcei) = {{"NONE", 0, PackBackground},

{"BORDER", "art/interface", PackBackground},
{"OUTTAKE", "art/misc", PackOuttake},
{"CHARGEN", "art/interface", PackBackground},
{"CHARGENB", "art/interface", PackInterface},
{"COMPASS", "art/interface", PackInterface},
{"INVENT", "art/interface", PackBackground},
{"ITEMS", "art/misc", PackCenter},
{"ITEMGS", "art/misc", PackOuttake},
{"ITEMGL", "art/misc", PackOuttake},

{"BLUE", "art/dungeons", PackDungeon},
{"BRICK", "art/dungeons", PackDungeon},
{"CRIMSON", "art/dungeons", PackDungeon},
{"DROW", "art/dungeons", PackDungeon},
{"DUNG", "art/dungeons", PackDungeon},
{"GREEN", "art/dungeons", PackDungeon},
{"FOREST", "art/dungeons", PackDungeon},
{"MEZZ", "art/dungeons", PackDungeon},
{"SILVER", "art/dungeons", PackDungeon},
{"XANATHA", "art/dungeons", PackDungeon},

{"MENU", "art/interface", PackBackground},
{"PLAYFLD", "art/interface", PackBackground},
{"PORTM", "art/misc", PackCenter},
{"THROWN", "art/misc", PackInterface},
{"XSPL", "art/interface", PackInterface},

{"ANKHEG", "art/monsters", PackMonster},
{"ANT", "art/monsters", PackMonster},
{"BLDRAGON", "art/monsters", PackMonster},
{"BUGBEAR", "art/monsters", PackMonster},
{"CLERIC1", "art/monsters", PackMonster},
{"CLERIC2", "art/monsters", PackMonster},
{"CLERIC3", "art/monsters", PackMonster},
{"DRAGON", "art/monsters", PackMonster},
{"DWARF", "art/monsters", PackMonster},
{"FLIND", "art/monsters", PackMonster},
{"GHOUL", "art/monsters", PackMonster},
{"GOBLIN", "art/monsters", PackMonster},
{"GUARD1", "art/monsters", PackMonster},
{"GUARD2", "art/monsters", PackMonster},
{"KOBOLD", "art/monsters", PackMonster},
{"KUOTOA", "art/monsters", PackMonster},
{"LEECH", "art/monsters", PackMonster},
{"ORC", "art/monsters", PackMonster},
{"SHADOW", "art/monsters", PackMonster},
{"SKELETON", "art/monsters", PackMonster},
{"SKELWAR", "art/monsters", PackMonster},
{"SPIDER1", "art/monsters", PackMonster},
{"WIGHT", "art/monsters", PackMonster},
{"WOLF", "art/monsters", PackMonster},
{"ZOMBIE", "art/monsters", PackMonster},
};
assert_enum(resource, ZOMBIE)
INSTELEM(resourcei)

void view_dungeon_reset();

sprite* draw::gres(resource_s id) {
	auto& e = bsdata<resourcei>::elements[id];
	if(!e.data) {
		if(!id)
			return 0;
		char temp[260];
		e.data = (sprite*)loadb(szurl(temp, e.path, e.name, "pma"));
	}
	return (sprite*)e.data;
}

void draw::resetres() {
	for(auto& e : bsdata<resourcei>()) {
		if(e.data) {
			delete e.data;
			e.data = 0;
		}
	}
	view_dungeon_reset();
}

bool resourcei::isdungeon() const {
	return pack == PackDungeon;
}

bool resourcei::ismonster() const {
	return pack == PackMonster;
}