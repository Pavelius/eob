#include "main.h"

void itema::select() {
	for(auto pv : party) {
		auto p = pv.getcreature();
		if(!p)
			continue;
		p->select(*this);
	}
}

void itema::forsale(bool remove) {
	auto ps = data;
	for(auto p : *this) {
		auto value = p->getcost() > 0;
		if(p->isidentified() && p->ismagical())
			value = false;
		if(value == remove)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}