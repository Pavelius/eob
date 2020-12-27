#include "main.h"

unsigned companyi::historyi::gethistorymax() const {
	for(unsigned i = 0; i < history_max; i++)
		if(!history[i])
			return i;
	return history_max;
}

companyi::adventurei* companyi::getadventure(point position) {
	for(auto& e : adventures) {
		if(e.position == position)
			return &e;
	}
	return 0;
}