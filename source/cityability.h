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
	void			add(city_ability_s i, int v) { abilities[i] += v; }
	void			addgold(int coins) { add(Gold, coins); }
	int				get(city_ability_s i) const { return abilities[i]; }
	int				getgold() const { return get(Gold); }
	void			pay(int coins) { addgold(-coins); }
};