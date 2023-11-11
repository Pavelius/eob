#pragma once

enum variant_s : unsigned char {
	NoVariant,
	Ability, Action, Adventure, Alignment, Case, Cell, Class,
	Cleveress, Condition, Creature, Damage, Enchant, Feat, Gender,
	Item, Morale, Race, Rarity, Reaction, Resource, Spell,
};
enum ability_s : unsigned char;
enum alignment_s : unsigned char;
enum cell_s : unsigned char;
enum class_s : unsigned char;
enum condition_s : unsigned char;
enum damage_s : unsigned char;
enum enchant_s : unsigned char;
enum feat_s : unsigned char;
enum gender_s : unsigned char;
enum intellegence_s : unsigned char;
enum item_s : unsigned char;
enum morale_s : unsigned char;
enum race_s : unsigned char;
enum rarity_s : unsigned char;
enum reaction_s : unsigned char;
enum resource_s : unsigned char;
enum spell_s : unsigned char;

class creature;

struct variant {
	variant_s		type;
	unsigned char	value;
	constexpr variant(const variant_s t, unsigned char v) : type(t), value(v) {}
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(const ability_s v) : type(Ability), value(v) {}
	constexpr variant(const alignment_s v) : type(Alignment), value(v) {}
	constexpr variant(const cell_s v) : type(Cell), value(v) {}
	constexpr variant(const class_s v) : type(Class), value(v) {}
	constexpr variant(const condition_s v) : type(Condition), value(v) {}
	constexpr variant(const damage_s v) : type(Damage), value(v) {}
	constexpr variant(const enchant_s v) : type(Enchant), value(v) {}
	constexpr variant(const intellegence_s v) : type(Cleveress), value(v) {}
	constexpr variant(const item_s v) : type(Item), value(v) {}
	constexpr variant(const race_s v) : type(Race), value(v) {}
	constexpr variant(const rarity_s v) : type(Rarity), value(v) {}
	constexpr variant(const reaction_s v) : type(Reaction), value(v) {}
	constexpr variant(const resource_s v) : type(Resource), value(v) {}
	constexpr variant(const spell_s v) : type(Spell), value(v) {}
	variant(const void* v);
	constexpr explicit operator bool() const { return type != NoVariant; }
	constexpr bool operator==(const variant& e) const { return type == e.type && value == e.value; }
	void			clear() { type = NoVariant; value = 0; }
	creature*		getcreature() const;
	const char*		getname() const;
};
typedef variant conditiona[6];