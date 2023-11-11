#pragma once

enum city_ability_s : unsigned char {
	Blessing, Prosperty, Reputation, ExperienceReward, Gold,
};
struct cityabilityi {
	const char*		name;
	const char*		format;
};
struct cityabilitya {
	int				abilities[Gold + 1];
	void			addcity(city_ability_s i, int v) { abilities[i] += v; }
	void			addgold(int coins) { addcity(Gold, coins); }
	int				getcity(city_ability_s i) const { return abilities[i]; }
	int				getgold() const { return getcity(Gold); }
	void			pay(int coins) { addgold(-coins); }
};