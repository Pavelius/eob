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
	return p->choose_name != 0;
}
static bool allow_item_type_no_natural(const void* object, int param) {
	if(!param)
		return false;
	auto p = bsdata<itemi>::elements + param;
	return !p->feats.is(Natural);
}
static const char* getenchantmentname(const void* object, stringbuilder& sb) {
	auto p = (enchantmenti*)object;
	if(!p->name) {
		if(!p->magic && !p->power)
			sb.add("No special power");
		else
			sb.add("magic+%1i", p->magic);
		return sb;
	}
	if(p->magic) {
		sb.add("%1+%2i", p->name, p->magic);
		return sb;
	}
	return p->name;
}
static void getenchantments(const void* object, array& result) {
	auto p = (item*)object;
	auto& ei = p->gete();
	result.setup(ei.enchantments.data, sizeof(ei.enchantments.data[0]), ei.enchantments.count, ei.enchantments.count);
}
static const char* get_wear_choose_name(const void* object, stringbuilder& sb) {
	auto p = (weari*)object;
	return p->choose_name;
}
DGMETA(weari) = {{"Name", DGREQ(name)},
{}};
DGMETA(usabilityi) = {{"Name", DGREQ(name)},
{}};
DGMETA(enchantmenti) = {{"Name", DGREQ(name)},
{}};
DGMETA(item) = {{"Type", DGREQ(type), {getitemname, allow_item_type_no_natural}},
{"Power", DGCHK(subtype, enchantmenti, int, 0, 0), {getenchantmentname, 0, getenchantments}},
{"Charges", DGREQ(charges)},
{"Identified", DGCHK(flags, int, int, 0, 1 << 1)},
{"Cursed", DGCHK(flags, int, int, 0, 1 << 2)},
{"Broken", DGCHK(flags, int, int, 0, 1 << 3)},
{}};
DGMETA(itemi) = {{"Name", DGREQ(name)},
{"Cost", DGREQ(cost)},
{"Wears", DGREQ(equipment), {get_wear_choose_name, allow_item_wears}},
{"Use ammo", DGREQ(ammo), {getitemname}},
{"#chk", DGREQ(usability)},
//cflags<item_feat_s>	feats;
//weaponi				weapon;
//armori				armor;
//aref<enchantmenti>	enchantments;
{}};