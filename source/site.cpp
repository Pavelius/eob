#include "main.h"

unsigned sitei::getleveltotal() const {
	unsigned r = 0;
	for(auto p = this; *p; p++)
		r += p->levels;
	return r;
}