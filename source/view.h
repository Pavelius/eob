#include "draw.h"
#include "main.h"

#pragma once

namespace draw {
struct palspr : pma {
	unsigned char		data[18][16];
};
sprite*					gres(resource_s id);
void					imagex(int x, int y, const sprite* res, int id, unsigned flags, int percent, unsigned char shadow);
}