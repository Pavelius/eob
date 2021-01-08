#include "crt.h"
#include "io_plugin.h"

io::plugin* io::plugin::first;

void io::plugin::addfilter(stringbuilder& sb, const char* name, const char* filter) {
	if(!filter || !name || filter[0]==0 || name[0]==0)
		return;
	sb.add("%+1 (%2)", name, filter);
	sb.addsz();
	sb.add(filter);
	sb.addsz();
}

io::plugin::plugin() : name(0), fullname(0), filter(0) {
	seqlink(this);
}

io::plugin* io::plugin::find(const char* name) {
	for(auto p = first; p; p = p->next) {
		if(!p->name)
			continue;
		if(strcmp(p->name, name) == 0)
			return p;
	}
	return 0;
}

void io::plugin::getfilter(stringbuilder& sb) {
	for(auto p = first; p; p = p->next) {
		if(!p->name)
			continue;
		addfilter(sb, p->fullname, p->filter);
	}
	sb.addsz();
}

bool io::read(const char* url, serializer::reader& e) {
	auto pp = io::plugin::find(szext(url));
	if(!pp)
		return false;
	const char* source = loadt(url);
	if(!source || source[0] == 0)
		return false;
	pp->read(source, e);
	delete source;
	return true;
}