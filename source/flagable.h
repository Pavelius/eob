#pragma once

// Abstract flag set
template<unsigned N>
class flagable {
	static constexpr unsigned s = 8;
	unsigned char	data[N];
public:
	constexpr flagable() : data{0} {}
	constexpr void	add(const flagable& e) { for(unsigned i = 0; i < N; i++) data[i] |= e.data[i]; }
	void			clear() { memset(this, 0, sizeof(*this)); }
	constexpr bool	is(short unsigned v) const { return (data[v / s] & (1 << (v % s))) != 0; }
	constexpr bool	is(const flagable& e) const { for(unsigned i = 0; i < N; i++) if((data[i] & e.data[i]) != 0) return true; return false; }
	constexpr int	getcount() const { auto r = 0; for(auto i = 0; i < N * s; i++) if(is(i)) r++; return r; }
	constexpr int	getmaximum() const { return N * 8; }
	constexpr void	remove(short unsigned v) { data[v / s] &= ~(1 << (v % s)); }
	constexpr void	set(short unsigned v) { data[v / s] |= 1 << (v % s); }
	constexpr void	set(short unsigned v, bool activate) { if(activate) set(v); else remove(v); }
};
