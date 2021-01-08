#include "main.h"

imagei	answers::last_image;

int answers::compare(const void* v1, const void* v2) {
	return strcmp(((answers::element*)v1)->text, ((answers::element*)v2)->text);
}

answers::answers() : sc(buffer) {}

void answers::addv(int id, const char* text, const char* format) {
	auto p = elements.add();
	p->id = id;
	p->text = sc.get(); sc.addv(text, format); sc.addsz();
}

void answers::sort() {
	qsort(elements.data, elements.count, sizeof(elements.data[0]), compare);
}

int	answers::random() const {
	if(!elements.count)
		return 0;
	return elements.data[rand() % elements.count].id;
}

int answers::choose(const char* title, bool interactive) const {
	if(interactive)
		return choose(title);
	return random();
}

int	answers::choosebg(const char* title, bool horizontal_buttons) const {
	char temp[400]; auto p = title;
	imagei im = {};
	temp[0] = 0;
	while(p && *p) {
		if(temp[0])
			message(temp, im);
		p = richtexti::parse(p, im, temp, temp + sizeof(temp));
	}
	return choosebg(temp, im, horizontal_buttons);
}

void answers::message(const char* format) {
	answers aw;
	aw.add(1, "Continue");
	aw.choosebg(format);
}

void answers::message(const char* format, const imagei& im) {
	answers aw;
	aw.add(1, "Continue");
	aw.choosebg(format, im, true);
}