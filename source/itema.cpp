#include "main.h"

void itema::select() {
	for(auto pv : party) {
		auto p = pv.getcreature();
		if(!p)
			continue;
		p->select(*this);
	}
}

void itema::select(pitem proc, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if((p->*proc)() != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void itema::forsale(bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto value = p->getcost() > 0;
		if(p->isidentified() && p->ismagical())
			value = false;
		if(value != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

item* itema::random() {
	if(!count)
		return 0;
	return data[rand() % count];
}