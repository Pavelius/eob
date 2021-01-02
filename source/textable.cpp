#include "main.h"

static bool has(const char* value) {
	return textable::getstrings().indexof(value) != -1;
}

static const char* find(const char* text, int textc) {
	auto c = text[0];
	auto& strings = textable::getstrings();
	auto data = strings.begin();
	auto count = strings.getcount();
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
	auto& strings = textable::getstrings();
	if(strings.getcount() == 0) {
		auto p = (char*)strings.add();
		p[0] = 0;
	}
	if(has(text))
		return text;
	if(textc == -1)
		textc = zlen(text);
	auto r = find(text, textc);
	if(r)
		return r;
	auto count = strings.getcount();
	strings.reserve(count + textc + 1);
	auto result = (char*)strings.ptr(count);
	memcpy(result, text, textc * sizeof(text[0]));
	result[textc] = 0;
	strings.setcount(count + textc + 1);
	return result;
}

const char* textable::getname() const {
	return name ? (char*)getstrings().ptr(name) : "";
}

array& textable::getstrings() {
	static array source(sizeof(char));
	return source;
}

void textable::setname(const char* name) {
	if(name && name[0])
		this->name = getstrings().indexof(add(name, -1));
	else
		this->name = 0;
}