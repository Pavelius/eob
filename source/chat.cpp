#include "main.h"

static int random_chance(rarity_s v) {
	switch(v) {
	case Common: return 75;
	case Uncommon: return 50;
	case Rare: return 25;
	case VeryRare: return 10;
	case Artifact: return 1;
	default: return 0;
	}
}

static bool testing(const conditiona& source, const aref<variant>& variants) {
	for(auto v : source) {
		if(!v)
			return true;
		switch(v.type) {
		case Rarity:
			if(d100() >= random_chance((rarity_s)v.value))
				return false;
			break;
		default:
			if(variants.indexof(v) == -1)
				return false;
			break;
		}
	}
	return true;
}

const char* chati::find(const aref<variant>& variants) const {
	for(auto p = this; *p; p++) {
		if(!testing(p->conditions, variants))
			continue;
		return p->text;
	}
	return 0;
}

int	chati::getconditions() const {
	auto result = 0;
	for(auto v : conditions) {
		if(!v)
			break;
		result++;
	}
	return result;
}