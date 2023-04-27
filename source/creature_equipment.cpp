#include "main.h"

struct equipmenti {
	int			level;
	conditiona	conditions;
	item		object;
	wear_s		slot;
};

BSDATA(equipmenti) = {
	{0, {Dwarf, Fighter}, AxeBattle, RightHand},
	{0, {Dwarf, Fighter}, Shield, LeftHand},
	{0, {Halfling, Fighter}, SwordShort, RightHand},
	{0, {Fighter}, SwordLong, RightHand},
	{0, {Fighter}, ArmorStuddedLeather, Body},
	{0, {Ranger}, SwordLong, RightHand},
	{0, {Ranger}, SwordShort, LeftHand},
	{0, {Cleric}, ArmorLeather, Body},
	{0, {Cleric}, Mace, RightHand},
	{0, {Theif}, ArmorLeather, Body},
	{0, {Mage}, Staff, RightHand},
	{0, {Mage}, Robe, Body},
	{0, {}, Dagger, RightHand},
	{0, {}, ArmorLeather, Body},
	{1, {}, RationIron},
	{1, {Ranger}, Arrow},
	{1, {Fighter}, Ration},
	{1, {Theif}, TheifTools},
	{1, {Cleric}, {BluePotion, CureLightWounds}},
};
BSDATAF(equipmenti);

void creature::random_equipment(int level) {
	for(auto& e : bsdata<equipmenti>()) {
		if(e.level != level)
			continue;
		bool valid = true;
		for(auto& c : e.conditions) {
			if(!c)
				break;
			if(!ismatch(c)) {
				valid = false;
				break;
			}
		}
		if(!valid)
			continue;
		item it = e.object;
		it.finish();
		if(e.level != 0)
			it.setstarted(true);
		if(e.slot) {
			if(wears[e.slot])
				continue;
			else
				wears[e.slot] = it;
		} else
			add(it);
	}
}