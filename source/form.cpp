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
DGMETA(weari) = {{0, "Name", DGREQ(name)},
{}};
DGMETA(usabilityi) = {{0, "Name", DGREQ(name)},
{}};
DGMETA(itemi) = {{0, "Name", DGREQ(name)},
{0, "Cost", DGREQ(cost)},
{0, "Equipment", DGREQ(equipment)},
{0, "Ammo", DGREQ(ammo)},
{0, "#chk", DGREQ(usability)},
//usabilitya			;
//cflags<item_feat_s>	feats;
//weaponi				weapon;
//armori				armor;
//aref<enchantmenti>	enchantments;
{}};
INSTDATA(formi) = {{"item", "items", getitemname, dginf<itemi>::meta},
{"item type", "item types", getitemname, dginf<item_s>::meta, bsdata<item_s>::source_ptr},
{"wear", "wears", getxname<weari>, dginf<wear_s>::meta, bsdata<wear_s>::source_ptr},
{"usability", "usabilities", getxname<usabilityi>, dginf<usabilityi>::meta, bsdata<usabilityi>::source_ptr},
//{"Monster", "Monsters", getmonstername},
};
INSTELEM(formi);