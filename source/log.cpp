#include "crt.h"
#include "io_stream.h"
#include "log.h"
#include "stringbuilder.h"

static int error_count;
static const char* current_url;
static const char* current_file;

void log::setfile(const char* v) {
	current_file = v;
}

void log::seturl(const char* v) {
	current_url = v;
}

const char* log::read(const char* url, bool error_if_not_exist) {
	auto p_alloc = loadt(url);
	if(!p_alloc) {
		current_url = 0;
		if(error_if_not_exist)
			error(0, "Can't find file '%1'", url);
		return 0;
	}
	seturl(url);
	setfile(p_alloc);
	return p_alloc;
}

void log::close() {
	if(current_file)
		delete current_file;
	current_file = 0;
}

const char* endline(const char* p) {
	while(*p && !(*p == 10 || *p == 13))
		p++;
	return p;
}

int getline(const char* pb, const char* pc) {
	auto p = pb;
	auto r = 0;
	while(*p && p < pc) {
		p = endline(p);
		p = skipcr(p);
		r++;
	}
	return r;
}

void log::errorv(const char* position, const char* format) {
	static io::file file("errors.txt", StreamWrite | StreamText);
	if(!file)
		return;
	error_count++;
	if(position)
		file << " Line " << getline(current_file, position) << ": ";
	file << format << "\n";
}

void log::error(const char* position, const char* format, ...) {
	char temp[4096]; stringbuilder sb(temp);
	if(current_url) {
		sb.add("In file `%1`:", current_url);
		current_url = 0;
		errorv(0, temp);
		sb.clear();
	}
	sb.addv(format, xva_start(format));
	errorv(position, temp);
}

int log::geterrors() {
	return error_count;
}

const char* log::getfilestart() {
	return current_file;
}