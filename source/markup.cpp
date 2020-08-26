#include "markup.h"

DGINF(int) = {{}};
DGINF(const char*) = {{}};

bool markup::is(const char* id) const {
	return title
		&& title[0] == '#'
		&& title[1] == id[0]
		&& title[2] == id[1]
		&& title[3] == id[2]
		&& (title[4] == 0 || title[4] == 0x20);
}