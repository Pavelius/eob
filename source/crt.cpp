#include "crt.h"
#include "io_stream.h"

extern "C" void* malloc(unsigned size);
extern "C" void* realloc(void *ptr, unsigned size);
extern "C" void	free(void* pointer);

unsigned rmoptimal(unsigned need_count) {
	const unsigned mc = 256 * 256 * 256;
	unsigned m = 16;
	while(m < mc) {
		if(need_count < m)
			return m;
		m = m << 1;
	}
	return m;
}

void* rmreserve(void* data, unsigned new_size) {
	if(data)
		return realloc(data, new_size);
	return malloc(new_size);
}

void rmreserve(void** data, unsigned count, unsigned& count_maximum, unsigned size) {
	if(count >= count_maximum) {
		count_maximum = rmoptimal(count + 1);
		*data = rmreserve(*data, count_maximum * size);
	}
}

void rmremove(void* data, unsigned size, unsigned index, unsigned& count, int elements_count) {
	if(index >= count)
		return;
	count -= elements_count;
	if(index >= count)
		return;
	memmove((char*)data + index*size, (char*)data + (index + elements_count)*size, (count - index)*size);
}

int szcmpi(const char* p1, const char* p2) {
	while(*p2 && *p1) {
		unsigned s1 = szupper(szget(&p1));
		unsigned s2 = szupper(szget(&p2));
		if(s1 == s2)
			continue;
		return s1 - s2;
	}
	unsigned s1 = szupper(szget(&p1));
	unsigned s2 = szupper(szget(&p2));
	return s1 - s2;
}

int szcmpi(const char* p1, const char* p2, int max_count) {
	while(*p2 && *p1 && max_count-- > 0) {
		unsigned s1 = szupper(szget(&p1));
		unsigned s2 = szupper(szget(&p2));
		if(s1 == s2)
			continue;
		return s1 - s2;
	}
	if(!max_count)
		return 0;
	unsigned s1 = szupper(szget(&p1));
	unsigned s2 = szupper(szget(&p2));
	return s1 - s2;
}

bool matchuc(const char* name, const char* filter) {
	if(!name || name[0] == 0)
		return false;
	auto sym = szupper(szget(&filter));
	auto pn = name;
	while(pn[0]) {
		auto sym1 = szupper(szget(&pn));
		if(sym1 == sym) {
			auto pf = filter;
			auto ps = pn;
			while(true) {
				if(pf[0] == 0)
					return true;
				auto sym2 = szupper(szget(&pf));
				auto sym1 = szupper(szget(&pn));
				if(sym1 != sym2)
					break;
			}
			pn = ps;
		}
	}
	return false;
}

bool ischa(unsigned u) {
	return (u >= 'A' && u <= 'Z')
		|| (u >= 'a' && u <= 'z')
		|| (u >= 0x410 && u <= 0x44F);
}

unsigned szupper(unsigned u) {
	if(u >= 0x61 && u <= 0x7A)
		return u - 0x61 + 0x41;
	else if(u >= 0x430 && u <= 0x44F)
		return u - 0x430 + 0x410;
	return u;
}

char* szupper(char* p, int count) {
	char* s1 = p;
	const char* p1 = p;
	while(count-- > 0)
		szput(&s1, szupper(szget(&p1)));
	return p;
}

unsigned szlower(unsigned u) {
	if(u >= 0x41 && u <= 0x5A)
		return u - 0x41 + 0x61;
	else if(u >= 0x410 && u <= 0x42F)
		return u - 0x410 + 0x430;
	return u;
}

int getdigitscount(unsigned number) {
	if(number < 10)
		return 1;
	if(number < 100)
		return 2;
	if(number < 1000)
		return 3;
	if(number < 10000)
		return 4;
	if(number < 100000)
		return 5;
	if(number < 1000000)
		return 6;
	if(number < 10000000)
		return 7;
	if(number < 100000000)
		return 8;
	return 9;
}

