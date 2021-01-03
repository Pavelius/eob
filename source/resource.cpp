#include "view.h"

using namespace draw;

BSDATA(packi) = {{"PackDungeon", "art/dungeons"},
{"PackMonster", "art/monsters"},
{"PackOuttake", "art/misc"},
{"PackInterface", "art/interface"},
{"PackBackground", "art/interface"},
{"PackCenter", "art/misc"},
{"PackCustom", "art/custom"},
};
assert_enum(pack, PackCustom)

BSDATA(resourcei) = {{"NONE", PackBackground},

{"BORDER", PackBackground},
{"OUTTAKE", PackOuttake},
{"DECORS", PackOuttake},
{"CHARGEN", PackBackground},
{"CHARGENB", PackInterface},
{"COMPASS", PackInterface},
{"INVENT", PackBackground},
{"ITEMS", PackCenter},
{"ITEMGS", PackOuttake},
{"ITEMGL", PackOuttake},

{"BLUE", PackDungeon},
{"BRICK", PackDungeon},
{"CRIMSON", PackDungeon},
{"DROW", PackDungeon},
{"DUNG", PackDungeon},
{"GREEN", PackDungeon},
{"FOREST", PackDungeon},
{"MEZZ", PackDungeon},
{"SILVER", PackDungeon},
{"XANATHA", PackDungeon},

{"MENU", PackBackground},
{"PLAYFLD", PackBackground},
{"PORTM", PackCenter},
{"THROWN", PackInterface},
{"XSPL", PackInterface},

{"ANKHEG", PackMonster},
{"ANT", PackMonster},
{"BLDRAGON", PackMonster},
{"BUGBEAR", PackMonster},
{"CLERIC1", PackMonster},
{"CLERIC2", PackMonster},
{"CLERIC3", PackMonster},
{"DRAGON", PackMonster},
{"DWARF", PackMonster},
{"FLIND", PackMonster},
{"GHOUL", PackMonster},
{"GOBLIN", PackMonster},
{"GUARD1", PackMonster},
{"GUARD2", PackMonster},
{"KOBOLD", PackMonster},
{"KUOTOA", PackMonster},
{"LEECH", PackMonster},
{"ORC", PackMonster},
{"SHADOW", PackMonster},
{"SKELETON", PackMonster},
{"SKELWAR", PackMonster},
{"SPIDER1", PackMonster},
{"WIGHT", PackMonster},
{"WOLF", PackMonster},
{"ZOMBIE", PackMonster},
};
assert_enum(resource, ZOMBIE)
INSTELEM(resourcei)

void view_dungeon_reset();

const char* resourcei::geturl() const {
	return bsdata<packi>::elements[pack].url;
}

sprite* draw::gres(resource_s id) {
	auto& e = bsdata<resourcei>::elements[id];
	if(!e.data) {
		if(!id)
			return 0;
		char temp[260];
		e.data = (sprite*)loadb(szurl(temp, e.geturl(), e.name, "pma"));
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