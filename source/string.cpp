#include "main.h"

void string::addidentifier(const char* identifier) {
	if(source_hero && strcmp(identifier, "hero") == 0)
		source_hero->getname(*this);
	else if(source_item && strcmp(identifier, "item") == 0)
		source_item->getname(*this);
	else
		stringbuilder::addidentifier(identifier);
}