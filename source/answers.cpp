#include "main.h"

imagei last_image;
aref<actioni> last_menu;
static cityi last_loot;
const char* last_name;
const char* last_menu_header;
static char temp_buffer[260];

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

static const char* readid(const char* p) {
	stringbuilder sb(temp_buffer);
	p = sb.psidf(p);
	return skipsp(p);
}

static const char* readvalue(const char* p, int& value) {
	p = stringbuilder::read(p, value);
	return skipsp(p);
}

static const char* readvalue(const char* p, unsigned short& value) {
	int v = 0;
	p = stringbuilder::read(p, v); value = v;
	return skipsp(p);
}

static const char* readvalue(const char* p, const array& source, int& record, int& value) {
	value = 0;
	p = readid(p);
	record = source.find(temp_buffer, 0);
	if(*p == '+' || *p == '-')
		p = readvalue(p, value);
	return skipsp(p);
}

static const char* readvalue(const char* p, cityi& value) {
	while(ischa(*p)) {
		int record, bonus;
		p = readvalue(p, bsdata<cityabilityi>::source, record, bonus);
		value.data[record] += bonus;
	}
	return p;
}

static bool iscommand(const char* id) {
	return strcmp(temp_buffer, id) == 0;
}

static const char* parse_rich(const char* p, char* ps, const char* pe) {
	while(*p == '/') {
		p = readid(p + 1);
		if(iscommand("REWARD"))
			p = readvalue(p, last_loot);
		else {
			auto index = bsdata<resourcei>::source.find(temp_buffer, 0);
			if(index == -1)
				return 0;
			last_image.res = (resource_s)index;
			p = readvalue(p, last_image.frame);
		}
		p = skipspcr(p);
	}
	while(*p && *p != 10) {
		if(ps < pe)
			*ps++ = *p;
		p++;
	}
	while(*p == 10)
		p++;
	*ps = 0;
	return p;
}

static void apply_loot(cityi& e) {
	game.addcity(e);
}

int	answers::choosebg(const char* title) const {
	auto push_image = last_image;
	auto push_loot = last_loot;
	last_loot.clear();
	last_image.clear();
	char temp[512]; temp[0] = 0; auto p = title;
	while(p && *p) {
		if(temp[0])
			pause(temp);
		p = parse_rich(p, temp, zendof(temp));
	}
	apply_loot(last_loot);
	auto result = choosehz(temp);
	last_loot = push_loot;
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

bool answers::confirm(const char* format, const char* positive, const char* negative) {
	if(!format || !format[0])
		return false;
	answers aw;
	aw.add(1, positive);
	aw.add(0, negative);
	return aw.choosebg(format) != 0;
}
