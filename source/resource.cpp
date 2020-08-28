#include "view.h"

using namespace draw;

BSDATA(resourcei) = {{"NONE"},
{"BORDER", "art/interface"},
{"OUTTAKE", "art/misc"},
{"CHARGEN", "art/interface"},
{"CHARGENB", "art/interface"},
{"COMPASS", "art/interface"},
{"INVENT", "art/interface"},
{"ITEMS", "art/misc"},
{"ITEMGS", "art/misc"},
{"ITEMGL", "art/misc"},
//
{"BLUE", "art/dungeons"},
{"BRICK", "art/dungeons"},
{"CRIMSON", "art/dungeons"},
{"DROW", "art/dungeons"},
{"DUNG", "art/dungeons"},
{"GREEN", "art/dungeons"},
{"FOREST", "art/dungeons"},
{"MEZZ", "art/dungeons"},
{"SILVER", "art/dungeons"},
{"XANATHA", "art/dungeons"},
//
{"MENU", "art/interface"},
{"PLAYFLD", "art/interface"},
{"PORTM", "art/misc"},
{"THROWN", "art/misc"},
{"XSPL", "art/interface"},
//
{"ANKHEG", "art/monsters"},
{"ANT", "art/monsters"},
{"BLDRAGON", "art/monsters"},
{"BUGBEAR", "art/monsters"},
{"CLERIC1", "art/monsters"},
{"CLERIC2", "art/monsters"},
{"CLERIC3", "art/monsters"},
{"DRAGON", "art/monsters"},
{"DWARF", "art/monsters"},
{"FLIND", "art/monsters"},
{"GHOUL", "art/monsters"},
{"GOBLIN", "art/monsters"},
{"GUARD1", "art/monsters"},
{"GUARD2", "art/monsters"},
{"KOBOLD", "art/monsters"},
{"KUOTOA", "art/monsters"},
{"LEECH", "art/monsters"},
{"ORC", "art/monsters"},
{"SHADOW", "art/monsters"},
{"SKELETON", "art/monsters"},
{"SKELWAR", "art/monsters"},
{"SPIDER1", "art/monsters"},
{"WIGHT", "art/monsters"},
{"WOLF", "art/monsters"},
{"ZOMBIE", "art/monsters"},
};
assert_enum(resource, ZOMBIE);
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
	return path && strcmp(path, "art/dungeons") == 0;
}

bool resourcei::ismonster() const {
	return path && strcmp(path, "art/monsters") == 0;
}