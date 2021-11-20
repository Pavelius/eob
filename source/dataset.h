#pragma once

template<unsigned N, class DT = char>
class dataset {
	DT				data[N + 1];
public:
	constexpr static auto maximal_element = N;
	typedef DT		data_type;
	constexpr dataset() : data() {}
	constexpr void operator+=(const dataset<N, DT>& e) { for(auto i = 0; i <= N; i++) data[i] += e.data[i]; }
	constexpr void operator-=(const dataset<N, DT>& e) { for(auto i = 0; i <= N; i++) data[i] -= e.data[i]; }
	constexpr bool operator<(const dataset<N, DT>& e) { for(auto i = 0; i <= N; i++) if(data[i] < e.data[i]) return true; return false; }
	void			add(int i) { data[i] += 1; }
	void			add(int i, int v) { data[i] += v; }
	const DT*		begin() const { return data; }
	void			clear() { for(auto& e : data) e = 0; }
	const DT*		end() const { return data + sizeof(data) / sizeof(data[0]); }
	constexpr int	get(int i) const { return data[i]; }
	constexpr void	set(int i, int v) { data[i] = v; }
};
