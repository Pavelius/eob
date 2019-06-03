#include "main.h"

void dialogi::choose() const {
	auto p = find("main");
	while(p) {
		answers aw;
		for(auto& e : p->variants)
			aw.add((int)&e, e.text);
	}
}

const dialogi* dialogi::find(const char* id) const {
	if(!this || !id)
		return 0;
	for(auto p = this; *p; p++) {
		if(strcmp(p->id, id) == 0)
			return p;
	}
	return 0;
}