void szlower(char* p, int count) {
	char* s1 = p;
	const char* p1 = p;
	if(count == -1) {
		while(true) {
			unsigned sym = szget(&p1);
			if(!sym)
				break;
			szput(&s1, szlower(sym));
		}
		szput(&s1, 0);
	} else {
		while(count-- > 0)
			szput(&s1, szlower(szget(&p1)));
	}
}

unsigned szget(const char** input, codepages code) {
	const unsigned char* p;
	unsigned result;
	switch(code) {
	case CPUTF8:
		p = (unsigned char*)*input;
		result = *p++;
		if(result >= 192 && result <= 223)
			result = (result - 192) * 64 + (*p++ - 128);
		else if(result >= 224 && result <= 239) {
			result = (result - 224) * 4096 + (p[0] - 128) * 64 + (p[1] - 128);
			p += 2;
		}
		*input = (const char*)p;
		return result;
	case CPU16LE:
		p = (unsigned char*)*input;
		result = p[0] | (p[1] << 8);
		*input = (const char*)(p + 2);
		return result;
	case CP1251:
		result = (unsigned char)*(*input)++;
		if(((unsigned char)result >= 0xC0))
			return result - 0xC0 + 0x410;
		else switch(result) {
		case 0xB2: return 0x406;
		case 0xAF: return 0x407;
		case 0xB3: return 0x456;
		case 0xBF: return 0x457;
		}
		return result;
	default:
		return *(*input)++;
	}
}

void szput(char** output, unsigned value, codepages code) {
	char* p;
	switch(code) {
	case CPUTF8:
		p = *output;
		if(((unsigned short)value) < 128)
			*p++ = (unsigned char)value;
		else if(((unsigned short)value) < 2047) {
			*p++ = (unsigned char)(192 + (((unsigned short)value) / 64));
			*p++ = (unsigned char)(128 + (((unsigned short)value) % 64));
		} else {
			*p++ = (unsigned char)(224 + (((unsigned short)value) / 4096));
			*p++ = (unsigned char)(128 + ((((unsigned short)value) / 64) % 64));
			*p++ = (unsigned char)(224 + (((unsigned short)value) % 64));
		}
		*output = p;
		break;
	case CP1251:
		if(value >= 0x410 && value <= 0x44F)
			value = value - 0x410 + 0xC0;
		else switch(value) {
		case 0x406: value = 0xB2; break; // I
		case 0x407: value = 0xAF; break; // �
		case 0x456: value = 0xB3; break;
		case 0x457: value = 0xBF; break;
		}
		*(*output)++ = (unsigned char)value;
		break;
	case CPU16LE:
		*(*output)++ = (unsigned char)(value & 0xFF);
		*(*output)++ = (unsigned char)(((unsigned)value >> 8));
		break;
	case CPU16BE:
		*(*output)++ = (unsigned char)(((unsigned)value >> 8));
		*(*output)++ = (unsigned char)(value & 0xFF);
		break;
	default:
		*(*output)++ = (unsigned char)value;
		break;
	}
}

char* szput(char* result, unsigned sym, codepages page) {
	char* p = result;
	szput(&p, sym, page);
	*p = 0;
	return result;
}

void szencode(char* output, int output_count, codepages output_code, const char* input, int input_count, codepages input_code) {
	char* s1 = output;
	char* s2 = s1 + output_count;
	const char* p1 = input;
	const char* p2 = p1 + input_count;
	while(p1 < p2 && s1 < s2)
		szput(&s1, szget(&p1, input_code), output_code);
	if(s1 < s2) {
		s1[0] = 0;
		if((output_code == CPU16BE || output_code == CPU16LE) && (s1 + 1) < s2)
			s1[1] = 0;
	}
}

void* array::add() {
	if(count >= count_maximum) {
		if(isgrowable())
			reserve(count + 1);
		else
			return (char*)data;
	}
	return (char*)data + size * (count++);
}

