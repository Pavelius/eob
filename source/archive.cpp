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

bool archive::checksum(unsigned long long v) {
	if(writemode)
		set(v);
	else {
		unsigned long long v1 = 0;
		set(v1);
		if(v1 != v)
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
			source.write(value.data, value.size * value.count);
		} else {
			value.clear();
			source.read(&value.size, sizeof(value.size));
			unsigned count = 0;
			source.read(&count, sizeof(count));
			value.reserve(count);
			value.count = count;
			source.read(value.data, value.size * value.count);
		}
	} else {
		set(value.count);
		if(writemode)
			source.write(value.data, value.size * value.count);
		else
			source.read(value.data, value.size * value.count);
	}
}

void archive::set(const char*& value) {
	if(writemode) {
		auto n = value ? zlen(value) : 0;
		source.write(n);
		if(n)
			source.write(value, n);
	} else {
		int n = 0;
		source.read(n);
		if(n) {
			char temp[512];
			auto pb = temp;
			if(n > (sizeof(temp) - 1))
				pb = new char[n + 1];
			source.read(pb, n);
			pb[n] = 0;
			value = szdup(pb);
			if(pb != temp)
				delete[] pb;
		} else
			value = 0;
	}
}