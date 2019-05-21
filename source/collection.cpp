#include "collection.h"
#include "crt.h"

void* arraydata::add(unsigned size) {
	auto p = this;
	while(p->count >= p->maximum) {
		if(!p->next) {
			auto m = p->maximum * 2;
			auto s = sizeof(*this) + size * m;
			p->next = (arraydata*)new char[s];
			p->next->count = 0;
			p->next->next = 0;
			p->next->maximum = m;
		}
		p = p->next;
	}
	return (char*)p + sizeof(arraydata) + (p->count++)*size;
}

void arraydata::release() {
	auto p = next;
	next = 0;
	count = 0;
	while(p) {
		auto n = p->next;
		delete ((char*)p); // Не вызывается ~arraydata();
		p = n;
	}
}

unsigned arraydata::getcount() const {
	unsigned result = 0;
	for(auto p = this; p; p = p->next)
		result += p->count;
	return result;
}

void* arraydata::get(int index, unsigned size) const {
	for(auto p = this; p; p = p->next) {
		if((unsigned)index < p->count)
			return (char*)p + sizeof(*this) + index * size;
		index -= p->count;
	}
	return 0;
}

void arraydata::clear() {
	for(auto p = this; p; p = p->next)
		p->count = 0;
}

int arraydata::indexof(const void* e, unsigned size) const {
	for(auto p = this; p; p = p->next) {
		if(e >= (char*)p + sizeof(*this)
			&& e <= (char*)p + sizeof(*this) + size * maximum)
			return (((char*)e - (char*)p) - sizeof(*this)) / size;
	}
	return -1;
}