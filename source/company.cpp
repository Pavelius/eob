#include "main.h"

BSDATAC(adventurei, 128)
BSDATAC(settlementi, 64)
BSDATAC(eventi, 256)

unsigned historyi::gethistorymax() const {
	for(unsigned i = 0; i < history_max; i++)
		if(!history[i])
			return i;
	return history_max;
}

bool adventurei::match(variant v) const {
	switch(v.type) {
	case Settlement: return settlement == v.value;
	default: return false;
	}
}