#include "main.h"

#define GENDEF(T) DGINF(T) = {{"Name", DGREQ(name)}, {}};

template<class T> const char* getnm(const void* object, stringbuilder& sb) {
	return ((T*)object)->name;
}
template<> const char* getnm<itemi>(const void* object, stringbuilder& sb) {
	auto p = (itemi*)object;
	if(p->feats.is(Natural)) {
		auto& e = bsdata<attacki>::elements[p->weapon.attack];
		if(e.prefix)
			sb.adds(e.prefix);
		sb.adds(p->name);
		p->weapon.damage.print(sb);
	} else
		return p->name;
	return sb;
}
template<> const char* getnm<variant>(const void* object, stringbuilder& sb) {
	auto p = (variant*)object;
	return p->getname();
}
static bool monster_resources(const void* object, int param) {
	auto p = bsdata<resourcei>::elements + param;
	if(!p->path)
		return false;
	return strcmp(p->path, "art/monsters")==0;
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
static bool allow_countable(const void* object, int param) {
	auto p = bsdata<itemi>::elements + param;
	return p->feats.is(Countable);
}
template<> const char* getnm<enchantmenti>(const void* object, stringbuilder& sb) {
	auto p = (enchantmenti*)object;
	if(!p->name && !p->magic && !p->power)
		return "No special power";
	auto pn = p->name;
	if(!pn)
		pn = "magic";
	sb.add("%+1", pn);
	if(p->magic)
		sb.add(" %+1i", p->magic);
	return sb;
}
static void getenchantments(const void* object, array& result) {
	auto p = (item*)object;
	auto& ei = p->gete();
	result.setup(ei.enchantments.data, sizeof(ei.enchantments.data[0]), ei.enchantments.count, ei.enchantments.count);
}
template<> const char* getnm<weari>(const void* object, stringbuilder& sb) {
	auto p = (weari*)object;
	return p->choose_name;
}
template<> const char* getnm<dice>(const void* object, stringbuilder& sb) {
	auto p = (dice*)object;
	p->print(sb);
	return sb;
}
GENDEF(alignmenti)
GENDEF(attacki)
GENDEF(damagei)
GENDEF(enchantmenti)
GENDEF(feati)
GENDEF(item_feati)
GENDEF(intellegencei)
GENDEF(genderi)
GENDEF(racei)
GENDEF(sizei)
GENDEF(usabilityi)
GENDEF(weari)
DGINF(variant) = {{"Value", DGREQ(value)},
{}};
DGINF(dice) = {{"Count", DGREQ(c)},
{"Dice", DGREQ(d)},
{"Modifier", DGREQ(b)},
{}};
DGINF(item) = {{"Type", DGREQ(type), {getnm<itemi>, allow_item_type_no_natural}},
{"Power", DGGEN(subtype, enchantmenti, int, 0), {getnm<enchantmenti>, 0, getenchantments}},
{"Charges", DGREQ(charges)},
{"Identified", DGCHK(flags, 1 << 1)},
{"Cursed", DGCHK(flags, 1 << 2)},
{"Broken", DGCHK(flags, 1 << 3)},
{}};
DGINF(itemi) = {{"Name", DGREQ(name)},
{"Cost", DGREQ(cost)},
{"Wears", DGREQ(equipment), {getnm<weari>, allow_item_wears}},
{"Use ammo", DGREQ(ammo), {getnm<itemi>, allow_countable}},
{"#tab attacks", DGREQ(weapon)},
{"#tab defences", DGREQ(armor)},
{"#chk usabilities", DGREQ(usability), {getnm<usabilityi>}},
{"#chk feats", DGREQ(feats), {getnm<item_feati>}},
{}};
DGINF(combati) = {{"Attack", DGREQ(attack), {getnm<attacki>}},
{"Type", DGREQ(type), {getnm<damagei>}},
{"Speed", DGREQ(speed)},
{"Damage S-M", DGREQ(damage), {getnm<dice>}},
{"Bonus", DGREQ(bonus)},
{"Crit. mul", DGREQ(critical_multiplier)},
{"Crit. rng", DGREQ(critical_range)},
{}};
DGINF(itemi::weaponi) = {{0, DGINH(combati)},
{"Damage L+", DGREQ(damage_large), {getnm<dice>}},
{}};
DGINF(itemi::armori) = {{"AC", DGREQ(ac)},
{"Crit. def", DGREQ(critical_deflect)},
{"Reduction", DGREQ(reduction)},
{}};
DGINF(resourcei) = {{"Name", DGREQ(name)},
{"Path", DGREQ(path)},
{}};
DGINF(monsteri) = {{"Name", DGREQ(name)},
{"Resource", DGREQ(rfile), {getnm<resourcei>, monster_resources}},
{"Race", DGREQ(race), {getnm<racei>}},
{"Gender", DGREQ(gender), {getnm<genderi>}},
{"Size", DGREQ(size), {getnm<sizei>}},
{"Alignment", DGREQ(alignment), {getnm<alignmenti>}},
{"Intellect", DGREQ(ins), {getnm<intellegencei>}},
{"Base AC", DGREQ(ac)},
{"Hit dice", DGREQ(hd[0])},
{"Hit bonus", DGREQ(hd[1])},
{"#div overlays"},
{"Overlay 1", DGREQ(overlays[0])},
{"Overlay 2", DGREQ(overlays[1])},
{"Overlay 3", DGREQ(overlays[2])},
{"Overlay 4", DGREQ(overlays[3])},
{"#div attacks"},
{"Attack 1", DGREQ(attacks[0]), {getnm<itemi>}},
{"Attack 2", DGREQ(attacks[1]), {getnm<itemi>}},
{"Attack 3", DGREQ(attacks[2]), {getnm<itemi>}},
{"Attack 4", DGREQ(attacks[3]), {getnm<itemi>}},
{"Power 1", DGREQ(enchantments[0]), {getnm<variant>}},
{"Power 2", DGREQ(enchantments[1]), {getnm<variant>}},
{"#chk feats", DGREQ(feats), {getnm<feati>}},
//skilla				skills;
{}};