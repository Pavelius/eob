#include "crt.h"
#include "io.h"

#pragma once

// Fast and simple driver for streaming binary data
struct archive {
	io::stream&		source;
	bool			writemode;
	archive(io::stream& source, bool writemode) : source(source), writemode(writemode) {}
	bool			signature(const char* id);
	bool			version(short major, short minor);
	void			set(array& value);
	template<typename T> void set(T& value) {
		if(writemode)
			source.write(&value, sizeof(value));
		else
			source.read(&value, sizeof(value));
	}
};