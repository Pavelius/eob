#include "cityability.h"

#pragma once

struct cityi : cityabilitya {
	void				addcity(const cityi& e);
	void				addcity(city_ability_s v, int n) { cityabilitya::addcity(v, n); }
	bool				askmiracle();
	void				clear();
	void				setcity(const cityi& e) { *this = e; }
};
