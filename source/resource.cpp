#include "main.h"
#include "draw.h"

using namespace draw;

BSDATA(packi) = {{"PackDungeon", "art/dungeons", false},
{"PackMonster", "art/monsters", false},
{"PackOuttake", "art/misc", true},
{"PackInterface", "art/interface", true},
{"PackBackground", "art/interface", false},
{"PackCenter", "art/misc", true},
{"Pack160x96", "art/scenes", true},
{"Pack320x120", "art/scenes", true},
{"Pack320x200", "art/interface", true},
};
assert_enum(pack, Pack320x200)

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
{"INTRO", Pack320x200},
{"PORTM", PackCenter},
{"THROWN", PackInterface},
{"XSPL", PackInterface},
{"WORLD", PackInterface},
{"NPC", Pack160x96},
{"BPLACE", Pack320x120},
{"ADVENTURE", Pack160x96},
{"BUILDNGS", Pack160x96},
{"DUNGEONS", Pack160x96},
{"CRYSTAL", Pack160x96},
{"SCENES", Pack160x96},

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

sprite* gres(resource_s id) {
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

resourcei* resourcei::find(const char* id, unsigned size) {
	if(!id)
		return 0;
	for(auto& e : bsdata<resourcei>()) {
		if(memcmp(e.name, id, size) == 0 && e.name[size]==0)
			return &e;
	}
	return 0;
}

void imagei::add(stringbuilder& sb) const {
	if(!(*this))
		return;
	sb.add("#");
	sb.add(bsdata<resourcei>::elements[res].name);
	sb.add(" %1i\n", frame);
}