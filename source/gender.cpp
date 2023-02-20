#include "crt.h"
#include "gender.h"

BSDATA(genderi) = {
	{"Transgender"},
	{"Male"},
	{"Female"},
};
assert_enum(genderi, Female)
BSDATAF(genderi)