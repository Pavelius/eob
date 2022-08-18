#include "main.h"

imagei answers::last_image;

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

static void pause(const char* format) {
	answers aw;
	aw.add(1, "Continue");
	aw.choosehz(format);
}

int	answers::choosebg(const char* title) const {
	auto push_image = last_image;
	char temp[512]; auto p = title;
	last_image.clear();
	temp[0] = 0;
	while(p && *p) {
		if(temp[0])
			pause(temp);
		p = richtexti::parse(p, last_image, temp, temp + sizeof(temp));
	}
	auto result = choosehz(temp);
	last_image = push_image;
	return result;
}

void answers::message(const char* format) {
	if(!format || !format[0])
		return;
	answers aw;
	aw.add(1, "Continue");
	aw.choosebg(format);
}