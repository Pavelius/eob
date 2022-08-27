#include "main.h"

BSDATA(cityabilityi) = {
	{"Blessing"},
	{"Prosperty"},
	{"Reputation"},
	{"Experience", "%1i exp"},
	{"Gold", "%1i gp"},
};
assert_enum(cityabilityi, Gold)
BSDATAF(cityabilityi)
