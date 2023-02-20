#pragma once

enum goal_s : unsigned char {
	KillBoss, KillAlmostAllMonsters, ExploreMostDungeon,
	GrabAlmostAllGems, GrabAllSpecialItems,
};
struct goali {
	const char*			id;
	const char*			name;
};
