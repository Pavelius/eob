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

static bool expand_group(const markup* p) {
	return p == dginf<point>::meta
		|| p == dginf<historyi>::meta;
}

static bool emphty_value(const char* value) {
	static const char* source[] = {"NONE", "None", "No monster", "No item"};
	if(!value || value[0] == 0)
		return true;
	for(auto v : source) {
		if(strcmp(v, value) == 0)
			return true;
	}
	return false;
}

static void write_req(serializer* sz, void* object, const markup* type, int index) {
	if(type->isdecortext())
		return;
	else if(type->ischeckboxes()) {
		sz->open(getheader(type->title, index), serializer::Array);
		auto ps = type->value.source;
		if(ps && type->list.getname) {
			auto flags = type->get(type->value.ptr(object), type->value.size);
			for(unsigned i = 0; i < ps->getcount(); i++) {
				if((flags&(1 << i)) == 0)
					continue;
				char value[1024]; stringbuilder sb(value);
				sz->set("element", type->list.getname(ps->ptr(i), sb));
			}
		}
		sz->close(getheader(type->title, index), serializer::Array);
	} else if(type->isgroup() || expand_group(type->value.type)) {
		sz->open(getheader(type->title, index), serializer::Struct);
		for(auto p = type->value.type; *p; p++)
			write_req(sz, type->value.ptr(object), p, p - type->value.type);
		sz->close(getheader(type->title, index), serializer::Struct);
	} else if(type->value.size) {
		char value[1024]; value[0] = 0;
		stringbuilder sb(value);
		type->getname(object, sb);
		if(type->value.isnum() && type->value.size <= sizeof(int) && !type->list.getname) {
			if(strcmp(value, "0") == 0)
				return;
			if(strcmp(value, "false") == 0)
				return;
			if(emphty_value(value))
				return;
			if(strcmp(value, "true")==0)
				sz->set(type->title, value, serializer::Number);
			else
				sz->set(type->title, sz2num(value), serializer::Number);
		} else {
			if(emphty_value(value))
				return;
			sz->set(type->title, value, serializer::Text);
		}
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

bool gamei::writetext(const char* url, std::initializer_list<variant_s> source) {
	auto pp = io::plugin::find(szext(url));
	if(!pp)
		return false;
	io::file file(url, StreamWrite | StreamText);
	auto sz = pp->write(file);
	sz->open("elements", serializer::Array);
	for(auto id : source) {
		auto& ei = bsdata<varianti>::elements[id];
		if(!ei.source || !ei.form)
			continue;
		auto pe = ei.source->end();
		auto p = ei.source->begin();
		if(id == Creature)
			p = (char*)ei.source->ptr(4);
		for(; p < pe; p += ei.source->getsize())
			write_obj(sz, p, ei.form);
	}
	sz->close("elements", serializer::Array);
	return true;
}

bool gamei::readtext(const char* url) {
	struct reader : serializer::reader {
		static int find_array(array* source, fntext getname, const char* value) {
			if(!source || !getname || !value || value[0] == 0)
				return -1;
			auto pe = source->end();
			for(auto p = source->begin(); p < pe; p += source->getsize()) {
				char temp[1024]; stringbuilder sb(temp);
				auto pn = getname(p, sb);
				if(!pn || pn[0] == 0)
					continue;
				if(strcmp(pn, value) == 0)
					return source->indexof(p);
			}
			return -1;
		}
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
					if(f->ischeckboxes())
						e.object_type = f;
					else
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
					auto object = pv->source->ptr(pv->source->getcount());
					if(pv->source->getcount() >= pv->source->getmaximum())
						object = pv->source->ptr(pv->source->getcount()-1);
					e.parent->object = object;
					e.parent->object_type = pv->form;
				}
			} else if(e.parent->object && e.parent->object_type) {
				auto object = e.parent->object;
				auto type = (const markup*)e.parent->object_type;
				if(type->ischeckboxes()) {
					auto i = find_array(type->value.source, type->list.getname, value);
					if(i != -1)
						type->set(object, type->value.size, type->get(object, type->value.size) | (1<<i));
					return;
				}
				auto f = type->find(e.name);
				if(!f)
					return;
				auto pv = f->value.ptr(object);
				if(f->list.getname == getnm<textable> && !f->value.source) {
					textable v; v.setname(value);
					f->set(pv, f->value.size, v.name);
				} else if(f->list.getname == getnm<variant>) {
					variant v = variant::find(value);
					f->set(pv, f->value.size, *((short unsigned*)&v));
				} else if(e.type == serializer::Number) {
					if(f->value.mask) {
						if(strcmp(value, "true") == 0)
							f->set(pv, f->value.size, f->get(pv, f->value.size) | f->value.mask);
						else if(strcmp(value, "false") == 0)
							f->set(pv, f->value.size, f->get(pv, f->value.size) & (~f->value.mask));
					} else
						f->set(pv, f->value.size, sz2num(value));
				} else if(f->value.source && f->list.getname && f->value.size<=sizeof(int)) {
					auto i = find_array(f->value.source, f->list.getname, value);
					if(i != -1)
						type->set(pv, f->value.size, i);
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
				auto pk = type->find("ID");
				if(!pk)
					pk = type->find("Name");
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
	clear();
	return io::read(url, e);
}