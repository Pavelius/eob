#include "crt.h"
#include "size.h"

BSDATA(sizei) = {
	{"Tiny"},
	{"Small"},
	{"Medium"},
	{"Tall"},
	{"Large"},
};
assert_enum(sizei, Large)
BSDATAF(sizei)
