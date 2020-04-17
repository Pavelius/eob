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
	template<typename T, unsigned N> void set(T(&value)[N]) {
		for(int i = 0; i < N; i++)
			set(value[i]);
	};
	template<typename T> void set(T& value) {
		if(writemode)
			source.write(&value, sizeof(value));
		else
			source.read(&value, sizeof(value));
	}
};