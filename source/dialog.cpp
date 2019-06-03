#include "main.h"

void dialogi::choose(bool border) const {
	auto p = find("main");
	while(p) {
		answers aw;
		for(auto& e : p->variants)
			aw.add((int)&e, e.text);
		auto pe = (elementi*)aw.choosebg(p->text, border);
		if(!pe || !pe->success)
			break;
		p = find(pe->success);
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