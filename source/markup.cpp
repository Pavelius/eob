#include "markup.h"

DGMETA(int) = {{}};
DGMETA(const char*) = {{}};

bool markup::is(const char* id) const {
	return title
		&& title[0] == '#'
		&& title[1] == id[0]
		&& title[2] == id[1]
		&& title[3] == id[2]
		&& title[4] == id[3];
}