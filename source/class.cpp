#include "main.h"

class_info bsmeta<class_info>::elements[] = {{"Monster", 0, 8, Strenght},
{"Cleric", 1, 8, Wisdow, {Cleric}, {UseScrolls, UseDivine, UseLeatherArmor, UseShield, UseMetalArmor}, {0, 0, 0, 0, 9, 0}, {Human, Dwarf, Elf, HalfElf, Halfling}},
{"Fighter", 1, 10, Strenght, {Fighter}, {UseMartialWeapon, UseLeatherArmor, UseShield, UseMetalArmor, UseTheifWeapon}, {9, 0, 0, 0, 0, 0}, {Human, Dwarf, Elf, HalfElf, Halfling}},
{"Mage", 1, 4, Intellegence, {Mage}, {UseScrolls, UseArcane}, {0, 0, 0, 9, 0, 0}, {Human, Elf, HalfElf}},
{"Paladin", 1, 10, Strenght, {Paladin}, {UseMartialWeapon, UseLeatherArmor, UseShield, UseMetalArmor, UseTheifWeapon}, {12, 0, 9, 0, 13, 17}, {Human}},
{"Ranger", 1, 10, Strenght, {Ranger}, {UseMartialWeapon, UseLeatherArmor, UseShield, UseMetalArmor, UseTheifWeapon}, {13, 13, 14, 0, 14, 0}, {Human, Elf, HalfElf}},
{"Theif", 1, 6, Dexterity, {Theif}, {UseTheif, UseLeatherArmor, UseTheifWeapon}, {0, 9, 0, 0, 0, 0}, {Human, Dwarf, Elf, HalfElf, Halfling}},
{"Fighter/Cleric", 1, 0, Strenght, {Fighter, Cleric}, {UseScrolls, UseDivine, UseMartialWeapon, UseLeatherArmor, UseShield, UseMetalArmor, UseTheifWeapon}, {9, 0, 0, 0, 9, 0}, {Dwarf, HalfElf}},
{"Fighter/Mage", 1, 0, Strenght, {Fighter, Mage}, {UseScrolls, UseArcane, UseMartialWeapon, UseLeatherArmor, UseShield, UseTheifWeapon}, {9, 0, 0, 9, 0, 0}, {Elf, HalfElf}},
{"Fighter/Theif", 1, 0, Strenght, {Fighter, Theif}, {UseTheif, UseMartialWeapon, UseLeatherArmor, UseShield, UseMetalArmor, UseTheifWeapon}, {9, 9, 0, 0, 0, 0}, {Dwarf, Elf, HalfElf, Halfling}},
{"Fighter/Mage/Theif", 1, 0, Strenght, {Fighter, Mage, Theif}, {UseTheif, UseScrolls, UseArcane, UseMartialWeapon, UseLeatherArmor, UseShield, UseMetalArmor, UseTheifWeapon}, {9, 9, 0, 0, 9, 0}, {Elf, HalfElf}},
{"Cleric/Theif", 1, 0, Wisdow, {Cleric, Theif}, {UseTheif, UseScrolls, UseDivine, UseLeatherArmor, UseShield, UseMetalArmor, UseTheifWeapon}, {0, 9, 0, 0, 9, 0}, {HalfElf}},
{"Mage/Theif", 1, 0, Intellegence, {Mage, Theif}, {UseTheif, UseScrolls, UseArcane, UseLeatherArmor, UseTheifWeapon}, {0, 9, 0, 9, 0, 0}, {Elf, HalfElf}},
};
assert_enum(class, MageTheif);