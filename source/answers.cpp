#include "main.h"

imagei last_image;
aref<actioni> last_menu;
const char* last_name;
const char* last_menu_header;
static char temp_buffer[260];
static char error_buffer[512];

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

static void error(const char* format, ...) {
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

static const char* readenum(const char* p, const array& source, int& value) {
	p = readid(p);
	value = source.find(temp_buffer, 0);
	if(value == -1)
		error("Can't find element with value \'%1\'", temp_buffer);
	return skipsp(p);
}

static const char* readvalue(const char* p, cityi& value) {
	while(ischa(*p)) {
		int record, bonus;
		p = readvalue(p, bsdata<cityabilityi>::source, record, bonus);
		value.abilities[record] += bonus;
	}
	return p;
}

static const char* readvalue(const char* p, adventuref& value) {
	int record;
	while(ischa(*p)) {
		p = readenum(p, bsdata<adventurei>::source, record);
		value.set(record);
	}
	return p;
}

static const char* readpart(const char* p, imagei& value) {
	auto index = bsdata<resourcei>::source.find(temp_buffer, 0);
	if(index == -1) {
		error("Can't find image resource \'%1\'", temp_buffer);
		value.res = ADVENTURE;
	} else
		value.res = (resource_s)index;
	return readvalue(p, value.frame);
}

static bool iscommand(const char* id) {
	return strcmp(temp_buffer, id) == 0;
}

static const char* parse_rich(const char* p, cityi& loot, adventuref& unlock, char* ps, const char* pe) {
	while(*p == '/') {
		p = readid(p + 1);
		if(iscommand("REWARD"))
			p = readvalue(p, loot);
		else if(iscommand("UNLOCK"))
			p = readvalue(p, unlock);
		else
			p = readpart(p, last_image);
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
	game.add(e);
}

static void apply_unlock(adventuref& source) {
	auto m = source.getmaximum();
	for(auto i = 0; i < m; i++) {
		if(source.is(i)) {
			if(bsdata<adventurei>::elements[i].stage == 0) {
				bsdata<adventurei>::elements[i].stage = 1;
				mslog("Unlocked %1", bsdata<adventurei>::elements[i].name);
			}
		}
	}
}

int	answers::choosebg(const char* title) const {
	cityi loot = {};
	adventuref unlock = {};
	auto push_image = last_image;
	last_image.clear();
	char temp[512]; temp[0] = 0; auto p = title;
	while(p && *p) {
		if(temp[0])
			pause(temp);
		p = parse_rich(p, loot, unlock, temp, zendof(temp));
	}
	apply_loot(loot);
	apply_unlock(unlock);
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

bool answers::confirm(const char* format, const char* positive, const char* negative) {
	if(!format || !format[0])
		return false;
	answers aw;
	aw.add(1, positive);
	aw.add(0, negative);
	return aw.choosebg(format) != 0;
}