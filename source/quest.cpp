#include "main.h"
#include "log.h"

const char* szdup(const char* p);
static const char* p;
static bool need_continue;

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
	return p[0] == '#' && p[1] == '#';
}

static void readid() {
	value.clear();
	char temp[260];
	p = psidn(p, temp, zendof(temp));
	skipws();
	value.text = szdup(temp);
}

static void readname() {
	value.clear();
	if(*p != '\"')
		return;
	char temp[4096];
	p = psstr(p + 1, temp, '\"');
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
	p = psnum(p, value.number);
	skipws();
}

static void readenum(array& source) {
	value.clear();
	char temp[128];
	auto p1 = p;
	if(*p == '\"') {
		p = psstr(p + 1, temp, '\"');
		if(*p == '\"')
			p++;
	} else
		p = psidn(p, temp);
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
	auto p1 = p + 2;
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
		need_continue = false;
		log::error(p, "Expected header \'%1\'", id);
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

void readval(char& v) {
	readnumber();
	v = value.number;
}

static void read_site(adventurei& e) {
	auto pv = e.addsite();
	if(!pv) {
		need_continue = false;
		log::error(p, "Too many sites in adventure \'%1\'", e.name);
		return;
	}
	pv->head.keys[0] = KeyCooper;
	pv->head.keys[1] = KeySilver;
	pv->head.language = Human;
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
		else if(isheader("Boss"))
			readval(pv->crypt.boss);
		else
			break;
		skipwscr();
	}
}

static void read_adventure() {
	auto pq = bsdata<adventurei>::add();
	readname(); pq->name = value.text;
	skipwscr();
	while(*p && need_continue) {
		if(isheader("Summary")) {
			readtext();
			pq->summary = value.text;
		} else if(isheader("Agree")) {
			readtext();
			pq->agree = value.text;
		} else if(isheader("Entering")) {
			readtext();
			pq->entering = value.text;
		} else if(isheader("Site")) {
			read_site(*pq);
		} else
			break;
		skipwscr();
	}
	if(reqheader("Reward")) {
		readtext();
		pq->reward = value.text;
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

void companyi::readn(const char* url) {
	p = log::read(url);
	if(!p)
		return;
	need_continue = true;
	while(*p && need_continue) {
		if(isheader("Quest"))
			read_adventure();
		else if(isheader("Variable"))
			read_variable();
		else {
			need_continue = false;
			log::error(p, "Expected valid header");
		}
	}
	log::close();
}