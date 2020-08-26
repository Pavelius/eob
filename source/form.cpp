#include "main.h"

template<class T> const char* getnm(const void* object, stringbuilder& sb) {
	return ((T*)object)->name;
}
template<> const char* getnm<itemi>(const void* object, stringbuilder& sb) {
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
DGINF(alignmenti) = {{"Name", DGREQ(name)},
{}};
DGINF(attacki) = {{"Name", DGREQ(name)},
{}};
DGINF(damagei) = {{"Name", DGREQ(name)},
{}};
DGINF(enchantmenti) = {{"Name", DGREQ(name)},
{}};
DGINF(item_feati) = {{"Name", DGREQ(name)},
{}};
DGINF(genderi) = {{"Name", DGREQ(name)},
{}};
DGINF(racei) = {{"Name", DGREQ(name)},
{}};
DGINF(sizei) = {{"Name", DGREQ(name)},
{}};
DGINF(usabilityi) = {{"Name", DGREQ(name)},
{}};
DGINF(weari) = {{"Name", DGREQ(name)},
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
{"Resource", DGREQ(rfile), {getnm<resourcei>}},
{"Race", DGREQ(race), {getnm<racei>}},
{"Gender", DGREQ(gender), {getnm<genderi>}},
{"Size", DGREQ(size), {getnm<sizei>}},
{"Alignment", DGREQ(alignment), {getnm<alignmenti>}},
//intellegence_s		ins;
//cflags<feat_s>		feats;
//char				hd[2];
//char				ac;
//item_s				attacks[4];
//variant				enchantments[2];
//skilla				skills;
//{"Overlay 1", DGREQ(overlays[0])},
{}};