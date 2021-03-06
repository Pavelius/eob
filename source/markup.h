#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct markup;

typedef bool(*fnallow)(const void* object, const void* pointer); // Is allow some pointer value
typedef void(*fncommand)(void* object); // Object's actions
typedef int(*fndraw)(int x, int y, int width, const void* object); // Custom draw
typedef const char* (*fntext)(const void* object, stringbuilder& sb);
typedef bool(*fnchoose)(void* object, const array& source, void* pointer);
typedef void*(*fnptr)(const void* object, int index);
typedef bool(*fnvisible)(const void* object);

#define DGLNK(L,T) template<> struct dginf<L> : dginf<T> {};
#define DGINF(T) const markup dginf<T>::meta[]
#define DGLST(T) {dginf<meta_decoy<T>::value>::meta, bsdata<meta_decoy<T>::value>::source_ptr}
#define DGGEN(R, T1, T2, MS) {dginf<meta_decoy<T1>::value>::meta,\
bsdata<meta_decoy<T2>::value>::source_ptr,\
(unsigned)&((data_type*)0)->R,\
sizeof(data_type::R),\
MS}
#define DGINH(R,F) {dginf<meta_decoy<R>::value>::meta,\
0,\
(unsigned)&((data_type*)0)->F,\
sizeof(R),\
0}
#define DGREQ(R) DGGEN(R, decltype(data_type::R), decltype(data_type::R), 0)
#define DGENM(R, T) DGGEN(R, T, T, 0)
#define DGCHK(R, M) DGGEN(R, decltype(data_type::R), decltype(data_type::R), M)

struct fnlist {
	fntext				getname;
	fnallow				match;
	fnchoose			choose;
	fndraw				preview;
	int					view_width;
	fnptr				getptr;
	fncommand			update;
};
struct fnelement {
	fnvisible			visible;
	fntext				getheader;
	fnallow				visiblex;
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
	unsigned			mask;
	//
	constexpr bool		isnum() const { return type == dginf<int>::meta; }
	constexpr bool		isreference() const { return source != 0; }
	constexpr bool		istext() const { return type == dginf<const char*>::meta; }
	void*				ptr(void* object) const { return (char*)object + offset; }
};
// Standart markup
struct markup {
	constexpr explicit operator bool() const { return title || value.type; }
	const char*			title;
	markitem			value;
	fnlist				list;
	fnelement			proc;
	//
	const markup*		find(const char* name) const;
	static int			get(void* p, unsigned size);
	void				getname(const void* p, stringbuilder& sb) const;
	bool				is(const char* id) const;
	bool				ischeckboxes() const { return is("chk"); }
	bool				isdecortext() const { return value.type == 0; }
	bool				isgroup() const { return value.type != 0 && !list.getname && !value.istext() && !value.isnum(); }
	bool				ispage() const { return title && title[0] == '#'; }
	bool				isvisible(const void* object) const;
	static void			set(void* p, unsigned size, int v);
};
DGLNK(char, int)
DGLNK(short, int)
DGLNK(unsigned char, int)
DGLNK(unsigned short, int)
DGLNK(unsigned, int)
DGLNK(bool, int)