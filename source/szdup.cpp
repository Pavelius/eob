#include "crt.h"

namespace {
class storage {
	storage*	next;
	int			count;
	char		data[256 * 255]; // Inner buffer
	bool has(const char* value) {
		auto e = this;
		while(e) {
			if(value >= e->data && value <= e->data + e->count)
				return true;
			e = e->next;
		}
		return false;
	}
	const char* find(const char* text, int textc) {
		if(textc == -1)
			textc = zlen(text);
		auto c = text[0];
		for(auto t = this; t; t = t->next) {
			int m = t->count - textc;
			if(m < 0)
				continue;
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
		}
		return 0;
	}
public:
	storage() : next(0), count(0) {
	}
	~storage() {
		auto z = next;
		while(z) {
			auto n = z->next;
			z->next = 0;
			delete z;
			z = n;
		}
		next = 0;
	}
	const char* add(const char* text, int textc) {
		if(!text)
			return 0;
		if(has(text))
			return text;
		auto r = find(text, textc);
		if(r)
			return r;
		auto t = this;
		while(true) {
			if((unsigned)(t->count + textc + 1) > sizeof(data) / sizeof(data[0])) {
				if(!t->next)
					t->next = new storage;
				if(!t->next)
					return 0;
				t = t->next;
				continue;
			}
			auto result = &t->data[t->count];
			memcpy(result, text, textc * sizeof(text[0]));
			result[textc] = 0;
			t->count += textc + 1;
			return result;
		}
	}
};
}

const char* szdup(const char* text) {
	static storage small;
	static storage big;
	if(!text)
		return 0;
	if(text[0] == 0)
		return "";
	auto lenght = zlen(text);
	if(lenght < 32)
		return small.add(text, lenght);
	else
		return big.add(text, lenght);
}