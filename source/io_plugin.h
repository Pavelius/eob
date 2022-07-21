#include "io_stream.h"
#include "stringbuilder.h"

#pragma once

namespace io {
// Plugin read file and post events to this class.
struct plugin {
	const char*			name;
	const char*			fullname;
	const char*			filter;
	static plugin*		first;
	plugin*				next;
	//
	plugin();
	static plugin*		find(const char* name);
	virtual const char*	read(const char* source, serializer::reader& r) = 0;
	virtual serializer*	write(stream& e) = 0;
};
bool					read(const char* url, serializer::reader& e);
}