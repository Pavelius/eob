#include "archive.h"

bool archive::signature(const char* id) {
	char temp[4];
	if(writemode) {
		memset(temp, 0, sizeof(temp));
		zcpy(temp, id, sizeof(temp) - 1);
		source.write(temp, sizeof(temp));
	} else {
		source.read(temp, sizeof(temp));
		if(szcmpi(temp, id) != 0)
			return false;
	}
	return true;
}

bool archive::version(short major, short minor) {
	short major_reader = major;
	short minor_reader = minor;
	set(major_reader);
	set(minor_reader);
	if(!writemode) {
		if(major_reader < major)
			return false;
		else if(major_reader == major && minor_reader < minor)
			return false;
	}
	return true;
}

void archive::set(array& value) {
	if(value.isgrowable()) {
		if(writemode) {
			source.write(&value.size, sizeof(value.size));
			source.write(&value.count, sizeof(value.count));
			source.write(value.data, value.size*value.count);
		} else {
			value.clear();
			source.read(&value.size, sizeof(value.size));
			unsigned count = 0;
			source.read(&count, sizeof(count));
			value.reserve(count);
			value.count = count;
			source.read(value.data, value.size*value.count);
		}
	} else {
		set(value.count);
		if(writemode)
			source.write(value.data, value.size*value.count);
		else
			source.read(value.data, value.size*value.count);
	}
}