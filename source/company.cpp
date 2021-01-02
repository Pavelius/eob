#include "main.h"

BSDATAC(adventurei, 128)
BSDATAC(settlementi, 32)
BSDATAC(fractioni, 12)

void looti::correct() {
	if(gold < 0)
		gold = 0;
}

unsigned historyi::gethistorymax() const {
	for(unsigned i = 0; i < history_max; i++)
		if(!history[i])
			return i;
	return history_max;
}

adventurei* companyi::getadventure(point position) {
	for(auto& e : bsdata<adventurei>()) {
		if(e.position == position)
			return &e;
	}
	return 0;
}