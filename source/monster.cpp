#include "main.h"

static const int hd_experience[] = {
	7,
	15, 35, 65, 120, 175, 270, 420, 650, 975, 1400,
	1400, 2000, 3000
};

monsteri bsmeta<monsteri>::elements[] = {{"No monster"},
{"Ant", ANT, {0}, Insectoid, Male, Large, TrueNeutral, {}, {3}, 3, {Bite}, {WeakPoison}},
{"Old cleric", CLERIC2, {0, 1, 3}, Human, Male, Medium, LawfulEvil, {}, {5}, 7, {Mace}, {}},
{"Gnoll", FLIND, {0}, Humanoid, Male, Medium, LawfulEvil, {}, {2, 3}, 5, {Mace}, {}},
{"Ghoul", GHOUL, {0}, Humanoid, Male, Medium, ChaoticEvil, {Undead}, {2}, 6, {Claws, Bite}, {StateParalized}},
{"Goblin", GOBLIN, {0}, Goblinoid, Male, Medium, LawfulEvil, {}, {0, 7}, 6, {Axe}, {}, {}},
{"Kobold", KOBOLD, {0}, Goblinoid, Male, Small, ChaoticEvil, {}, {0, 4}, 7, {Dagger}, {}},
{"Kuotoa", KUOTOA, {0}, Humanoid, Male, Large, NeutralEvil, {}, {3}, 7, {Slam}, {}},
{"Leech", LEECH, {0}, Animal, Male, Large, TrueNeutral, {}, {2}, 9, {Bite2d6}, {}, {OfVampirism}},
{"Orc", ORC, {0}, Goblinoid, Male, Medium, LawfulEvil, {}, {1}, 7, {SwordLong}, {}, {}},
{"Skeleton", SKELETON, {0}, Human, Male, Medium, TrueNeutral, {Undead, ResistPierce, ResistSlashing}, {1}, 7, {Axe}, {}, {}},
{"Spider", SPIDER1, {0}, Insectoid, Male, Large, ChaoticEvil, {}, {4, 4}, 4, {Bite}, {Poison}},
{"Zombie", ZOMBIE, {0}, Human, Male, Medium, TrueNeutral, {Undead}, {2}, 8, {Slam}, {}, {}}
};
assert_enum(monster, Zombie);

bool monsteri::is(state_s id) const {
	return special[0] == id
		|| special[1] == id
		|| special[2] == id;
}

bool monsteri::is(enchant_s id) const {
	return enchantments[0] == id
		|| enchantments[1] == id;
}

int monsteri::getexperience() const {
	int r = hd[0];
	if(r && hd[1] > 0)
		r++;
	if(ac <= 0)
		r++;
	if(is(WeakPoison) || is(Poison))
		r++;
	else if(is(StrongPoison))
		r += 2;
	else if(is(DeadlyPoison))
		r += 3;
	if(feats.is(ResistBludgeon) || feats.is(ResistPierce) || feats.is(ResistSlashing))
		r += 1;
	auto exp = maptbl(hd_experience, r);
	if(r > 13)
		exp += (r - 13) * 1000;
	return exp;
}

dice creature::gethitdice() const {
	dice hd = {1, (unsigned char)bsmeta<classi>::elements[getclass(type, 0)].hd};
	hd.c = gethd();
	//hd.b = get(HitPointsAdjustment);
	if(!hd.c) {
		hd.c = 1;
		hd.d = hd.b;
		hd.b = 0;
	}
	return hd;
}

void creature::set(monster_s value) {
	kind = value;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		ability[i] = 10;
	switch(getsize()) {
	case Large:
		ability[Strenght] += 4;
		break;
	case Tall:
		ability[Strenght] += 1;
		break;
	case Small:
		ability[Strenght] -= 1;
		ability[Dexterity] += 2;
		break;
	}
	auto& mi = bsmeta<monsteri>::elements[kind];
	alignment = mi.alignment;
	race = mi.race;
	gender = mi.gender;
	feats = mi.feats;
	levels[0] = mi.hd[0];
	if(mi.hd[1] >= 3)
		levels[0]++;
	for(auto i : mi.attacks)
		equip(i);
}

creature* dungeon::addmonster(monster_s type, short unsigned index, char side, direction_s dir) {
	auto pc = monsters;
	while(pc < monsters + sizeof(monsters) / sizeof(monsters[0])) {
		if(!(*pc))
			break;
		pc++;
	}
	if(pc >= monsters + sizeof(monsters) / sizeof(monsters[0]))
		pc = monsters;
	pc->clear();
	pc->set(type);
	pc->set(dir);
	pc->setindex(index);
	pc->setside(side);
	pc->sethitsroll(pc->gethitdice().roll());
	pc->finish();
	return pc;
}

void dungeon::addmonster(monster_s type, short unsigned index, direction_s dir) {
	const auto& e = bsmeta<monsteri>::elements[type];
	int count = 1;
	if(e.size == Tall)
		count = xrand(1, 2);
	else if(e.size < Large) {
		auto r = d100();
		if(r < 50)
			count = 1;
		else if(r < 75)
			count = 2;
		else if(r < 90)
			count = 3;
		else
			count = 4;
	}
	while(count > 0) {
		int side = getfreeside(index);
		if(side == -1)
			break;
		addmonster(type, index, side, dir);
		count--;
	}
}