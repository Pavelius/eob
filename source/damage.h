#pragma once

enum feat_s : unsigned char;
enum ability_s : unsigned char;

enum damage_s : unsigned char {
	Bludgeon, Slashing, Pierce,
	Cold, Electricity, Fire, Magic,
	Heal, Paralize, Death, Petrification,
};
struct damagei {
	const char*			name;
	feat_s				half;
	ability_s			reduce;
	feat_s				immunity;
};
