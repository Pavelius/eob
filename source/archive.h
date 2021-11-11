#include "crt.h"
#include "io.h"

#pragma once

// Fast and simple driver for streaming binary data
struct archive {
	io::stream&		source;
	bool			writemode;
	archive(io::stream& source, bool writemode) : source(source), writemode(writemode) {}
	bool			checksum(unsigned long long v);
	bool			signature(const char* id);
	void			set(array& value);
	bool			version(short major, short minor);
	template<typename T> void set(T& value) {
		if(writemode)
			source.write(&value, sizeof(value));
		else
			source.read(&value, sizeof(value));
	}
};