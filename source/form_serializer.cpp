#include "main.h"
#include "io_plugin.h"

static const markup* getmetatype(const char* name) {
	for(auto& e : bsdata<varianti>()) {
		if(strcmp(e.name, name) == 0)
			return e.form;
	}
	return 0;
}

static varianti* getmetadata(const void* object) {
	variant v1 = object;
	if(!v1.type)
		return 0;
	return bsdata<varianti>::elements + v1.type;
}

static void write_obj(serializer* sz, void* object, const markup* type);

static const char* getheader(const char* title, int index) {
	static char temp[32];
	stringbuilder sb(temp);
	if(!title || title[0] == '0') {
		sb.add("#%1i", index);
		return temp;
	}
	return title;
}

static void write_req(serializer* sz, void* object, const markup* type, int index) {
	if(type->isdecortext())
		return;
	char value[1024]; value[0] = 0;
	stringbuilder sb(value);
	type->getname(object, sb);
	if(type->value.isnum()) {
		if(strcmp(value, "0") == 0)
			return;
		if(strcmp(value, "None") == 0)
			return;
		sz->set(type->title, value);
	} else if(type->isgroup()) {
		sz->open(getheader(type->title, index), serializer::Struct);
		for(auto p = type->value.type; *p; p++)
			write_req(sz, type->value.ptr(object), p, p - type->value.type);
		sz->close(getheader(type->title, index), serializer::Struct);
	} else {
		if(strcmp(value, "None") == 0)
			return;
		sz->set(type->title, value, serializer::Text);
	}
}

static void write_obj(serializer* sz, void* object, const markup* type) {
	auto pm = getmetadata(object);
	if(!pm)
		return;
	sz->open(pm->namepl, serializer::Struct);
	sz->set("objectid", pm->name);
	for(auto p = type; *p; p++)
		write_req(sz, object, p, p - type);
	sz->close(pm->namepl, serializer::Struct);
}

bool gamei::writetext(const char* url, variant_s id) {
	auto& ei = bsdata<varianti>::elements[id];
	if(!ei.source || !ei.form)
		return false;
	auto pp = io::plugin::find(szext(url));
	if(!pp)
		return false;
	io::file file(url, StreamWrite | StreamText);
	auto sz = pp->write(file);
	sz->open("elements", serializer::Array);
	auto pe = (unsigned char*)ei.source->end();
	for(auto p = (unsigned char*)ei.source->begin(); p < pe; p += ei.source->getsize())
		write_obj(sz, p, ei.form);
	sz->close("elements", serializer::Array);
	return true;
}

bool gamei::readtext(const char* url) {
	struct reader : serializer::reader {
		const markup* type;
		const markup* find(const markup* t, const char* name) const {
			for(; *t;t++) {
				if(!t->title || t->title[0]==0)
					continue;
				if(strcmp(t->title, name) == 0)
					return t;
			}
			return 0;
		}
		void open(serializer::node& e) {
		}
		void set(serializer::node& e, const char* value) {
			if(e == "objectid")
				type = getmetatype(value);
			else if(type) {
				auto f = find(type, e.name);
				if(f) {
				}
			}
		}
		void close(serializer::node& e) {
		}
	};
	auto pp = io::plugin::find(szext(url));
	if(!pp)
		return false;
	io::file file(url, StreamRead | StreamText);
	return true;
}