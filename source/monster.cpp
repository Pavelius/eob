#include "intellegence.h"
#include "main.h"

static const int hd_experience[] = {
	7,
	15, 35, 65, 120, 175, 270, 420, 650, 975, 1400,
	1400, 2000, 3000
};

monsteri bsdata<monsteri>::elements[] = {
	{"No monster"},
	{"Ant", ANT, {0}, Insectoid, Male, Large, TrueNeutral, NoInt, {}, {3}, 3, {Bite}, {OfPoison}},
	{"Bugbear", BUGBEAR, {0}, Humanoid, Male, Large, ChaoticEvil, Low, {}, {3, 1}, 5, {AxeBattle}},
	{"Cleric", CLERIC2, {0, 1, 3}, Human, Male, Medium, LawfulEvil, High, {}, {5}, 7, {Mace}},
	{"Dwarf", DWARF, {0}, Dwarf, Male, Medium, LawfulGood, Ave, {}, {1, 1}, 5, {AxeBattle}},
	{"Gnoll", FLIND, {0}, Humanoid, Male, Medium, LawfulEvil, Low, {}, {2, 3}, 5, {Mace}},
	{"Ghoul", GHOUL, {0}, Humanoid, Male, Medium, ChaoticEvil, Semi, {Undead}, {2}, 6, {Claws, Bite}, {OfParalize}},
	{"Goblin", GOBLIN, {0}, Goblinoid, Male, Medium, LawfulEvil, Low, {}, {1, -1}, 6, {Axe}},
	{"Kobold", KOBOLD, {0}, Goblinoid, Male, Small, ChaoticEvil, Low, {}, {1, -3}, 7, {Dagger}},
	{"Kuotoa", KUOTOA, {0}, Humanoid, Male, Large, NeutralEvil, Semi, {}, {3}, 7, {Slam}},
	{"Leech", LEECH, {0}, Animal, Male, Large, TrueNeutral, AnimalInt, {}, {2}, 9, {Bite2d6}, {OfVampirism}},
	{"Orc", ORC, {0}, Goblinoid, Male, Medium, LawfulEvil, Ave, {}, {1}, 7, {SwordLong}},
	{"Shadow", SHADOW, {0}, Humanoid, Male, Medium, ChaoticEvil, Low, {Undead, ImmuneNormalWeapon}, {3, 3}, 7, {Bite1d41}, {OfStrenghtDrain}},
	{"Skeleton", SKELETON, {0}, Human, Male, Medium, TrueNeutral, Semi, {Undead, ResistPierce, ResistSlashing}, {1}, 7, {Axe}},
	{"Skeleton Warrior", SKELWAR, {0}, Human, Male, Medium, NeutralEvil, Exeptional, {Undead, ImmuneNormalWeapon}, {9, 10}, 2, {SwordTwoHanded}, {OfFear}, {{ResistMagic, 90}}},
	{"Spider", SPIDER1, {0}, Insectoid, Male, Large, ChaoticEvil, AnimalInt, {}, {4, 4}, 4, {Bite}, {OfPoison}},
	{"Wight", WIGHT, {0}, Human, Male, Medium, LawfulEvil, Ave, {Undead, ImmuneNormalWeapon}, {4, 3}, 7, {Slam1d4}, {OfEnergyDrain}},
	{"Wolf", WOLF, {0}, Animal, Male, Large, TrueNeutral, Semi, {}, {3}, 7, {Bite1d41}},
	{"Zombie", ZOMBIE, {0}, Human, Male, Medium, TrueNeutral, Semi, {Undead, Slowest}, {2}, 8, {Slam}},
	{"Object", NONE, {0}, Human, NoGender, Medium, TrueNeutral, NoInt, {}, {3}, 10, {}}
};
assert_enum(monsteri, StaticObject);
BSDATAF(monsteri);

bool monsteri::is(variant id) const {
	return enchantments[0] == id
		|| enchantments[1] == id;
}

bool monsteri::is(feat_s id) const {
	return feats.is(id);
}

int monsteri::getexperience() const {
	auto r = hd[0];
	if(r && hd[1] > 0)
		r += 1;
	if(ac <= 0)
		r += 1;
	if(is(OfPoison))
		r += 1;
	if(is(ResistBludgeon) || is(ResistPierce) || is(ResistSlashing))
		r += 1;
	if(is(ImmuneNormalWeapon))
		r += 1;
	if(is(OfEnergyDrain))
		r += 3;
	if(is(Undead))
		r++;
	if(is(OfFear))
		r += 1;
	if(is(OfParalize))
		r += 2;
	if(skills.data[ResistMagic] >= 50)
		r++;
	if(skills.data[ResistMagic] >= 90)
		r++;
	auto exp = maptbl(hd_experience, r);
	if(r > 13)
		exp += (r - 13) * 1000;
	return exp;
}

dice creature::gethitdice() const {
	dice hd = {};
	hd.c = gethd();
	hd.d = bsdata<classi>::elements[getclass(type, 0)].hd;
	hd.b = getmonster().hd[1];
	if(!hd.c) {
		hd.c = 1;
		hd.d = hd.b;
		hd.b = 0;
	}
	return hd;
}

void creature::set(monster_s v_kind) {
	nameable::set(v_kind);
	auto& mi = getmonster();
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		basic.ability[i] = 10;
	if(bsdata<intellegencei>::elements[mi.ins].v2)
		basic.ability[Intellegence] = xrand(bsdata<intellegencei>::elements[mi.ins].v1, bsdata<intellegencei>::elements[mi.ins].v2);
	else
		basic.ability[Intellegence] = 0;
	switch(getsize()) {
	case Large: ability[Strenght] += 5; break;
	case Tall: ability[Strenght] += 1; break;
	case Small: ability[Strenght] -= 1; ability[Dexterity] += 2; break;
	}
	alignment = mi.alignment;
	nameable::set(mi.race);
	nameable::set(mi.gender);
	basic.feats = mi.feats;
	levels[0] = mi.hd[0];
	if(mi.hd[1] >= 3)
		levels[0]++;
	pallette = mi.getpallette();
}

creature* dungeoni::addmonster(monster_s type, indext index, char side, direction_s dir) {
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
	pc->finish();
	auto hitd = pc->gethd();
	auto& mi = bsdata<monsteri>::elements[type];
	for(auto i : mi.attacks) {
		if(!i)
			continue;
		auto chance_magic = imax(0, imin(65, 5 + hitd * 3));
		auto chance_cursed = 5;
		if(pc->is(Undead))
			chance_cursed += 5;
		pc->equip(i);
	}
	return pc;
}

int dungeoni::addmonster(monster_s type, indext index, direction_s dir) {
	const auto& e = bsdata<monsteri>::elements[type];
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
	auto result = count;
	while(count > 0) {
		int side = getfreeside(index);
		if(side == -1)
			break;
		addmonster(type, index, side, dir);
		count--;
	}
	return result;
}