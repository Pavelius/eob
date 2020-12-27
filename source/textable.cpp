#include "main.h"

array textable::strings;

static bool has(const char* value) {
	return textable::strings.indexof(value) != -1;
}

static const char* find(const char* text, int textc) {
	auto c = text[0];
	auto data = textable::strings.begin();
	auto count = textable::strings.getcount();
	int m = count - textc;
	if(m < 0)
		return 0;
	for(int i = 0; i < m; i++) {
		if(c == data[i]) {
			auto j = 1;
			auto p = &data[i];
			for(; j < textc; j++)
				if(p[j] != text[j])
					break;
			if(j == textc && p[j] == 0)
				return p;
		}
	}
	return 0;
}

static const char* add(const char* text, int textc) {
	if(!text || text[0]==0)
		return 0;
	if(has(text))
		return text;
	if(textc == -1)
		textc = zlen(text);
	auto r = find(text, textc);
	if(r)
		return r;
	auto count = textable::strings.getcount();
	textable::strings.reserve(count + textc + 1);
	auto result = (char*)textable::strings.ptr(count);
	memcpy(result, text, textc * sizeof(text[0]));
	result[textc] = 0;
	textable::strings.setcount(count + textc + 1);
	return result;
}

const char* textable::get() const {
	if(!id)
		return "";
	return (char*)strings.ptr(id);
}

void textable::set(const char* v) {
	if(!v || v[0] == 0)
		id = 0;
	else
		id = strings.indexof(add(v, -1));
}

void textable::initialize() {
	strings.setup(1);
	add("None", 4);
}