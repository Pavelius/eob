#pragma once

#ifdef _MSC_VER
#include "my_initialize_list.h"
#else
#include <initializer_list>
#endif

#ifdef _DEBUG
#define assert(e) if(!(e)) {exit(255);}
#else
#define assert(e)
#endif
#define maptbl(t, id) (t[imax((unsigned)0, imin((unsigned)id, (sizeof(t)/sizeof(t[0])-1)))])
#define maprnd(t) t[rand()%(sizeof(t)/sizeof(t[0]))]
#define lenof(t) (sizeof(t)/sizeof(t[0]))
#define zendof(t) (t + sizeof(t)/sizeof(t[0]) - 1)
#define BSDATA(e) template<> e bsdata<e>::elements[]
#define BSDATAC(e, c) e bsdata<e>::elements[c]; array bsdata<e>::source(bsdata<e>::elements, sizeof(bsdata<e>::elements[0]), 0, sizeof(bsdata<e>::elements)/sizeof(bsdata<e>::elements[0]));
#define BSLNK(R, S) template<> struct bsdata<R> : bsdata<S> {};
#define NOBSDATA(e) template<> struct bsdata<e> : bsdata<int> {};
#define BSDATAF(e) template<> array bsdata<e>::source(bsdata<e>::elements, sizeof(bsdata<e>::elements[0]), sizeof(bsdata<e>::elements)/sizeof(bsdata<e>::elements[0]));
#define FO(c, f) (unsigned)(&((c*)0)->f)
#define assert_enum(e, last) static_assert(sizeof(bsdata<e>::elements) / sizeof(bsdata<e>::elements[0]) == last + 1, "Invalid count of " #e " elements");

extern "C" int						atexit(void(*func)(void));
extern "C" void*					bsearch(const void* key, const void *base, unsigned num, unsigned size, int(*compar)(const void *, const void *));
extern "C" unsigned					clock(); // Returns the processor time consumed by the program.
extern "C" void						exit(int exit_code);
extern "C" int						memcmp(const void* p1, const void* p2, unsigned size);
extern "C" void*					memmove(void* destination, const void* source, unsigned size);
extern "C" void*					memcpy(void* destination, const void* source, unsigned size);
extern "C" void*					memset(void* destination, int value, unsigned size);
extern "C" void						qsort(void* base, unsigned num, unsigned size, int(*compar)(const void*, const void*));
extern "C" int						rand(void); // Get next random value
extern "C" void						sleep(unsigned time);
extern "C" void						srand(unsigned seed); // Set random seed
extern "C" int						strcmp(const char* s1, const char* s2); // Compare two strings
extern "C" long long				time(long long* seconds);

