#include "main.h"

static struct portraiti {
	gender_s		gender;
	race_s			races[8];
	class_s			classes[8];
} portrait_data[] = {{Male, {Human, Halfling}, {Fighter, Ranger}},
{Male, {Human}, {Mage, Cleric}},
{Male, {Human}, {Mage, Cleric}},
{Male, {Human, Halfling}, {Theif}},
{Male, {Elf}, {Theif, Mage, Fighter, Ranger}},
{Male, {Elf}, {Theif, Fighter, Ranger}},
{Male, {Dwarf}, {Cleric, Fighter}},
{Male, {Human}, {Theif, Mage, Ranger}},
{Male, {Human, Halfling}, {Theif, Fighter}},
{NoGender, {Halfling}, {Theif, Fighter}},
//
{Male, {Elf, HalfElf}, {Mage}},
{Male, {Human}, {Theif}},
{Male, {Human}, {Mage, Cleric}},
{Male, {Elf, Human}, {Fighter, Paladin, Cleric}},
{Male, {HalfElf, Human}, {Mage}},
{Male, {Human}, {Fighter, Paladin}},
{Male, {Elf, HalfElf}, {Mage, Ranger}},
{Male, {Human}, {Fighter, Paladin}},
{Male, {Human}, {Fighter, Paladin, Cleric}},
{Male, {Human}, {Fighter, Paladin}},
//
{Male, {Elf, HalfElf}, {Fighter, Paladin}},
{Male, {Dwarf}, {Fighter}},
{Male, {Elf, HalfElf}, {Fighter, Paladin, Ranger}},
{Male, {Human}, {Fighter}},
{Male, {Dwarf}, {Fighter}},
{Male, {Human}, {Fighter}},
{NoGender, {Human, Elf, HalfElf}, {Mage}},
{NoGender, {Human}, {Mage}},
{NoGender, {Human, Dwarf}, {Cleric}},
{Female, {Elf}, {Mage, Cleric}},
//
{Female, {Human}, {Theif, Ranger, Fighter}},
{Female, {Human}, {Fighter, Cleric}},
{Female, {Human}, {Fighter, Cleric}},
{Female, {Human, Elf}, {Cleric, Mage}},
{Female, {Elf, HalfElf}, {Cleric, Mage, Ranger}},
{Female, {Human, HalfElf}, {Fighter, Paladin, Ranger}},
{Female, {Human}, {Fighter, Cleric, Mage}},
{Female, {Dwarf, Human}, {Cleric, Mage}},
{Female, {Dwarf, Human}, {Cleric, Mage}},
{Female, {Dwarf, Human}, {Fighter, Cleric, Mage}},
//
{Female, {Human}, {Fighter, Cleric, Mage}},
{Female, {Human, HalfElf}, {Cleric, Mage}},
{Female, {Human, HalfElf, Elf}, {Cleric, Mage, Paladin}},
{Female, {Human, HalfElf, Elf}, {Cleric, Mage, Paladin, Theif}},
{Female, {Human}, {Fighter}},
{Male, {Human}, {Fighter, Paladin}},
{Female, {Human, HalfElf, Elf}, {Cleric, Mage}},
{Female, {Human}, {Cleric, Mage}},
{Female, {Human}, {Cleric, Mage, Theif}},
{NoGender, {Halfling}, {Theif, Fighter}},
//
{Male, {Dwarf}, {Fighter}},
{Male, {Dwarf}, {Fighter, Cleric}},
{Male, {Dwarf}, {Fighter, Cleric, Theif}},
{Male, {Halfling}, {Fighter, Cleric, Theif}},
{Female, {Human}, {Fighter, Cleric, Ranger}},
{Male, {Dwarf}, {Cleric}},
{Male, {Human, Elf, HalfElf}, {Mage}},
{Male, {Dwarf}, {Fighter, Cleric}},
{Female, {Human}, {Cleric, Mage}},
};

static bool isportrait(int id, gender_s value) {
	if(!value)
		return true;
	return portrait_data[id].gender == NoGender
		|| portrait_data[id].gender == value;
}

static int select_avatar(int* result, const int* result_maximum, race_s race, gender_s gender, class_s cls) {
	int* p = result;
	for(auto& e : portrait_data) {
		if(gender && e.gender != NoGender && e.gender != gender)
			continue;
		if(race && !zchr(e.races, race))
			continue;
		if(cls && !zchr(e.classes, cls))
			continue;
		if(p<result_maximum)
			*p++ = &e - portrait_data;
	}
	return p - result;
}

int gamei::getavatar(int* result, const int* result_maximum, race_s race, gender_s gender, class_s cls) {
	auto c = select_avatar(result, result_maximum, race, gender, cls);
	if(!c)
		c = select_avatar(result, result_maximum, race, gender, NoClass);
	if(!c)
		c = select_avatar(result, result_maximum, NoRace, gender, NoClass);
	return c;
}

int gamei::getavatar(race_s race, gender_s gender, class_s cls) {
	int result[64];
	auto c = getavatar(result, zendof(result), race, gender, cls);
	return result[rand() % c];
}