#include "main.h"
#include "draw.h"
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
	auto& ei = bsdata<resourcei>::elements[p->res];
	sb.add(ei.name);
	switch(ei.pack) {
	case PackOuttake:
	case Pack160x96:
	case PackInterface:
		sb.adds("%1i", p->frame);
		break;
	}
	return sb;
}
template<> const char* getnm<adventurei>(const void* object, stringbuilder& sb) {
	return ((adventurei*)object)->getname();
}
template<> const char* getnm<casei>(const void* object, stringbuilder& sb) {
	return ((casei*)object)->name;
}
template<> const char* getnm<companyi>(const void* object, stringbuilder& sb) {
	return ((companyi*)object)->getname();
}
template<> const char* getnm<actionseti>(const void* object, stringbuilder& sb) {
	return ((actionseti*)object)->name;
}
template<> const char* getnm<enchanti>(const void* object, stringbuilder& sb) {
	return ((enchanti*)object)->name;
}
template<> const char* getnm<eventi>(const void* object, stringbuilder& sb) {
	return ((eventi*)object)->id.getname();
}
template<> const char* getnm<moralei>(const void* object, stringbuilder& sb) {
	return ((moralei*)object)->name;
}
template<> const char* getnm<spelli>(const void* object, stringbuilder& sb) {
	return ((spelli*)object)->name;
}
template<> const char* getnm<textable>(const void* object, stringbuilder& sb) {
	return ((textable*)object)->getname();
}
template<> const char* getnm<settlementi>(const void* object, stringbuilder& sb) {
	return ((settlementi*)object)->getname();
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
template<> const char* getnm<historyi>(const void* object, stringbuilder& sb) {
	auto p = (historyi*)object;
	auto m = p->gethistorymax();
	if(!m)
		return "None";
	sb.add("Have %1i stages", m);
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
		return "None";
	auto& e = bsdata<varianti>::elements[p->type];
	if(e.pgetname) {
		if(!e.source)
			return "No source";
		return e.pgetname(e.source->ptr(p->value), sb);
	}
	return "None";
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
static bool variant_action_selectable(const void* object, const void* pointer) {
	auto p = (varianti*)pointer;
	return p->allowed.is(Action);
}
static bool choose_action_variant(void* object, const array& source, void* pointer) {
	auto v = (variant*)pointer;
	if(!draw::choose(bsdata<varianti>::source, "Type",
		object, &v->type, sizeof(v->type), {getnm<varianti>, variant_action_selectable}))
		return false;
	auto& e = bsdata<varianti>::elements[v->type];
	if(!e.source)
		return false;
	if(!draw::choose(*e.source, e.name,
		object, &v->value, sizeof(v->value), {e.pgetname}))
		return false;
	return true;
}
static bool variant_selectable(const void* object, const void* pointer) {
	auto p = (varianti*)pointer;
	return p->pgetname != 0;
}
static bool choose_variant(void* object, const array& source, void* pointer) {
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
static bool choose_wordmap_point(void* object, const array& source, void* pointer) {
	auto v = (point*)pointer;
	auto r = draw::choosepoint(*v);
	if(!r)
		return false;
	*v = r;
	return true;
}
static bool choose_history(void* object, const array& source, void* pointer) {
	auto v = (historyi*)pointer;
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
static bool monster_resources(const void* object, const void* pointer) {
	auto p = (resourcei*)pointer;
	return p->pack == PackMonster;
}
static bool dungeon_resources(const void* object, const void* pointer) {
	auto p = (resourcei*)pointer;
	return p->pack == PackDungeon;
}
static bool scene_resources(const void* object, const void* pointer) {
	auto p = (resourcei*)pointer;
	return p->pack == Pack160x96;
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
static bool visible_level(const void* object, const void* pointer) {
	auto p = ((sitei*)pointer) - 1;
	return p->levels != 0;
}
static bool visible_specal(const void* object, const void* pointer) {
	auto p = ((item*)pointer) - 1;
	return p->operator bool();
}
static bool visible_condition(const void* object, const void* pointer) {
	auto p = ((variant*)pointer) - 1;
	return p->operator bool();
}
static bool visible_history(const void* object, const void* pointer) {
	auto p = ((char*)pointer) - sizeof(historyi::history[0]);
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
GENDGINF(actioni)
GENDGINF(alignmenti)
GENDGINF(attacki)
GENDGINF(buildingi)
GENDGINF(casei)
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
DGINF(point) = {{"x", DGREQ(x)},
	{"y", DGREQ(y)},
	{}};
DGINF(textable) = {{0, DGREQ(name), {getnm<textable>, 0, textable::edit}},
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
	{}};
DGINF(classi) = {{"Name", DGREQ(name)},
	{"Playable", DGREQ(playable)},
	{"Hit dice", DGREQ(hd)},
	{"Main", DGREQ(ability), {getnm<abilityi>, ability_only}},
	{"#tab minimum", DGREQ(minimum)},
	{"#chk feats", DGREQ(feats), {getnm<feati>}},
	{"#chk usabilities", DGREQ(usability), {getnm<usabilityi>}},
	{}};
DGINF(racei) = {{"Name", DGREQ(name)},
	{"#tab minimum", DGREQ(minimum)},
	{"#tab maximum", DGREQ(maximum)},
	{"#tab adjustment", DGREQ(adjustment)},
	{"#chk feats", DGREQ(feats), {getnm<feati>}},
	{"#chk usabilities", DGREQ(usability), {getnm<usabilityi>}},
	{"#adc skills", DGREQ(skills), {getnm<abilityi>}},
	{}};
DGINF(sitei) = {{"Resource", DGREQ(head.type), {getnm<resourcei>, dungeon_resources, 0, resourcei::preview, 130}},
	{"Monster 1", DGREQ(head.habbits[0]), {getnm<monsteri>}},
	{"Monster 2", DGREQ(head.habbits[1]), {getnm<monsteri>}},
	{"Key 1", DGREQ(head.keys[0]), {getnm<itemi>, key_items}},
	{"Key 2", DGREQ(head.keys[1]), {getnm<itemi>, key_items}},
	{"Special", DGREQ(head.wands), {getnm<itemi>, unique_items}},
	{"Language", DGREQ(head.language), {getnm<racei>}},
	{"Curse item", DGREQ(chance.curse)},
	{"Levels", DGREQ(levels)},
	{"Boss", DGREQ(crypt.boss), {getnm<monsteri>}},
	{}};
DGINF(historyi) = {{"Stage 1", DGREQ(history[0]), {getnm<textable>, 0, textable::edit}},
	{"Stage 2", DGREQ(history[1]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 3", DGREQ(history[2]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 4", DGREQ(history[3]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 5", DGREQ(history[4]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 6", DGREQ(history[5]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 7", DGREQ(history[6]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 8", DGREQ(history[7]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 9", DGREQ(history[8]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 10", DGREQ(history[9]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 11", DGREQ(history[10]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{"Stage 12", DGREQ(history[11]), {getnm<textable>, 0, textable::edit}, {0, 0, visible_history}},
	{}};
DGINF(adventurei) = {{"Name", DGREQ(name), {getnm<textable>, 0, textable::edit}},
	{"Settlement", DGENM(settlement, settlementi), {getnm<settlementi>}},
	{"Rumor", DGREQ(rumor_activate), {getnm<textable>, 0, textable::edit}},
	{"Briefing", DGREQ(message_before), {getnm<textable>, 0, textable::editrich}},
	{"Accepted", DGREQ(message_agree), {getnm<textable>, 0, textable::editrich}},
	{"Entering", DGREQ(message_entering), {getnm<textable>, 0, textable::editrich}},
	{"Done", DGREQ(message_done), {getnm<textable>, 0, textable::editrich}},
	{"Fail", DGREQ(message_fail), {getnm<textable>, 0, textable::editrich}},
	{"Position", DGREQ(position), {getnm<point>, 0, choose_wordmap_point}},
	{"History", DGINH(historyi, history), {getnm<historyi>, 0, choose_history}},
	{"#tab Part 1", DGREQ(levels[0])},
	{"#tab Part 2", DGREQ(levels[1]), {}, {0, 0, visible_level}},
	{"#tab Part 3", DGREQ(levels[2]), {}, {0, 0, visible_level}},
	{"#tab Part 4", DGREQ(levels[3]), {}, {0, 0, visible_level}},
	{"#tab Part 5", DGREQ(levels[4]), {}, {0, 0, visible_level}},
	{"#tab Part 6", DGREQ(levels[5]), {}, {0, 0, visible_level}},
	{"#tab Part 7", DGREQ(levels[6]), {}, {0, 0, visible_level}},
	{"#tab Part 8", DGREQ(levels[7]), {}, {0, 0, visible_level}},
	{}};
DGINF(abilitya) = {
	{"Strenght", DGREQ(data[0])},
	{"Dexterity", DGREQ(data[1])},
	{"Constit.", DGREQ(data[2])},
	{"Intellect", DGREQ(data[3])},
	{"Wisdow", DGREQ(data[4])},
	{"Charisma", DGREQ(data[5])},
	{}};
DGINF(companyi) = {
	{"Name", DGREQ(name), {getnm<textable>, 0, textable::edit}},
	{"Introducion", DGREQ(intro), {getnm<textable>, 0, textable::editrich}},
	{"Start", DGENM(start, settlementi), {getnm<settlementi>}},
	{"Start gold", DGREQ(start_gold)},
	{"Pixels/day", DGREQ(pixels_per_day)},
	{"#div Modules"},
	{"Adventures", DGLST(adventurei), {getnm<adventurei>}},
	{"Characters", DGLST(creature), {getnm<creature>}},
	{"Events", DGLST(eventi), {getnm<eventi>}},
	{"Settlements", DGLST(settlementi), {getnm<settlementi>}},
	{}};
DGINF(imagei) = {
	{0, DGREQ(res), {getnm<resourcei>, 0, 0, resourcei::preview, 130}},
	{}};
DGINF(settlementi) = {
	{"Name", DGREQ(name), {getnm<textable>, 0, textable::edit}},
	{"Image", DGREQ(image), {getnm<imagei>, scene_resources, imagei::choose}},
	{"Position", DGREQ(position), {getnm<point>, 0, choose_wordmap_point}},
	{"Description", DGREQ(description), {getnm<textable>, 0, textable::edit}},
	{"Prosperty", DGREQ(prosperty)},
	{"#chk Buildings", DGREQ(buildings), {getnm<buildingi>}},
	{}};
DGINF(richtexti) = {
	{"", DGREQ(images[0]), {getnm<imagei>, scene_resources, imagei::choose}},
	{"Enter text 1", DGREQ(data[0])},
	{"", DGREQ(images[1]), {getnm<imagei>, scene_resources, imagei::choose}},
	{"Enter text 2", DGREQ(data[1])},
	{"", DGREQ(images[2]), {getnm<imagei>, scene_resources, imagei::choose}},
	{"Enter text 3", DGREQ(data[2])},
	{"#div"},
	{"", DGREQ(images[3]), {getnm<imagei>, scene_resources, imagei::choose}},
	{"Enter text 4", DGREQ(data[3])},
	{"", DGREQ(images[4]), {getnm<imagei>, scene_resources, imagei::choose}},
	{"Enter text 5", DGREQ(data[4])},
	{"", DGREQ(images[5]), {getnm<imagei>, scene_resources, imagei::choose}},
	{"Enter text 6", DGREQ(data[5])},
	{}};
DGINF(eventi) = {
	{"ID", DGREQ(id), {getnm<textable>, 0, textable::edit}},
	{"Text", DGREQ(name), {getnm<textable>, 0, textable::editrich}},
	{"1)", DGREQ(ask[0]), {getnm<textable>, 0, textable::edit}},
	{"2)", DGREQ(ask[1]), {getnm<textable>, 0, textable::edit}},
	{"Starting", DGCHK(flags, 1 << eventi::Start)},
	{"Wilderness event", DGCHK(flags, 1 << eventi::Wilderness)},
	{"#tab Result 1", DGREQ(results[0])},
	{"#tab Result 2", DGREQ(results[1])},
	{"#tab Result 3", DGREQ(results[2])},
	{"#tab Result 4", DGREQ(results[3])},
	{}};
DGINF(resultable) = {
	{"Text", DGREQ(name), {getnm<textable>, 0, textable::editrich}},
	{"Action 1", DGREQ(actions[0]), {getnm<variant>, 0, choose_action_variant}},
	{"Action 2", DGREQ(actions[1]), {getnm<variant>, 0, choose_action_variant}, {0, 0, visible_condition}},
	{"Action 3", DGREQ(actions[2]), {getnm<variant>, 0, choose_action_variant}, {0, 0, visible_condition}},
	{"Action 4", DGREQ(actions[3]), {getnm<variant>, 0, choose_action_variant}, {0, 0, visible_condition}},
	{"Action 5", DGREQ(actions[4]), {getnm<variant>, 0, choose_action_variant}, {0, 0, visible_condition}},
	{"Action 6", DGREQ(actions[5]), {getnm<variant>, 0, choose_action_variant}, {0, 0, visible_condition}},
	{}};
DGINF(actionseti) = {
	{"Action", DGREQ(action), {getnm<actioni>}},
	{"Count 1", DGREQ(count1)},
	{"Count 2", DGREQ(count2)},
	{}};