#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct markup;

typedef bool(*fnallow)(const void* object, int index); // Is allow some property
typedef void(*fncommand)(void* object); // Object's actions
typedef int(*fndraw)(int x, int y, int width, const void* object); // Custom draw
typedef const char* (*fntext)(const void* object, stringbuilder& sb);

#define DGLNK(R,T) template<> struct dginf<R> : dginf<T> {};
#define DGMETA(T) const markup dginf<T>::meta[]
#define DGREQ(R) {dginf<meta_decoy<decltype(data_type::R)>::value>::meta,\
(unsigned)&((data_type*)0)->R,\
sizeof(data_type::R),\
0}

template<class T> struct dginf {
	typedef T			data_type;
	static const markup	meta[];
};
// Data binding element
struct markitem {
	const markup*		type;
	unsigned			offset;
	unsigned			size;
	unsigned			index;
	//
	constexpr bool		isnum() const { return type == dginf<int>::meta; }
	constexpr bool		istext() const { return type == dginf<const char*>::meta; }
	int					get(const void* value_ptr) const;
	void				getname(const void* object, stringbuilder& sb, fntext pgetname = 0) const;
	void*				ptr(void* object) const { return (char*)object + offset + size * index; }
};
// Standart markup
struct markup {
	constexpr explicit operator bool() const { return title || value.type; }
	int					width;
	const char*			title;
	markitem			value;
	fnallow				isallow;
	fntext				getname;
	//
	bool				is(const char* id) const;
	bool				ischeckboxes() const { return is("chk"); }
	bool				isdecortext() const { return value.type==0; }
	bool				isgroup() const { return is("grp"); }
};
// Data type descriptor element
struct formi {
	const char*			name;
	const char*			nameof;
	fntext				getname;
	const markup*		meta;
	array*				source;
};
DGLNK(char, int)
DGLNK(short, int)
DGLNK(unsigned char, int)
DGLNK(unsigned short, int)
DGLNK(unsigned, int)
DGLNK(bool, int)