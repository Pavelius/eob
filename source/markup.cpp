#include "markup.h"

DGMETA(int) = {{}};
DGMETA(const char*) = {{}};

bool markup::is(const char* id) const {
	return title[0] == '#'
		&& title[1] == id[0]
		&& title[2] == id[1]
		&& title[3] == id[2]
		&& title[4] == id[3];
}

int	markitem::get(const void* value_ptr) const {
	switch(size) {
	case sizeof(char) : return *((char*)value_ptr);
	case sizeof(short) : return *((short*)value_ptr);
	default : return *((int*)value_ptr);
	}
}

void markitem::getname(const void* object, stringbuilder& sb, fntext pgetname) const {
	if(isnum()) {
		auto value = get(ptr((void*)object));
		sb.add("%1i", value);
	} else if(istext()) {
		auto value = (const char*)get(ptr((void*)object));
		if(!value)
			value = "";
		sb.add(value);
	} else {
		if(pgetname)
			pgetname(object, sb);
		if(!sb || !sb[0])
			sb.add("None");
	}
}