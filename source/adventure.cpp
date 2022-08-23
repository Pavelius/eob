#include "main.h"
#include "log.h"

using namespace log;

static const char* p;

struct svalue {
	const char*		text;
	int				number;
	void			clear() { memset(this, 0, sizeof(*this)); }
};
static svalue		value;

static void skipws() {
	while(*p && (*p == ' ' || *p == '\t'))
		p++;
}

static void skipwscr() {
	while(*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
		p++;
}

static bool isheader() {
	return p[0] == '#';
}

static void readid() {
	value.clear();
	char temp[260]; stringbuilder sb(temp);
	p = sb.psidf(p);
	skipws();
	value.text = szdup(temp);
}

static void readname() {
	value.clear();
	if(*p != '\"')
		return;
	char temp[4096]; stringbuilder sb(temp);
	p = sb.psstr(p + 1, '\"');
	if(*p == '\"')
		p++;
	skipws();
	value.text = szdup(temp);
}

static void readtext() {
	value.clear();
	skipwscr();
	char temp[4096];
	auto ps = temp;
	auto pe = zendof(temp);
	while(*p && !isheader()) {
		if(p[0] == 10 || p[0] == 13) {
			if(ps < pe)
				*ps++ = '\n';
			p = skipcr(p);
		} else {
			if(ps < pe)
				*ps++ = *p;
			p++;
		}
	}
	*ps = 0;
	while(ps > temp && (ps[-1] == 10 || ps[-1] == 13)) {
		ps--;
		ps[0] = 0;
	}
	value.text = szdup(temp);
}

static void readnumber() {
	value.clear();
	p = stringbuilder::read(p, value.number);
	skipws();
}

static void readenum(array& source) {
	value.clear();
	char temp[260]; stringbuilder sb(temp);
	auto p1 = p;
	if(*p == '\"') {
		p = sb.psstr(p + 1, '\"');
		if(*p == '\"')
			p++;
	} else
		p = sb.psidf(p);
	skipws();
	value.text = szdup(temp);
	auto i = source.find(value.text, 0);
	if(i == -1)
		log::error(p1, "Can't find element with value \'%1\'", temp);
	else
		value.number = i;
}

static bool isheader(const char* id) {
	if(!isheader())
		return false;
	auto p1 = p + 1;
	while(*p1) {
		if(*id == 0 && !ischa(*p1) && !isnum(*p1)) {
			p = p1; skipws();
			return true;
		}
		if(*p1 != *id)
			break;
		p1++; id++;
	}
	return false;
}

static bool reqheader(const char* id) {
	if(!need_continue)
		return false;
	if(!isheader(id)) {
		log::cerror(p, "Expected header \'%1\'", id);
		return false;
	}
	return true;
}

template<typename T>
static void readval(T& v) {
	readenum(bsdata<T>::source);
	v = (T)value.number;
}

void readval(int& v) {
	readnumber();
	v = value.number;
}

void readval(short& v) {
	readnumber();
	v = value.number;
}

void readval(char& v) {
	readnumber();
	v = value.number;
}

void readval(const char*& v) {
	readtext();
	v = value.text;
}

static void read_site(adventurei& e) {
	auto pv = e.addsite();
	if(!pv) {
		log::cerror(p, "Too many sites in adventure \'%1\'", e.name);
		return;
	}
	pv->head.keys[0] = KeyCooper;
	pv->head.keys[1] = KeySilver;
	pv->head.language = Human;
	pv->chance.curse = 5;
	readval(pv->head.type);
	readval(pv->levels);
	skipwscr();
	while(*p && need_continue) {
		if(isheader("Keys")) {
			readval(pv->head.keys[0]);
			readval(pv->head.keys[1]);
		} else if(isheader("Habbit")) {
			readval(pv->head.habbits[0]);
			readval(pv->head.habbits[1]);
		} else if(isheader("Language"))
			readval(pv->head.language);
		else if(isheader("Curse"))
			readval(pv->chance.curse);
		else if(isheader("Item"))
			readval(pv->head.wands);
		else if(isheader("Boss"))
			readval(pv->crypt.boss);
		else if(isheader("Corner")) {
			readval(pv->crypt.corner);
			readval(pv->crypt.corner_count);
		} else
			break;
		skipwscr();
	}
}

static void read_variable() {
	skipwscr();
	while(*p && need_continue && !isheader()) {
		readid();
		auto pv = bsdata<variablei>::find(value.text);
		if(!pv) {
			pv = bsdata<variablei>::add();
			pv->id = value.text;
			pv->maximum = 10;
		}
		readnumber(); pv->maximum = value.number;
		readnumber(); pv->minimum = value.number;
		skipwscr();
	}
}

void adventurei::read(const char* url) {
	p = log::read(url);
	if(!p)
		return;
	int history_current = gethistorymax();
	while(*p && need_continue) {
		if(isheader("Summary"))
			readval(summary);
		else if(isheader("Variable"))
			read_variable();
		else if(isheader("Name")) {
			readname(); name = value.text;
			skipwscr();
		} else if(isheader("Agree"))
			readval(agree);
		else if(isheader("Entering"))
			readval(entering);
		else if(isheader("History"))
			readval(history[history_current++]);
		else if(isheader("Site"))
			read_site(*this);
		else
			break;
		skipwscr();
	}
	if(reqheader("Reward")) {
		readtext();
		reward = value.text;
	}
	log::close();
}

void companyi::readc(const char* url) {
	p = log::read(url);
	if(!p)
		return;
	while(*p && need_continue) {
		if(isheader("Adventure")) {
			auto pv = bsdata<adventurei>::add();
			pv->clear();
			readid(); pv->id = value.text;
			skipwscr();
		} else if(isheader("Intro"))
			readval(intro);
		else if(isheader("City")) {
			readname(); city = value.text;
			readval(city_frame);
		} else
			log::cerror(p, "Expected valid header");
			skipwscr();
	}
	log::close();
}