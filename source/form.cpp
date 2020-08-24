#include "main.h"

template<class T> const char* getxname(const void* object, stringbuilder& sb) {
	return ((T*)object)->name;
}
static const char* getmonstername(const void* object, stringbuilder& sb) {
	return ((monsteri*)object)->name;
}
static const char* getitemname(const void* object, stringbuilder& sb) {
	auto p = (itemi*)object;
	if(p->feats.is(Natural)) {
		sb.add(p->name);
		sb.add(" ");
		p->weapon.damage.print(sb);
	} else
		return p->name;
	return sb;
}
INSTDATA(formi) = {{"Item", "Items", getitemname, dginf<itemi>::meta},
{"Item type", "Item types", getitemname, dginf<item_s>::meta, bsdata<item_s>::source_ptr},
{"Wear", "wears", getxname<weari>, dginf<wear_s>::meta, bsdata<wear_s>::source_ptr},
//{"Monster", "Monsters", getmonstername},
};