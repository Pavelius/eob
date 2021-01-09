#include "main.h"
#include "io_plugin.h"

static const varianti* getmetatype(const char* name) {
	for(auto& e : bsdata<varianti>()) {
		if(strcmp(e.name, name) == 0)
			return &e;
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
	if(type->value.isnum() && type->value.size <= sizeof(int)) {
		if(strcmp(value, "0") == 0)
			return;
		if(strcmp(value, "false") == 0)
			return;
		if(strcmp(value, "None") == 0)
			return;
		sz->set(type->title, value, serializer::Number);
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
		void open(serializer::node& e) override {
			if(!e.parent)
				return;
			else if(e == "element") {
				e.object = 0;
				e.object_type = 0;
			} else if(e.parent->object && e.parent->object_type) {
				auto object = e.parent->object;
				auto type = (const markup*)e.parent->object_type;
				auto f = type->find(e.name);
				if(f) {
					e.object = f->value.ptr(e.parent->object);
					e.object_type = f->value.type;
				}
			}
		}
		void set(serializer::node& e, const char* value) override {
			if(!e.parent)
				return;
			else if(e.isparent("element") && e == "objectid" && e.parent->parent->isroot()) {
				auto pv = getmetatype(value);
				if(pv->source && pv->form) {
					if(pv->source->getcount() >= pv->source->getmaximum())
						return;
					auto object = pv->source->ptr(pv->source->getcount());
					e.parent->object = object;
					e.parent->object_type = pv->form;
				}
			} else if(e.parent->object && e.parent->object_type) {
				auto object = e.parent->object;
				auto type = (const markup*)e.parent->object_type;
				auto f = type->find(e.name);
				if(!f)
					return;
				auto pv = f->value.ptr(object);
				if(e.type == serializer::Number) {
					if(f->value.mask) {
						if(strcmp(value, "true") == 0)
							f->set(pv, f->value.size, f->get(pv, f->value.size) | f->value.mask);
						else if(strcmp(value, "false") == 0)
							f->set(pv, f->value.size, f->get(pv, f->value.size) & (~f->value.mask));
					} else
						f->set(pv, f->value.size, sz2num(value));
				} else if(f->list.getname == getnm<textable>) {
					textable v; v.setname(value);
					f->set(pv, f->value.size, v.name);
				} else if(f->list.getname == getnm<variant>) {
					variant v = variant::find(value);
					f->set(pv, f->value.size, *((short unsigned*)&v));
				}
			}
		}
		void close(serializer::node& e) override {
			if(e == "element" && e.object && e.object_type) {
				auto type = (const markup*)e.object_type;
				auto meta = varianti::find(type);
				if(!meta)
					return;
				auto index = -1;
				auto pk = type->find("Name");
				if(!pk)
					pk = type->find("Text");
				if(pk)
					index = meta->source->find(pk->value.ptr(e.object), pk->value.offset, pk->value.size);
				if(index != -1)
					memcpy(meta->source->ptr(index), e.object, meta->source->getsize());
				else // Add new one (already initialized)
					meta->source->setcount(meta->source->getcount() + 1);
			}
		}
	};
	reader e;
	return io::read(url, e);
}