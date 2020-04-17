#include "main.h"

static char result[256];
struct names_object {
	char		type;
	race_s		race;
	gender_s	gender;
	const char*	name[2];
};
static names_object objects[] = {{0, NoRace, NoGender, {"?", "?"}},
{0, Human, Male, {"Am", "��"}},
{0, Human, Male, {"Br", "��"}},
{0, Human, Male, {"Fr", "��"}},
{0, Human, Male, {"J", "��"}},
{0, Human, Male, {"Lo", "��"}},
{0, Human, Male, {"Mi", "��"}},
{0, Human, Male, {"R", "�"}},
{0, Human, Male, {"Vi", "��"}},
{0, Human, Female, {"So", "��"}},
{0, Human, Female, {"Vi", "��"}},
{0, Human, Female, {"Ya", "�"}},
{1, Human, Male, {"ad", "��"}},
{1, Human, Male, {"ed", "��"}},
{1, Human, Male, {"nsk", "���"}},
{1, Human, Male, {"on", "��"}},
{1, Human, Male, {"rt", "��"}},
{1, Human, Male, {"rn", "��"}},
{1, Human, Male, {"ust", "���"}},
{1, Human, Female, {"ka", "��"}},
{1, Human, Female, {"na", "��"}},
{1, Human, Female, {"nia", "��"}},
{0, Dwarf, Male, {"Du", "��"}},
{0, Dwarf, Male, {"Gi", "��"}},
{0, Dwarf, Male, {"Ki", "��"}},
{0, Dwarf, Male, {"Ra", "��"}},
{0, Dwarf, Male, {"Ro", "��"}},
{0, Dwarf, Male, {"Ru", "��"}},
{0, Dwarf, Male, {"To", "��"}},
{1, Dwarf, Male, {"dred", "����"}},
{1, Dwarf, Male, {"grin", "����"}},
{1, Dwarf, Male, {"lly", "���"}},
{1, Dwarf, Male, {"mly", "���"}},
{1, Dwarf, Male, {"rin", "���"}},
{1, Dwarf, Male, {"skin", "����"}},
{0, Dwarf, Female, {"Dur", "���"}},
{0, Dwarf, Female, {"Dir", "���"}},
{0, Dwarf, Female, {"Ag", "��"}},
{0, Dwarf, Female, {"Thor", "���"}},
{1, Dwarf, Female, {"na", "��"}},
{1, Dwarf, Female, {"rila", "����"}},
{1, Dwarf, Female, {"ura", "���"}},
{0, Elf, Male, {"Cala", "����"}},
{0, Elf, Male, {"Hel", "���"}},
{0, Elf, Male, {"Lan", "���"}},
{0, Elf, Male, {"Meir", "����"}},
{0, Elf, Male, {"Sel", "���"}},
{0, Elf, Female, {"Gala", "����"}},
{0, Elf, Female, {"Dar", "���"}},
{0, Elf, Female, {"Amru", "����"}},
{1, Elf, Male, {"dralel", "������"}},
{1, Elf, Male, {"drell", "����"}},
{1, Elf, Male, {"dalel", "�����"}},
{1, Elf, Male, {"dlon", "����"}},
{1, Elf, Male, {"liss", "����"}},
{1, Elf, Female, {"drala", "�����"}},
{1, Elf, Female, {"driell", "������"}},
{1, Elf, Female, {"dlon", "�����"}},
{1, Elf, Female, {"lara", "����"}},
{0, Halfling, Male, {"Bil", "����"}},
{0, Halfling, Male, {"Jan", "��"}},
{0, Halfling, Male, {"Tri", "���"}},
{0, Halfling, Male, {"Su", "��"}},
{0, Halfling, Female, {"Tri", "���"}},
{0, Halfling, Female, {"Su", "��"}},
{1, Halfling, Male, {"xim", "����"}},
{1, Halfling, Male, {"mir", "���"}},
{1, Halfling, Male, {"mak", "���"}},
{1, Halfling, Male, {"no", "��"}},
{1, Halfling, Male, {"bo", "��"}},
{1, Halfling, Female, {"no", "��"}},
{1, Halfling, Female, {"bo", "��"}},
};
static_assert((sizeof(objects) / sizeof(objects[0])) < 128, "Names count invalid");

static bool iswowal(char sym) {
	switch(sym) {
	case 'A':
	case 'a':
	case 'E':
	case 'e':
	case 'I':
	case 'i':
	case 'O':
	case 'o':
	case 'U':
	case 'u':
	case 'Y':
	case 'y':
		return true;
	default:
		return false;
	}
}

int get_autowowal(int rec) {
	if(rec == -1)
		return -1;
	const char* e = zend(objects[rec].name[0]) - 1;
	return iswowal(e[0]) ? 0 : 1;
}

const char* get_name_part(short rec) {
	if(rec == -1)
		return "";
	return objects[rec].name[0];
}

int	gamei::getrandom(int type, race_s race, gender_s gender, int prev_name) {
	int data[sizeof(objects) / sizeof(objects[0]) + 1];
	int* p = data;
	bool wowal = false;
	if(prev_name != -1) {
		const char* p1 = zend(objects[prev_name].name[0]);
		wowal = iswowal(p1[-1]);
	}
	for(unsigned i = 1; i < sizeof(objects) / sizeof(objects[0]); i++) {
		if(objects[i].type != type)
			continue;
		if(race && objects[i].race != race)
			continue;
		if(gender && objects[i].gender != gender)
			continue;
		if(prev_name != -1) {
			if(iswowal(objects[i].name[0][0]) == wowal)
				continue;
		}
		*p++ = i;
	}
	*p = 0;
	auto c = p - data;
	if(!c)
		return 0;
	return data[rand() % c];
}