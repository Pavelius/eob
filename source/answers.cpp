#include "main.h"

int compare(const void* v1, const void* v2) {
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
	richtexti rt;
	if(!rt.load(title))
		return 0;
	auto index = 0;
	imagei im = rt.images[index];
	while(index + 1 < rt.maximum && rt.data[index + 1]) {
		if(rt.images[index])
			im = rt.images[index];
		answers aw;
		aw.add(1, "Next");
		aw.choosebg(rt.data[index], im, true);
		index++;
	}
	if(rt.images[index])
		im = rt.images[index];
	if(!rt.data[index])
		return 0;
	return choosebg(rt.data[index], im, horizontal_buttons);
}