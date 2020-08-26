#include "main.h"

template<class T> const char* getxname(const void* object, stringbuilder& sb) {
	return ((T*)object)->name;
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
static bool allow_countable(const void* object, int param) {
	auto p = bsdata<itemi>::elements + param;
	return p->feats.is(Countable);
}
static const char* getenchantmentname(const void* object, stringbuilder& sb) {
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
static const char* get_wear_choose_name(const void* object, stringbuilder& sb) {
	auto p = (weari*)object;
	return p->choose_name;
}
static const char* getdicename(const void* object, stringbuilder& sb) {
	auto p = (dice*)object;
	p->print(sb);
	return sb;
}
DGMETA(attacki) = {{"Name", DGREQ(name)},
{}};
DGMETA(damagei) = {{"Name", DGREQ(name)},
{}};
DGMETA(enchantmenti) = {{"Name", DGREQ(name)},
{}};
DGMETA(usabilityi) = {{"Name", DGREQ(name)},
{}};
DGMETA(item_feati) = {{"Name", DGREQ(name)},
{}};
DGMETA(weari) = {{"Name", DGREQ(name)},
{}};
DGMETA(dice) = {{"Count", DGREQ(c)},
{"Dice", DGREQ(d)},
{"Modifier", DGREQ(b)},
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
{"Use ammo", DGREQ(ammo), {getitemname, allow_countable}},
{"#tab", DGREQ(weapon)},
{"#tab", DGREQ(armor)},
{"#chk", DGREQ(usability), {getxname<usabilityi>}},
{"#chk", DGREQ(feats), {getxname<item_feati>}},
{}};
DGMETA(combati) = {{"Attack", DGREQ(attack), {getxname<attacki>}},
{"Type", DGREQ(type), {getxname<damagei>}},
{"Speed", DGREQ(speed)},
{"Damage S-M", DGREQ(damage), {getdicename}},
{"Bonus", DGREQ(bonus)},
{"Crit. mul", DGREQ(critical_multiplier)},
{"Crit. rng", DGREQ(critical_range)},
{}};
DGMETA(itemi::weaponi) = {{0, DGINH(combati)},
{"Damage L+", DGREQ(damage_large), {getdicename}},
{}};
DGMETA(itemi::armori) = {{"AC", DGREQ(ac)},
{"Crit. def", DGREQ(critical_deflect)},
{"Reduction", DGREQ(reduction)},
{}};