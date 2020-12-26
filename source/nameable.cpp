#include "main.h"

void nameablei::setnamev(const char* value, const char* format) {
	stringbuilder sb(name);
	sb.addv(value, format);
}