#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct markup;

typedef bool(*fnallow)(const void* object, int index); // Is allow some property
typedef void(*fncommand)(void* object); // Object's actions
typedef int(*fndraw)(int x, int y, int width, const void* object); // Custom draw
typedef const char* (*fntext)(const void* object, stringbuilder& sb);
typedef void (*fnsource)(const void* object, array& source);

#define DGLNK(R,T) template<> struct dginf<R> : dginf<T> {};
#define DGMETA(T) const markup dginf<T>::meta[]
#define DGCHK(R, T1, T2, ID, MS) {dginf<meta_decoy<T1>::value>::meta,\
bsdata<meta_decoy<T2>::value>::source_ptr,\
(unsigned)&((data_type*)0)->R,\
sizeof(data_type::R),\
ID,\
MS}
#define DGREQ(R) DGCHK(R, decltype(data_type::R), decltype(data_type::R), 0, 0)

struct fnlist {
	fntext				getname;
	fnallow				allow;
	fnsource			source;
};
template<class T> struct dginf {
	typedef T			data_type;
	static const markup	meta[];
};
// Data binding element
struct markitem {
	const markup*		type;
	array*				source;
	unsigned			offset;
	unsigned			size;
	unsigned			index;
	unsigned			mask;
	//
	constexpr bool		isnum() const { return type == dginf<int>::meta; }
	constexpr bool		istext() const { return type == dginf<const char*>::meta; }
	void*				ptr(void* object) const { return (char*)object + offset + size * index; }
};
// Standart markup
struct markup {
	constexpr explicit operator bool() const { return title || value.type; }
	const char*			title;
	markitem			value;
	fnlist				list;
	fncommand			execute;
	//
	bool				is(const char* id) const;
	bool				ischeckboxes() const { return is("chk"); }
	bool				isdecortext() const { return value.type==0; }
	bool				isgroup() const { return is("grp"); }
};
DGLNK(char, int)
DGLNK(short, int)
DGLNK(unsigned char, int)
DGLNK(unsigned short, int)
DGLNK(unsigned, int)
DGLNK(bool, int)