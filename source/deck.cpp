#include "main.h"

void deck::create(variant_s v) {
	type = v;
	clear();
}

void deck::shuffle() {
	zshuffle(data, count);
}

void deck::addbottom(variant v) {
	if(v.type != type)
		return;
	if(count >= (unsigned)getmaximum())
		return;
	data[count++] = v.value;
}

variant	deck::gettop() {
	if(!count)
		return variant();
	auto r = variant(type, data[0]);
	remove(0, 1);
	return r;
}

variant	deck::getbottom() {
	if(!count)
		return variant();
	return variant(type, data[count--]);
}

void deck::discard(variant v) {
	if(v.type != type)
		return;
	auto p = data;
	for(auto& e : *this) {
		if(e == v.value)
			continue;
		*p++ = v.value;
	}
	count = p - data;
}