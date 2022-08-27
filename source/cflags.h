#ifdef _MSC_VER
#include "my_initialize_list.h"
#else
#include <initializer_list>
#endif

#pragma once

// Abstract flag data bazed on enumerator
template<typename T, typename DT = unsigned>
class cflags {
	DT data = 0;
public:
	constexpr cflags() : data(0) {}
	cflags(const std::initializer_list<T>& list) : data() { for(auto e : list) add(e); }
	constexpr explicit operator bool() const { return data != 0; }
	constexpr void add(const T id) { data |= 1 << id; }
	constexpr void add(const cflags& e) { data |= e.data; }
	constexpr void clear() { data = 0; }
	constexpr bool is(const T id) const { return (data & (1 << id)) != 0; }
	constexpr bool allof(const cflags& e) const { return (data & e.data) == data; }
	constexpr bool is(const cflags& e) const { return (data & e.data) != 0; }
	constexpr void remove(T id) { data &= ~(1 << id); }
};


