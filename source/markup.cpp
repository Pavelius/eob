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

bool markup::isvisible(const void* object) const {
	if(proc.visible && !proc.visible(object))
		return false;
	if(proc.visiblex) {
		auto p = value.ptr((void*)object);
		if(!proc.visiblex(object, p))
			return false;
	}
	return true;
}

void markup::set(void* p, unsigned size, int v) {
	if(!p)
		return;
	switch(size) {
	case sizeof(char) : *((char*)p) = v; break;
	case sizeof(short) : *((short*)p) = v; break;
	case sizeof(int) : *((int*)p) = v; break;
	}
}

int markup::get(void* p, unsigned size) {
	if(!p)
		return 0;
	switch(size) {
	case sizeof(char) : return *((char*)p);
	case sizeof(short) : return *((short*)p);
	case sizeof(int) : return *((int*)p);
	}
	return 0;
}

void markup::getname(const void* object, stringbuilder& sb) const {
	auto pv = value.ptr((void*)object);
	if(!value.size) {
		if(value.source)
			sb.add("%1i of %2i", value.source->getcount(), value.source->getmaximum());
		else
			sb.add(title);
	} else {
		if(value.source) {
			pv = (void*)get(pv, value.size);
			if(value.size < sizeof(int))
				pv = value.source->ptr((int)pv);
		} else if(list.getptr)
			pv = list.getptr(object, get(pv, value.size));
		auto pfn = list.getname;
		if(pfn) {
			if(pv) {
				auto pn = pfn((void*)pv, sb);
				if(pn && pn != sb)
					sb.add(pn);
			}
		} else if(value.istext()) {
			auto p = (const char*)markup::get(pv, value.size);
			if(!p)
				p = "";
			sb.add(p);
		} else if(value.isnum()) {
			if(value.size <= sizeof(int)) {
				auto v = get(pv, value.size);
				if(value.mask) {
					if((v&value.mask) != 0)
						sb.add("true");
					else
						sb.add("false");
				} else
					sb.add("%1i", v);
			} else
				sb.add((char*)pv);
		}
	}
	if(!sb || !sb[0])
		sb.add("None");
}

const markup* markup::find(const char* name) const {
	for(auto t = this; *t; t++) {
		if(!t->title || t->title[0] == 0)
			continue;
		if(strcmp(t->title, name) == 0)
			return t;
	}
	return 0;
}