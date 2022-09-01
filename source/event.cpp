#include "crt.h"
#include "event.h"

BSDATA(eventsi) = {
	{"Time"},
	{"Expand"},
	{"Overwelmed"},
};
assert_enum(eventsi, VariableOverwelmedEvent)
BSDATAF(eventsi)

BSDATAC(eventi, 256)

eventi* eventi::add(event_s type) {
	auto p = bsdata<eventi>::add();
	p->clear();
	p->type = type;
	return p;
}

void eventi::clear() {
	memset(this, 0, sizeof(*this));
}

eventi* eventi::findtimer() {
	unsigned maximum = 0;
	eventi* result = 0;
	for(auto& e : bsdata<eventi>()) {
		if(e.type != TimeEvent)
			continue;
		if(e.param2 < maximum) {
			maximum = e.param2;
			result = &e;
		}
	}
	return result;
}

unsigned eventi::getstart() const {
	switch(type) {
	case TimeEvent: return param1;
	default: return 0;
	}
}

unsigned eventi::getfinish() const {
	switch(type) {
	case TimeEvent: return param2;
	default: return 0;
	}
}