enum codepages { CPNONE, CP1251, CPUTF8, CPU16BE, CPU16LE };
namespace metrics {
const codepages						code = CP1251;
}
typedef void(*fnevent)();
typedef bool(*fnscript)(bool run);
typedef bool(*fntestcase)();
typedef int(*fnint)(const void* object);
typedef const char* (*fnstring)(const void* object);
bool								equal(const char* p, const char* s);
int									getdigitscount(unsigned number); // Get digits count of number. For example if number=100, result be 3.
bool								ischa(unsigned u); // is alphabetical character?
inline bool							isnum(unsigned u) { return u >= '0' && u <= '9'; } // is numeric character?
void*								loadb(const char* url, int* size = 0, int additional_bytes_alloated = 0); // Load binary file.
char*								loadt(const char* url, int* size = 0); // Load text file and decode it to system codepage.
bool								matchuc(const char* name, const char* filter);
unsigned							rmoptimal(unsigned need_count);
void								rmremove(void* data, unsigned size, unsigned index, unsigned& count, int elements_count);
void*								rmreserve(void* data, unsigned new_size);
void								rmreserve(void** data, unsigned count, unsigned& count_maximum, unsigned size);
inline const char*					skipsp(const char* p) { if(p) while(*p == 32 || *p == 9) p++; return p; }
inline const char*					skipspcr(const char* p) { if(p) while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++; return p; }
float								sqrt(const float x); // Return aquare root of 'x'
const char*							szdup(const char* p);
void								szencode(char* output, int output_count, codepages output_code, const char* input, int input_count, codepages input_code);
unsigned							szget(const char** input, codepages page = metrics::code);
int									szcmpi(const char* p1, const char* p2);
int									szcmpi(const char* p1, const char* p2, int count);
const char*							szext(const char* path);
const char*							szfname(const char* text); // Get file name from string (no fail, always return valid value)
char*								szfnamewe(char* result, const char* name); // get file name without extension (no fail)
unsigned							szlower(unsigned u); // to lower reg
void								szlower(char* p, int count = 1); // to lower reg
bool								szmatch(const char* text, const char* name); //
bool								szpmatch(const char* text, const char* pattern);
void								szput(char** output, unsigned u, codepages page = metrics::code);
char*								szput(char* output, unsigned u, codepages page = metrics::code); // Fast symbol put function. Return 'output'.
const char*							skipcr(const char* p);
const char*							szskipcrr(const char* p0, const char* p);
unsigned							szupper(unsigned u);
char*								szupper(char* p, int count = 1); // to upper reg
char*								szurl(char* p, const char* path, const char* name, const char* ext = 0, const char* suffix = 0);
char*								szurlc(char* p1);
inline int							xrand(int n1, int n2) { return n1 + rand() % (n2 - n1 + 1); }
// Common used templates
inline int							ifloor(double n) { return (int)n; }
template<class T> inline T			imax(T a, T b) { return a > b ? a : b; }
template<class T> inline T			imin(T a, T b) { return a < b ? a : b; }
template<class T> inline T			iabs(T a) { return a > 0 ? a : -a; }
template<class T> inline void		iswap(T& a, T& b) { T i = a; a = b; b = i; }
// Inline sequence functions
template<class T> inline T*			seqlast(T* p) { while(p->next) p = p->next; return p; } // Return last element in sequence.
template<class T> inline void		seqlink(T* p) { p->next = 0; if(!T::first) T::first = p; else seqlast(T::first)->next = p; }
// Inline strings functions
template<class T> inline const T*	zchr(const T* p, T e) { while(*p) { if(*p == e) return p; p++; } return 0; }
template<class T> inline void		zcpy(T* p1, const T* p2) { while(*p2) *p1++ = *p2++; *p1 = 0; }
template<class T> inline void		zcpy(T* p1, const T* p2, int max_count) { while(*p2 && max_count-- > 0) *p1++ = *p2++; *p1 = 0; }
template<class T> inline T*			zend(T* p) { while(*p) p++; return p; }
template<class T> inline void		zcat(T* p1, const T e) { p1 = zend(p1); p1[0] = e; p1[1] = 0; }
template<class T> inline void		zcat(T* p1, const T* p2) { zcpy(zend(p1), p2); }
template<class T> constexpr size_t	zlen(T* p) { return zend(p) - p; }
template<class T> inline void		zshuffle(T* p, int count) { for(int i = 0; i < count; i++) iswap(p[i], p[rand() % count]); }
// Storge like vector
template<class T, int count_max = 128>
struct adat {
	T						data[count_max];
	unsigned				count;
	constexpr adat() : count(0) {}
	constexpr adat(std::initializer_list<T> list) : count(0) { for(auto& e : list) *add() = e; }
	constexpr const T& operator[](unsigned index) const { return data[index]; }
	constexpr T& operator[](unsigned index) { return data[index]; }
	explicit operator bool() const { return count != 0; }
	T*						add() { if(count < count_max) return data + (count++); return 0; }
	void					add(const T& e) { if(count < count_max) data[count++] = e; }
	T*						begin() { return data; }
	const T*				begin() const { return data; }
	void					clear() { count = 0; }
	T*						end() { return data + count; }
	const T*				end() const { return data + count; }
	const T*				endof() const { return data + count_max; }
	int						getcount() const { return count; }
	int						getmaximum() const { return count_max; }
	int						indexof(const T* e) const { if(e >= data && e < data + count) return e - data; return -1; }
	int						indexof(const T t) const { for(unsigned i = 0; i < count; i++) if(data[i] == t) return i; return -1; }
	bool					is(const T t) const { return indexof(t) != -1; }
	T						random() const { return data[rand() % count]; }
	void					remove(int index, int remove_count = 1) { if(index < 0) return; if(index<int(count - 1)) memcpy(data + index, data + index + 1, sizeof(data[0])*(count - index - 1)); count--; }
};
typedef adat<void*, 64>		reflist;
// Reference to array with dymanic size
template<class T>
struct aref {
	T*						data;
	unsigned				count;
	constexpr aref() = default;
	constexpr aref(T* source, unsigned count) : data(source), count(count) {}
	template<unsigned N> constexpr aref(T(&data)[N]) : data(data), count(N) {}
	template<unsigned N> constexpr aref(adat<T, N>& source) : data(source.data), count(source.count) {}
	constexpr T& operator[](int index) { return data[index]; }
	constexpr const T& operator[](int index) const { return data[index]; }
	explicit operator bool() const { return count != 0; }
	constexpr T*			begin() { return data; }
	constexpr const T*		begin() const { return data; }
	constexpr T*			end() { return data + count; }
	constexpr const T*		end() const { return data + count; }
	int						getcount() const { return count; }
	int						indexof(const T* t) const { if(t<data || t>data + count) return -1; return t - data; }
	int						indexof(const T t) const { for(unsigned i = 0; i < count; i++) if(data[i] == t) return i; return -1; }
	bool					is(const T t) const { return indexof(t) != -1; }
};
// Abstract pair element
template<typename K, typename V>
struct pair {
	K						key;
	V						value;
};
// Abstract data set
template<typename K, typename T, unsigned N>
struct adatc {
	T						data[N];
	constexpr adatc() = default;
	constexpr adatc(const std::initializer_list<pair<K, T>>& it) : data() {
		for(auto& e : it)
			data[e.key] = e.value;
	}
	constexpr const T& operator[](int index) const { return data[index]; }
};
// Abstract array vector
class array {
	void*					data;
	unsigned				size;
	unsigned				count_maximum;
	bool					growable;
	friend struct archive;
public:
	unsigned				count;
	constexpr array() : data(0), size(0), count(0), count_maximum(0), growable(true) {}
	constexpr array(unsigned size) : data(0), size(size), count(0), count_maximum(0), growable(true) {}
	constexpr array(void* data, unsigned size, unsigned count) : data(data), size(size), count(count), count_maximum(0), growable(false) {}
	constexpr array(void* data, unsigned size, unsigned count, unsigned count_maximum) : data(data), size(size), count(count), count_maximum(count_maximum), growable(false) {}
	~array();
	void*					add();
	void*					add(const void* element);
	char*					begin() const { return (char*)data; }
	void					clear();
	char*					end() const { return (char*)data + size * count; }
	void*					find(const char* value) const;
	int						find(const char* value, unsigned offset) const;
	int						find(void* value, unsigned offset, unsigned size) const;
	unsigned				getmaximum() const { return count_maximum; }
	unsigned				getcount() const { return count; }
	unsigned				getsize() const { return size; }
	int						indexof(const void* element) const;
	void*					insert(int index, const void* element);
	bool					isgrowable() const { return growable; }
	void*					ptr(int index) const { return (char*)data + size * index; }
	void					remove(int index, int elements_count);
	void					setcount(unsigned value) { count = value; }
	void					setup(unsigned size);
	void					setup(void* data, unsigned size, unsigned count, unsigned count_maximum);
	void					shift(int i1, int i2, unsigned c1, unsigned c2);
	void					sort(int i1, int i2, int(*compare)(const void* p1, const void* p2, void* param), void* param);
	void					swap(int i1, int i2);
	void					reserve(unsigned count);
};
// Abstract data access class
template<typename T> struct bsdata {
	static T				elements[];
	static array			source;
	static constexpr array*	source_ptr = &source;
	//
	static T*				add() { return (T*)source.add(); }
	static constexpr T*		begin() { return elements; }
	static constexpr T*		end() { return elements + source.getcount(); }
	static T*				find(const char* id) { return (T*)source.find(id); }
	static constexpr const T& get(int v) { return bsdata<T>::elements[v]; }
};
template<> struct bsdata<int> { static constexpr array*	source_ptr = 0; };
NOBSDATA(unsigned)
NOBSDATA(short)
NOBSDATA(unsigned short)
NOBSDATA(char)
NOBSDATA(unsigned char)
NOBSDATA(const char*)