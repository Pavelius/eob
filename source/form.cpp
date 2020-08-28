#include "main.h"

#define GENDGINF(T) DGINF(T) = {{"Name", DGREQ(name)}, {}};

template<class T> const char* getnm(const void* object, stringbuilder& sb) {
	return ((T*)object)->name;
}
template<> const char* getnm<enchanti>(const void* object, stringbuilder& sb) {
	return ((enchanti*)object)->name;
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
	if(!(*p))
		return "";
	auto& e = bsdata<varianti>::elements[p->type];
	if(e.pgetname) {
		if(!e.source)
			return "No source";
		return e.pgetname(e.source->ptr(p->value), sb);
	}
	return "Noname";
}
static bool variant_selectable(const void* object, int param) {
	auto p = bsdata<varianti>::elements + param;
	return p->pgetname != 0;
}
static bool choose_variant(const void* object, array& source, void* pointer) {
	auto v = (variant*)pointer;
	if(!draw::choose(bsdata<varianti>::source, "Type",
		object, &v->type, sizeof(v->type), {getnm<varianti>, variant_selectable}))
		return false;
	auto& e = bsdata<varianti>::elements[v->type];
	if(!e.source)
		return false;
	if(!draw::choose(*e.source, e.name,
		object, &v->value, sizeof(v->value), {e.pgetname}))
		return false;
	return true;
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
static bool weapon_visible(const void* object) {
	auto p = (itemi*)object;
	return p->equipment == RightHand;
}
static bool armor_visible(const void* object) {
	auto p = (itemi*)object;
	return p->equipment != Backpack;
}
static bool allow_countable(const void* object, int param) {
	auto p = bsdata<itemi>::elements + param;
	return p->feats.is(Countable);
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
GENDGINF(abilityi)
GENDGINF(alignmenti)
GENDGINF(attacki)
GENDGINF(classi)
GENDGINF(damagei)
GENDGINF(enchantmenti)
GENDGINF(feati)
GENDGINF(item_feati)
GENDGINF(intellegencei)
GENDGINF(genderi)
GENDGINF(racei)
GENDGINF(sizei)
GENDGINF(usabilityi)
GENDGINF(weari)
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
{"#tab attacks", DGREQ(weapon), {}, {weapon_visible}},
{"#tab defences", DGREQ(armor), {}, {armor_visible}},
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
{"Resource", DGREQ(rfile), {getnm<resourcei>, monster_resources, 0, 0, resourcei::preview, 130}},
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
{"Power 1", DGREQ(enchantments[0]), {getnm<variant>,0,0,choose_variant}},
{"Power 2", DGREQ(enchantments[1]), {getnm<variant>}},
{"#chk feats", DGREQ(feats), {getnm<feati>}},
{"#adc skills", DGREQ(skills), {getnm<abilityi>}},
{}};
DGINF(creature) = {{"Race", DGREQ(race), {getnm<racei>}},
{"Gender", DGREQ(gender), {getnm<genderi>}},
{"Alignment", DGREQ(alignment), {getnm<alignmenti>}},
{"Monster", DGREQ(kind), {getnm<monsteri>}},
{"Class", DGREQ(type), {getnm<classi>}},
{"Hits", DGREQ(hits)},
{"Hits rolled", DGREQ(hits_rolled)},
{"Level 1", DGREQ(levels[0])},
{"Level 2", DGREQ(levels[1])},
{"Level 3", DGREQ(levels[2])},
{"#chk feats", DGREQ(feats), {getnm<feati>}},
{"#chk usabilities", DGREQ(usability), {getnm<usabilityi>}},
//direction_s			direction;
//short				hits, hits_aid, hits_rolled;
//char				initiative;
//char				ability[LastAbility + 1];
//item				wears[LastInvertory + 1];
//char				spells[LastSpellAbility + 1];
//char				prepared[LastSpellAbility + 1];
//spella			known_spells;
//spella			active_spells;
//char				avatar;
//unsigned			experience;
//unsigned char		name[2];
//char				str_exeptional;
//char				drain_energy, drain_strenght, disease_progress;
//char				pallette;
//short				food;
//reaction_s		reaction;
{}};