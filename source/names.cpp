#include "main.h"

static char result[256];
struct names_object {
	char		type;
	race_s		race;
	gender_s	gender;
	const char*	name[2];
};
static names_object objects[] = {{0, NoRace, NoGender, {"?", "?"}},
{0, Human, Male, {"Am", "Ам"}},
{0, Human, Male, {"Br", "Бр"}},
{0, Human, Male, {"Fr", "Фр"}},
{0, Human, Male, {"J", "Дж"}},
{0, Human, Male, {"Lo", "Ло"}},
{0, Human, Male, {"Mi", "Ми"}},
{0, Human, Male, {"R", "Р"}},
{0, Human, Male, {"Vi", "Ви"}},
{0, Human, Female, {"So", "Со"}},
{0, Human, Female, {"Vi", "Ви"}},
{0, Human, Female, {"Ya", "Я"}},
{1, Human, Male, {"ad", "ад"}},
{1, Human, Male, {"ed", "ед"}},
{1, Human, Male, {"nsk", "нск"}},
{1, Human, Male, {"on", "он"}},
{1, Human, Male, {"rt", "рт"}},
{1, Human, Male, {"rn", "рн"}},
{1, Human, Male, {"ust", "аст"}},
{1, Human, Female, {"ka", "ка"}},
{1, Human, Female, {"na", "на"}},
{1, Human, Female, {"nia", "ня"}},
{0, Dwarf, Male, {"Du", "Ду"}},
{0, Dwarf, Male, {"Gi", "Ги"}},
{0, Dwarf, Male, {"Ki", "Ки"}},
{0, Dwarf, Male, {"Ra", "Ра"}},
{0, Dwarf, Male, {"Ro", "Ро"}},
{0, Dwarf, Male, {"Ru", "Ру"}},
{0, Dwarf, Male, {"To", "То"}},
{1, Dwarf, Male, {"dred", "дред"}},
{1, Dwarf, Male, {"grin", "грин"}},
{1, Dwarf, Male, {"lly", "лли"}},
{1, Dwarf, Male, {"mly", "мли"}},
{1, Dwarf, Male, {"rin", "рин"}},
{1, Dwarf, Male, {"skin", "скин"}},
{0, Dwarf, Female, {"Dur", "Дур"}},
{0, Dwarf, Female, {"Dir", "Дир"}},
{0, Dwarf, Female, {"Ag", "Аг"}},
{0, Dwarf, Female, {"Thor", "Тор"}},
{1, Dwarf, Female, {"na", "на"}},
{1, Dwarf, Female, {"rila", "рила"}},
{1, Dwarf, Female, {"ura", "ура"}},
{0, Elf, Male, {"Cala", "Кала"}},
{0, Elf, Male, {"Hel", "Хел"}},
{0, Elf, Male, {"Lan", "Лан"}},
{0, Elf, Male, {"Meir", "Мэир"}},
{0, Elf, Male, {"Sel", "Сэл"}},
{0, Elf, Female, {"Gala", "Гала"}},
{0, Elf, Female, {"Dar", "Дар"}},
{0, Elf, Female, {"Amru", "Арму"}},
{1, Elf, Male, {"dralel", "драрел"}},
{1, Elf, Male, {"drell", "дрел"}},
{1, Elf, Male, {"dalel", "далел"}},
{1, Elf, Male, {"dlon", "длон"}},
{1, Elf, Male, {"liss", "лисс"}},
{1, Elf, Female, {"drala", "драла"}},
{1, Elf, Female, {"driell", "дриель"}},
{1, Elf, Female, {"dlon", "длона"}},
{1, Elf, Female, {"lara", "лара"}},
{0, Halfling, Male, {"Bil", "Биль"}},
{0, Halfling, Male, {"Jan", "Ян"}},
{0, Halfling, Male, {"Tri", "Три"}},
{0, Halfling, Male, {"Su", "Су"}},
{0, Halfling, Female, {"Tri", "Три"}},
{0, Halfling, Female, {"Su", "Су"}},
{1, Halfling, Male, {"xim", "ксим"}},
{1, Halfling, Male, {"mir", "мир"}},
{1, Halfling, Male, {"mak", "мак"}},
{1, Halfling, Male, {"no", "но"}},
{1, Halfling, Male, {"bo", "бо"}},
{1, Halfling, Female, {"no", "но"}},
{1, Halfling, Female, {"bo", "бо"}},
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