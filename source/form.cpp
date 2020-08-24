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
static bool allow_item_wears(const void* object, int param) {
	auto p = bsdata<weari>::elements + param;
	return p->choose_name!=0;
}
static const char* get_wear_choose_name(const void* object, stringbuilder& sb) {
	auto p = (weari*)object;
	return p->choose_name;
}
DGMETA(weari) = {{0, "Name", DGREQ(name)},
{}};
DGMETA(usabilityi) = {{0, "Name", DGREQ(name)},
{}};
DGMETA(itemi) = {{0, "Name", DGREQ(name)},
{0, "Cost", DGREQ(cost)},
{0, "Wears", DGREQ(equipment), {get_wear_choose_name, allow_item_wears}},
{0, "Use ammo", DGREQ(ammo), {getitemname}},
{0, "#chk", DGREQ(usability)},
//cflags<item_feat_s>	feats;
//weaponi				weapon;
//armori				armor;
//aref<enchantmenti>	enchantments;
{}};
INSTDATA(formi) = {{"item type", "item types", getitemname, dginf<item_s>::meta, bsdata<item_s>::source_ptr},
{"wear", "wears", getxname<weari>, dginf<wear_s>::meta, bsdata<wear_s>::source_ptr},
{"usability", "usabilities", getxname<usabilityi>, dginf<usabilityi>::meta, bsdata<usabilityi>::source_ptr},
//{"Monster", "Monsters", getmonstername},
};
INSTELEM(formi);