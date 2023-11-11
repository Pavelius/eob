#include "cityability.h"

#pragma once

struct cityi : cityabilitya {
	void	add(const cityabilitya& e);
	void	add(city_ability_s v, int n) { cityabilitya::add(v, n); }
	bool	askmiracle();
	void	clear();
	void	setcity(const cityi& e) { *this = e; }
};
