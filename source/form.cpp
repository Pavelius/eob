#include "view.h"
#include "io.h"

#define GENDGINF(T) DGINF(T) = {{"Name", DGREQ(name)}, {}};

template<typename T> const char* getnm(const void* object, stringbuilder& sb) {
	return ((T*)object)->name;
}
template<> const char* getnm<enchantmenti>(const void* object, stringbuilder& sb) {
	auto p = (enchantmenti*)object;
	if(!p->name && !p->magic && !p->power)
		return "No special power";
	auto n = p->name;
	if(!n) {
		if(p->power)
			n = p->power.getname();
		else
			n = "magic+%1i";
	}
	sb.add(n, p->magic, p->power.getname());
	sb[0] = sb.upper(sb[0]);
	return sb;
}
template<> const char* getnm<messagei::imagei>(const void* object, stringbuilder& sb) {
	auto p = (messagei::imagei*)object;
	return p->custom;
}
template<> const char* getnm<adventurei>(const void* object, stringbuilder& sb) {
	return ((adventurei*)object)->name;
}
template<> const char* getnm<enchanti>(const void* object, stringbuilder& sb) {
	return ((enchanti*)object)->name;
}
template<> const char* getnm<dialogi>(const void* object, stringbuilder& sb) {
	return ((dialogi*)object)->name;
}
template<> const char* getnm<spelli>(const void* object, stringbuilder& sb) {
	return ((spelli*)object)->name;
}
template<> const char* getnm<point>(const void* object, stringbuilder& sb) {
	auto p = (point*)object;
	if(!(*p))
		sb.add("None");
	else
		sb.add("%1i, %2i", p->x, p->y);
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
	if(e.form.pgetname) {
		if(!e.form.source)
			return "No source";
		return e.form.pgetname(e.form.source->ptr(p->value), sb);
	}
	return "Noname";
}
void* item::getenchantptr(const void* object, int index) {
	auto p = (item*)object;
	auto& ei = p->gete();
	if(!ei.enchantments)
		return 0;
	return ei.enchantments.data + p->subtype;
}
const char* getnoname(const void* object, stringbuilder& sb) {
	return 0;
}
static bool variant_selectable(const void* object, const void* pointer) {
	auto p = (varianti*)pointer;
	return p->form.pgetname != 0;
}
static bool choose_variant(const void* object, const array& source, void* pointer) {
	auto v = (variant*)pointer;
	if(!draw::choose(bsdata<varianti>::source, "Type",
		object, &v->type, sizeof(v->type), {getnm<varianti>, variant_selectable}))
		return false;
	auto& e = bsdata<varianti>::elements[v->type];
	if(!e.form.source)
		return false;
	if(!draw::choose(*e.form.source, e.name,
		object, &v->value, sizeof(v->value), {e.form.pgetname}))
		return false;
	return true;
}
static bool choose_wordmap_point(const void* object, const array& source, void* pointer) {
	auto v = (point*)pointer;
	draw::setimage("worldmap");
	auto r = draw::choosepoint(*v);
	if(!r)
		return false;
	*v = r;
	return true;
}
bool item::choose_enchantment(const void* object, const array& source, void* pointer) {
	auto p = (item*)object;
	auto& ei = p->gete();
	if(!ei.enchantments)
		return false;
	array ars(ei.enchantments.data, sizeof(ei.enchantments.data[0]), ei.enchantments.count);
	auto current = draw::choose(ars, "Enchantment", object, ars.ptr(p->subtype),
		getnm<enchantmenti>, 0, 0, 0);
	if(current)
		p->subtype = ars.indexof(current);
	return false;
}
static bool choose_custom_images(const void* object, const array& source, void* pointer) {
	typedef messagei::imagei T;
	auto v = (T*)pointer;
	array files(sizeof(T));
	for(io::file::find e(bsdata<packi>::elements[PackCustom].url); e; e.next()) {
		if(e.name()[0] == '.')
			continue;
		auto p = (messagei::imagei*)files.add();
		szfnamewe(p->custom, e.name());
		stringbuilder::upper(p->custom);
	}
	auto current_index = files.find(v, 0, sizeof(T));
	void* pc = 0;
	if(current_index != -1)
		pc = files.ptr(current_index);
	pc = draw::choose(files, "Custom images", object, pc,
		getnm<messagei::imagei>, 0, messagei::imagei::preview, 100);
	if(!pc)
		return false;
	memcpy(v, pc, sizeof(T));
	return true;
}
static bool monster_resources(const void* object, const void* pointer) {
	auto p = (resourcei*)pointer;
	return p->pack == PackMonster;
}
static bool dungeon_resources(const void* object, const void* pointer) {
	auto p = (resourcei*)pointer;
	return p->pack==PackDungeon;
}
static bool allow_item_wears(const void* object, const void* pointer) {
	auto p = (weari*)pointer;
	return p->choose_name != 0;
}
static bool unique_items(const void* object, const void* pointer) {
	auto p = (itemi*)pointer;
	return p->feats.is(Unique);
}
static bool allow_item_type_no_natural(const void* object, const void* pointer) {
	auto p = (itemi*)pointer;
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
static bool allow_countable(const void* object, const void* pointer) {
	auto p = (itemi*)pointer;
	return p->feats.is(Countable);
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
static bool visible_parameter(const void* object) {
	auto p = (action*)object;
	return bsdata<actioni>::elements[p->type].params >= 1;
}

static bool visible_class1(const void* object) {
	auto p = (creature*)object;
	auto c = p->getclass();
	return bsdata<classi>::elements[c].classes[0] != 0;
}
static const char* getclass1(const void* object, stringbuilder& sb) {
	auto p = (creature*)object;
	auto c = p->getclass();
	auto n = bsdata<classi>::elements[c].classes[0];
	return bsdata<classi>::elements[n].name;
}
static bool visible_class2(const void* object) {
	auto p = (creature*)object;
	auto c = p->getclass();
	return bsdata<classi>::elements[c].classes[1] != 0;
}
static const char* getclass2(const void* object, stringbuilder& sb) {
	auto p = (creature*)object;
	auto c = p->getclass();
	auto n = bsdata<classi>::elements[c].classes[1];
	return bsdata<classi>::elements[n].name;
}
static bool visible_class3(const void* object) {
	auto p = (creature*)object;
	auto c = p->getclass();
	return bsdata<classi>::elements[c].classes[2] != 0;
}
static bool visible_levels2(const void* object) {
	auto p = (adventurei*)object;
	return p->levels[0].levels != 0;
}
static bool visible_levels3(const void* object) {
	auto p = (adventurei*)object;
	return p->levels[1].levels != 0;
}
static bool visible_levels4(const void* object) {
	auto p = (adventurei*)object;
	return p->levels[2].levels != 0;
}
static bool visible_levels5(const void* object) {
	auto p = (adventurei*)object;
	return p->levels[3].levels != 0;
}
static bool visible_levels6(const void* object) {
	auto p = (adventurei*)object;
	return p->levels[4].levels != 0;
}
static bool visible_levels7(const void* object) {
	auto p = (adventurei*)object;
	return p->levels[5].levels != 0;
}
static bool visible_levels8(const void* object) {
	auto p = (adventurei*)object;
	return p->levels[6].levels != 0;
}
static bool visible_condition_2(const void* object) {
	auto p = (messagei*)object;
	return p->variants[0].operator bool();
}
static bool visible_condition_3(const void* object) {
	auto p = (messagei*)object;
	return p->variants[1].operator bool();
}
static bool visible_condition_4(const void* object) {
	auto p = (messagei*)object;
	return p->variants[2].operator bool();
}
static const char* getclass3(const void* object, stringbuilder& sb) {
	auto p = (creature*)object;
	auto c = p->getclass();
	auto n = bsdata<classi>::elements[c].classes[2];
	return bsdata<classi>::elements[n].name;
}
static bool key_items(const void* object, const void* pointer) {
	auto p = (itemi*)pointer;
	return p->image.ground == 8 && p->image.size == 0;
}
static bool ability_only(const void* object, const void* pointer) {
	auto param = (ability_s)bsdata<abilityi>::source.indexof(pointer);
	return param <= Charisma;
}
static const char* condition_param(const void* object, stringbuilder& sb) {
	auto p = (action*)object;
	auto n = p->param.type;
	if(!n)
		return "Parameter";
	return bsdata<varianti>::elements[n].name;
}
GENDGINF(abilityi)
GENDGINF(alignmenti)
GENDGINF(attacki)
GENDGINF(damagei)
GENDGINF(enchanti)
GENDGINF(enchantmenti)
GENDGINF(feati)
GENDGINF(itemfeati)
GENDGINF(intellegencei)
GENDGINF(genderi)
GENDGINF(resourcei)
GENDGINF(sizei)
GENDGINF(speechi)
GENDGINF(spelli)
GENDGINF(usabilityi)
GENDGINF(weari)
GENDGINF(varianti)
DGINF(variant) = {{"Type", DGREQ(type), {getnm<varianti>}},
{"Value", DGREQ(value)},
{}};
static bool choose_conditions(const void* object, array& source, void* pointer) {
	auto v = (conditiona*)pointer;
	return true;
}
DGINF(point) = {{"x", DGREQ(x)},
{"y", DGREQ(y)},
{}};
DGINF(dice) = {{"Count", DGREQ(c)},
{"Dice", DGREQ(d)},
{"Modifier", DGREQ(b)},
{}};
DGINF(item) = {{"Type", DGREQ(type), {getnm<itemi>, allow_item_type_no_natural}},
{"Power", DGGEN(subtype, enchantmenti, int, 0), {getnm<enchantmenti>, 0, item::choose_enchantment, 0, 0, item::getenchantptr}},
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
{"#chk feats", DGREQ(feats), {getnm<itemfeati>}},
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
DGINF(monsteri) = {{"Name", DGREQ(name)},
{"Resource", DGREQ(rfile), {getnm<resourcei>, monster_resources, 0, resourcei::preview, 130}},
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
{"Power 1", DGREQ(enchantments[0]), {getnm<variant>, 0, choose_variant}},
{"Power 2", DGREQ(enchantments[1]), {getnm<variant>}},
{"#chk feats", DGREQ(feats), {getnm<feati>}},
{"#adc skills", DGREQ(skills), {getnm<abilityi>}},
{}};
DGINF(creature) = {{"Race", DGREQ(race), {getnm<racei>}},
{"Gender", DGREQ(gender), {getnm<genderi>}},
{"Alignment", DGREQ(alignment), {getnm<alignmenti>}},
{"Monster", DGREQ(kind), {getnm<monsteri>}},
{"Class", DGREQ(type), {getnm<classi>}},
{"Level 1", DGREQ(levels[0]), {}, {visible_class1, getclass1}},
{"Level 2", DGREQ(levels[1]), {}, {visible_class2, getclass2}},
{"Level 3", DGREQ(levels[2]), {}, {visible_class3, getclass3}},
{"#div Abilities"},
{"Strenght", DGREQ(ability[0])},
{"Dexterity", DGREQ(ability[1])},
{"Constitut.", DGREQ(ability[2])},
{"Intellect", DGREQ(ability[3])},
{"Wisdow", DGREQ(ability[4])},
{"Charisma", DGREQ(ability[5])},
{"Strenght %", DGREQ(str_exeptional)},
{"Hits", DGREQ(hits)},
{"Hits roll", DGREQ(hits_rolled)},
{"#chk feats", DGREQ(feats), {getnm<feati>}},
{"#chk usabilities", DGREQ(usability), {getnm<usabilityi>}},
//item				wears[LastInvertory + 1];
//char				spells[LastSpellAbility + 1];
//char				prepared[LastSpellAbility + 1];
//spella			known_spells;
//char				avatar;
//unsigned			experience;
//unsigned char		name[2];
//reaction_s		reaction;
{}};
DGINF(classi) = {{"Name", DGREQ(name)},
{"Playable", DGREQ(playable)},
{"Hit dice", DGREQ(hd)},
{"Main", DGREQ(ability), {getnm<abilityi>, ability_only}},
{"#tab minimum", DGREQ(minimum)},
{"#chk feats", DGREQ(feats), {getnm<feati>}},
{"#chk usabilities", DGREQ(usability), {getnm<usabilityi>}},
//adat<class_s, 4>	classes;
//adat<race_s, 12>	races;
{}};
DGINF(racei) = {{"Name", DGREQ(name)},
{"#tab minimum", DGREQ(minimum)},
{"#tab maximum", DGREQ(maximum)},
{"#tab adjustment", DGREQ(adjustment)},
{"#chk feats", DGREQ(feats), {getnm<feati>}},
{"#chk usabilities", DGREQ(usability), {getnm<usabilityi>}},
{"#adc skills", DGREQ(skills), {getnm<abilityi>}},
{}};
DGINF(sitei::headi) = {{"Resource", DGREQ(type), {getnm<resourcei>, dungeon_resources, 0, resourcei::preview, 130}},
{"Monster 1", DGREQ(habbits[0]), {getnm<monsteri>}},
{"Monster 2", DGREQ(habbits[1]), {getnm<monsteri>}},
{"Key 1", DGREQ(keys[0]), {getnm<itemi>, key_items}},
{"Key 2", DGREQ(keys[1]), {getnm<itemi>, key_items}},
{"Special 1", DGREQ(special[0]), {getnm<itemi>, unique_items}},
{"Special 2", DGREQ(special[1]), {getnm<itemi>, unique_items}},
{"Language", DGREQ(language), {getnm<racei>}},
{}};
DGINF(sitei::chancei) = {{"Curse item", DGREQ(curse)},
{}};
DGINF(sitei::crypti) = {{"Boss", DGREQ(boss), {getnm<monsteri>}},
{}};
DGINF(sitei) = {{0, DGREQ(head)},
{0, DGREQ(chance)},
{"Levels", DGREQ(levels)},
{0, DGREQ(crypt)},
{}};
DGINF(adventurei) = {{"Name", DGREQ(name)},
{"Position", DGREQ(position), {getnm<point>, 0, choose_wordmap_point}},
{"#tab Part 1", DGREQ(levels[0])},
{"#tab Part 2", DGREQ(levels[1]), {}, {visible_levels2}},
{"#tab Part 3", DGREQ(levels[2]), {}, {visible_levels3}},
{"#tab Part 4", DGREQ(levels[3]), {}, {visible_levels4}},
{"#tab Part 5", DGREQ(levels[4]), {}, {visible_levels5}},
{"#tab Part 6", DGREQ(levels[5]), {}, {visible_levels6}},
{"#tab Part 7", DGREQ(levels[6]), {}, {visible_levels7}},
{"#tab Part 8", DGREQ(levels[7]), {}, {visible_levels8}},
{}};
DGINF(abilitya) = {{"Strenght", DGREQ(data[0])},
{"Dexterity", DGREQ(data[1])},
{"Constit.", DGREQ(data[2])},
{"Intellect", DGREQ(data[3])},
{"Wisdow", DGREQ(data[4])},
{"Charisma", DGREQ(data[5])},
{}};
DGINF(actioni) = {{"Name", DGREQ(name)},
{}};
DGINF(action) = {{"Action", DGREQ(type), {getnm<actioni>}, {0, getnoname}},
{"Parameter", DGREQ(param), {getnm<variant>, 0, choose_variant}, {visible_parameter, condition_param}},
{}};
DGINF(messagei::imagei) = {{"Resource", DGREQ(custom), {getnm<resourcei>, 0, choose_custom_images, messagei::imagei::preview, 130}},
{"Mirror vertical", DGCHK(flags, ImageMirrorV)},
{"Mirror horizontal", DGCHK(flags, ImageMirrorH)},
{}};
DGINF(messagei) = {{"ID", DGREQ(id)},
{"Type", DGREQ(type), {getnm<speechi>}},
{"Image", DGREQ(overlay), {getnm<messagei::imagei>, 0, choose_custom_images, messagei::imagei::preview, 170}},
{"Cond.1", DGREQ(variants[0]), {getnm<variant>, 0, choose_variant}, {}},
{"Cond.2", DGREQ(variants[1]), {getnm<variant>, 0, choose_variant}, {visible_condition_2}},
{"Cond.3", DGREQ(variants[2]), {getnm<variant>, 0, choose_variant}, {visible_condition_3}},
{"Cond.4", DGREQ(variants[3]), {getnm<variant>, 0, choose_variant}, {visible_condition_4}},
{"Text", DGREQ(text)},
{}};
DGINF(dialogi) = {{"Image", DGREQ(image), {getnm<messagei::imagei>, 0, choose_custom_images}},
{"Text", DGREQ(name)},
{}};