void* array::add(const void* element) {
	auto p = add();
	memcpy(p, element, getsize());
	return p;
}

array::~array() {
	if(isgrowable())
		clear();
}

void array::clear() {
	count = 0;
	if(!isgrowable())
		return;
	count_maximum = 0;
	if(data)
		delete (char*)data;
	data = 0;
}

void array::setup(void* data, unsigned size, unsigned count, unsigned count_maximum) {
	clear();
	this->data = data;
	this->size = size;
	this->count = count;
	this->count_maximum = count_maximum;
	this->growable = false;
}

void array::setup(unsigned size) {
	if(!isgrowable())
		return;
	clear();
	this->size = size;
}

void array::reserve(unsigned count) {
	if(!isgrowable())
		return;
	if(!size)
		return;
	if(data && count < count_maximum)
		return;
	count_maximum = rmoptimal(count);
	if(data)
		data = realloc(data, count_maximum*size);
	else
		data = malloc(count_maximum*size);
}

int array::find(const char* value, unsigned offset) const {
	auto m = getcount();
	for(unsigned i = 0; i < m; i++) {
		auto p = (const char**)((char*)ptr(i) + offset);
		if(!(*p))
			continue;
		if(strcmp(*p, value) == 0)
			return i;
	}
	return -1;
}

void* array::find(const char* value) const {
	auto i = find(value, 0);
	if(i == -1)
		return 0;
	return ptr(i);
}

int array::find(void* value, unsigned offset, unsigned size) const {
	auto m = getcount();
	for(unsigned i = 0; i < m; i++) {
		if(memcmp(value, (char*)ptr(i) + offset, size) == 0)
			return i;
	}
	return -1;
}

void array::sort(int i1, int i2, int(*compare)(const void* p1, const void* p2, void* param), void* param) {
	for(int i = i2; i > i1; i--) {
		for(int j = i1; j < i; j++) {
			auto t1 = ptr(j);
			auto t2 = ptr(j + 1);
			if(compare(t1, t2, param) > 0)
				swap(j, j + 1);
		}
	}
}

void array::remove(int index, int elements_count) {
	if(((unsigned)index) >= count)
		return;
	if((unsigned)index < count - elements_count)
		memcpy(ptr(index), ptr(index + elements_count), (count - (index + elements_count))*getsize());
	count -= elements_count;
}

int	array::indexof(const void* element) const {
	if(element >= data && element < ((char*)data + size*count))
		return ((char*)element - (char*)data) / size;
	return -1;
}

void* array::insert(int index, const void* element) {
	auto count_before = getcount(); add();
	memmove((char*)data + (index + 1)*size, (char*)data + index * size, (count_before - index)*size);
	void* p = ptr(index);
	if(element)
		memcpy(p, element, size);
	else
		memset(p, 0, size);
	return p;
}

void array::swap(int i1, int i2) {
	unsigned char* a1 = (unsigned char*)ptr(i1);
	unsigned char* a2 = (unsigned char*)ptr(i2);
	for(unsigned i = 0; i < size; i++) {
		char a = a1[i];
		a1[i] = a2[i];
		a2[i] = a;
	}
}

void array::shift(int i1, int i2, unsigned c1, unsigned c2) {
	if(i2 < i1) {
		iswap(i2, i1);
		iswap(c1, c2);
	}
	unsigned char* a1 = (unsigned char*)ptr(i1);
	unsigned char* a2 = (unsigned char*)ptr(i2);
	unsigned s1 = c1 * size;
	unsigned s2 = c2 * size;
	unsigned s = (a2 - a1) + s2 - 1;
	for(unsigned i = 0; i < s1; i++) {
		auto a = a1[0];
		memcpy(a1, a1 + 1, s);
		a1[s] = a;
	}
}

bool equal(const char* p, const char* s) {
	while(*s && *p)
		if(szupper(szget(&s)) != szupper(szget(&p)))
			return false;
	return true;
}