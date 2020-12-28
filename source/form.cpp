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
template<> const char* getnm<imagei>(const void* object, stringbuilder& sb) {
	auto p = (imagei*)object;
	return p->custom;
}
template<> const char* getnm<companyi::adventurei>(const void* object, stringbuilder& sb) {
	return ((companyi::adventurei*)object)->name;
}
template<> const char* getnm<enchanti>(const void* object, stringbuilder& sb) {
	return ((enchanti*)object)->name;
}
template<> const char* getnm<moralei>(const void* object, stringbuilder& sb) {
	return ((moralei*)object)->name;
}
template<> const char* getnm<spelli>(const void* object, stringbuilder& sb) {
	return ((spelli*)object)->name;
}
template<> const char* getnm<textable>(const void* object, stringbuilder& sb) {
	return *((textable*)object);
}
template<> const char* getnm<item>(const void* object, stringbuilder& sb) {
	auto p = (item*)object;
	item it = *p;
	it.setidentified(1);
	it.getname(sb);
	return sb;
}
template<> const char* getnm<point>(const void* object, stringbuilder& sb) {
	auto p = (point*)object;
	if(!(*p))
		sb.add("None");
	else
		sb.add("%1i, %2i", p->x, p->y);
	return sb;
}
template<> const char* getnm<companyi::historyi>(const void* object, stringbuilder& sb) {
	auto p = (companyi::historyi*)object;
	auto m = p->gethistorymax();
	if(!m)
		return "None";
	sb.add("Have %1i stages", m);
	return sb;
}
template<> const char* getnm<companyi::fractioni>(const void* object, stringbuilder& sb) {
	return ((companyi::fractioni*)object)->name;
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
template<> const char* getnm<creature>(const void* object, stringbuilder& sb) {
	auto p = (creature*)object;
	return p->getname();
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
static bool choose_variant(void* object, const array& source, void* pointer) {
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
static bool choose_wordmap_point(void* object, const array& source, void* pointer) {
	auto v = (point*)pointer;
	draw::setimage("worldmap");
	auto r = draw::choosepoint(*v);
	if(!r)
		return false;
	*v = r;
	return true;
}
static bool choose_history(void* object, const array& source, void* pointer) {
	auto v = (companyi::historyi*)pointer;
	return draw::edit("History", v, dginf<meta_decoy<decltype(v)>::value>::meta, false);
}
static bool edit_character(void* object, const array& source, void* pointer) {
	auto v = (creature*)pointer;
	return draw::edit("Character", v, dginf<meta_decoy<decltype(v)>::value>::meta, false);
}
bool item::choose_enchantment(void* object, const array& source, void* pointer) {
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
static bool choose_custom_images(void* object, const array& source, void* pointer) {
	typedef imagei T;
	auto v = (T*)pointer;
	array files(sizeof(T));
	for(io::file::find e(bsdata<packi>::elements[PackCustom].url); e; e.next()) {
		if(e.name()[0] == '.')
			continue;
		auto p = (imagei*)files.add();
		char temp[260]; szfnamewe(temp, e.name());
		stringbuilder::lower(temp);
		p->custom = temp;
	}
	auto current_index = files.find(v, 0, sizeof(T));
	void* pc = 0;
	if(current_index != -1)
		pc = files.ptr(current_index);
	pc = draw::choose(files, "Custom images", object, pc,
		getnm<imagei>, 0, imagei::preview, 100);
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
	return p->pack == PackDungeon;
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
	if(p->feats.is(Natural))
		return false;
	auto pi = (item*)object;
	auto slot = pi->getequiped();
	if(slot != Backpack && p->equipment != slot)
		return false;
	return true;
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
static bool visible_fraction(const void* object, const void* pointer) {
	auto p = ((companyi::fractioni*)pointer) - 1;
	return p->operator bool();
}
static bool visible_adventure(const void* object, const void* pointer) {
	auto p = ((companyi::adventurei*)pointer) - 1;
	return p->operator bool();
}
static bool visible_level(const void* object, const void* pointer) {
	auto p = ((sitei*)pointer) - 1;
	return p->levels != 0;
}
static bool visible_condition(const void* object, const void* pointer) {
	auto p = ((variant*)pointer) - 1;
	return p->operator bool();
}
static bool visible_history(const void* object, const void* pointer) {
	auto p = ((char*)pointer) - sizeof(companyi::historyi::history[0]);
	return *p != 0;
}
static bool visible_class2(const void* object) {
	auto p = (creature*)object;
	auto c = p->getclass();
	return bsdata<classi>::elements[c].classes[1] != 0;
}
static bool visible_class3(const void* object) {
	auto p = (creature*)object;
	auto c = p->getclass();
	return bsdata<classi>::elements[c].classes[2] != 0;
}
static const char* getclass2(const void* object, stringbuilder& sb) {
	auto p = (creature*)object;
	auto c = p->getclass();
	auto n = bsdata<classi>::elements[c].classes[1];
	return bsdata<classi>::elements[n].name;
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
void creature::update_race(void* object) {
	auto p = (creature*)object;
	if(!p->isallow(p->type, p->race))
		p->type = chooseclass(false, p->race);
	if(!p->isallow(p->alignment, p->type))
		p->alignment = choosealignment(false, p->type);
	p->random_name();
	p->random_ability();
	p->finish();
}
void creature::update_class(void* object) {
	auto p = (creature*)object;
	if(!p->isallow(p->alignment, p->type))
		p->alignment = choosealignment(false, p->type);
	p->random_name();
	p->random_ability();
	p->finish();
}
static void update_name(void* object) {
	auto p = (creature*)object;
	p->random_name();
}
static bool allow_class(const void* object, const void* pointer) {
	if(!pointer)
		return false;
	auto p = (creature*)object;
	auto v = (class_s)((classi*)pointer - bsdata<classi>::elements);
	return p->isallow(v, p->getrace());
}
static bool allow_alignment(const void* object, const void* pointer) {
	if(!pointer)
		return false;
	auto p = (creature*)object;
	auto v = (alignment_s)((alignmenti*)pointer - bsdata<alignmenti>::elements);
	return p->isallow(v, p->getclass());
}
static bool allow_race(const void* object, const void* pointer) {
	auto p = (creature*)object;
	auto pe = (racei*)pointer;
	return pe->characters != 0;
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
GENDGINF(moralei)
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
DGINF(point) = {{0, DGREQ(x), {getnm<point>, 0, choose_wordmap_point}},
{}};
DGINF(textable) = {{0, DGREQ(id), {getnm<textable>, 0, textable::edit}},
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
DGINF(itemi::weaponi) = {{0, DGINH(combati, attack)},
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
DGINF(creature) = {{"Race", DGREQ(race), {getnm<racei>, allow_race, 0, 0, 0, 0, creature::update_race}},
{"Gender", DGREQ(gender), {getnm<genderi>, 0, 0, 0, 0, 0, update_name}},
{"Class", DGREQ(type), {getnm<classi>, allow_class, 0, 0, 0, 0, creature::update_class}},
{"Alignment", DGREQ(alignment), {getnm<alignmenti>, allow_alignment}},
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
{"#div equipment"},
{"Head", DGREQ(wears[Head]), {getnm<item>}},
{"Neck", DGREQ(wears[Neck]), {getnm<item>}},
{"Body", DGREQ(wears[Body]), {getnm<item>}},
{"Right hand", DGREQ(wears[RightHand]), {getnm<item>}},
{"Left hand", DGREQ(wears[LeftHand]), {getnm<item>}},
{"Right ring", DGREQ(wears[RightRing]), {getnm<item>}},
{"Left ring", DGREQ(wears[LeftRing]), {getnm<item>}},
{"Elbow", DGREQ(wears[Elbow]), {getnm<item>}},
{"Legs", DGREQ(wears[Legs]), {getnm<item>}},
{"Quiver", DGREQ(wears[Quiver]), {getnm<item>}},
{"Belt 1", DGREQ(wears[FirstBelt]), {getnm<item>}},
{"Belt 2", DGREQ(wears[SecondBelt]), {getnm<item>}},
{"Belt 3", DGREQ(wears[LastBelt]), {getnm<item>}},
//char				spells[LastSpellAbility + 1];
//char				prepared[LastSpellAbility + 1];
//spella			known_spells;
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
DGINF(sitei::eventi) = {{"Resource", DGREQ(image)},
{}};
DGINF(sitei::headi) = {{"Resource", DGREQ(type), {getnm<resourcei>, dungeon_resources, 0, resourcei::preview, 130}},
{"Monster 1", DGREQ(habbits[0]), {getnm<monsteri>}},
{"Monster 2", DGREQ(habbits[1]), {getnm<monsteri>}},
{"Key 1", DGREQ(keys[0]), {getnm<itemi>, key_items}},
{"Key 2", DGREQ(keys[1]), {getnm<itemi>, key_items}},
{"Special", DGREQ(special), {getnm<itemi>, unique_items}},
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
//{0, DGREQ(events)},
{}};
DGINF(companyi::historyi) = {{"Stage 1", DGREQ(history[0])},
{"Stage 2", DGREQ(history[1]), {}, {0, 0, visible_history}},
{"Stage 3", DGREQ(history[2]), {}, {0, 0, visible_history}},
{"Stage 4", DGREQ(history[3]), {}, {0, 0, visible_history}},
{"Stage 5", DGREQ(history[4]), {}, {0, 0, visible_history}},
{"Stage 6", DGREQ(history[5]), {}, {0, 0, visible_history}},
{"Stage 7", DGREQ(history[6]), {}, {0, 0, visible_history}},
{"Stage 8", DGREQ(history[7]), {}, {0, 0, visible_history}},
{"Stage 9", DGREQ(history[8]), {}, {0, 0, visible_history}},
{"Stage 10", DGREQ(history[9]), {}, {0, 0, visible_history}},
{"Stage 11", DGREQ(history[10]), {}, {0, 0, visible_history}},
{"Stage 12", DGREQ(history[11]), {}, {0, 0, visible_history}},
{}};
DGINF(companyi::adventurei) = {{"Name", DGREQ(name)},
{"Position", DGREQ(position)},
{"History", DGINH(companyi::historyi, history), {getnm<companyi::historyi>, 0, choose_history}},
{"#tab Part 1", DGREQ(levels[0])},
{"#tab Part 2", DGREQ(levels[1]), {}, {0, 0, visible_level}},
{"#tab Part 3", DGREQ(levels[2]), {}, {0, 0, visible_level}},
{"#tab Part 4", DGREQ(levels[3]), {}, {0, 0, visible_level}},
{"#tab Part 5", DGREQ(levels[4]), {}, {0, 0, visible_level}},
{"#tab Part 6", DGREQ(levels[5]), {}, {0, 0, visible_level}},
{"#tab Part 7", DGREQ(levels[6]), {}, {0, 0, visible_level}},
{"#tab Part 8", DGREQ(levels[7]), {}, {0, 0, visible_level}},
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
DGINF(looti) = {{"Gold", DGREQ(gold)},
{"Experience", DGREQ(experience)},
{"Fame", DGREQ(fame)},
{"Progress", DGREQ(progress)},
{"Luck", DGREQ(luck)},
{}};
DGINF(companyi::fractioni) = {{"Name", DGREQ(name)},
{"Loot", DGINH(looti, gold)},
{}};
DGINF(companyi) = {{"Name", DGREQ(name)},
{"Start", DGREQ(start)},
{"loot", DGREQ(resources)},
{"#div Fractions"},
{"Fraction 1", DGREQ(fractions[0]), {getnm<companyi::fractioni>}},
{"Fraction 2", DGREQ(fractions[1]), {getnm<companyi::fractioni>}, {0, 0, visible_fraction}},
{"Fraction 3", DGREQ(fractions[2]), {getnm<companyi::fractioni>}, {0, 0, visible_fraction}},
{"Fraction 4", DGREQ(fractions[3]), {getnm<companyi::fractioni>}, {0, 0, visible_fraction}},
{"Fraction 5", DGREQ(fractions[4]), {getnm<companyi::fractioni>}, {0, 0, visible_fraction}},
{"Fraction 6", DGREQ(fractions[5]), {getnm<companyi::fractioni>}, {0, 0, visible_fraction}},
{"Fraction 7", DGREQ(fractions[6]), {getnm<companyi::fractioni>}, {0, 0, visible_fraction}},
{"Fraction 8", DGREQ(fractions[7]), {getnm<companyi::fractioni>}, {0, 0, visible_fraction}},
{"#div Adventures"},
{"Adventure 1", DGREQ(adventures[0]), {getnm<companyi::adventurei>}},
{"Adventure 2", DGREQ(adventures[1]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 3", DGREQ(adventures[2]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 4", DGREQ(adventures[3]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 5", DGREQ(adventures[4]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 6", DGREQ(adventures[5]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 7", DGREQ(adventures[6]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 8", DGREQ(adventures[7]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 9", DGREQ(adventures[8]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 10", DGREQ(adventures[9]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 11", DGREQ(adventures[10]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 12", DGREQ(adventures[11]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"Adventure 13", DGREQ(adventures[12]), {getnm<companyi::adventurei>}, {0, 0, visible_adventure}},
{"#div Characters"},
{"Character 1", DGREQ(characters[4]), {getnm<creature>, 0, edit_character}},
{}};
DGINF(imagei) = {{0, DGREQ(custom), {getnm<imagei>, 0, choose_custom_images, imagei::preview, 130}},
{}};
DGINF(messagei::aski) = {{0, DGREQ(text)},
{}};
DGINF(messagei) = {{"Image", DGREQ(overlay)},
{"Condition 1", DGREQ(variants[0]), {getnm<variant>, 0, choose_variant}, {}},
{"Condition 2", DGREQ(variants[1]), {getnm<variant>, 0, choose_variant}, {0, 0, visible_condition}},
{"Condition 3", DGREQ(variants[2]), {getnm<variant>, 0, choose_variant}, {0, 0, visible_condition}},
{"Condition 4", DGREQ(variants[3]), {getnm<variant>, 0, choose_variant}, {0, 0, visible_condition}},
{"Condition 5", DGREQ(variants[4]), {getnm<variant>, 0, choose_variant}, {0, 0, visible_condition}},
{"Condition 6", DGREQ(variants[5]), {getnm<variant>, 0, choose_variant}, {0, 0, visible_condition}},
{"Text", DGREQ(text)},
{"1)", DGREQ(actions[0])},
{"2)", DGREQ(actions[1])},
{"3)", DGREQ(actions[2])},
{"4)", DGREQ(actions[3])},
{"5)", DGREQ(actions[4])},
{"6)", DGREQ(actions[5])},
{"7)", DGREQ(actions[6])},
{"8)", DGREQ(actions[7])},
{}};
DGINF(buildingi) = {{"Name", DGREQ(name)},
{"Image", DGREQ(image)},
{"#chk Actions", DGREQ(actions), {getnm<actioni>}},
{}};