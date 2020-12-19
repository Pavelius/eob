#include "main.h"

struct equipmenti {
	variant			conditions[4];
	item			object;
	wear_s			slot;
};

BSDATA(equipmenti) = {{{Dwarf, Fighter}, AxeBattle, RightHand},
{{Dwarf, Fighter}, Shield, LeftHand},
{{Halfling, Fighter}, SwordShort, RightHand},
{{Fighter}, SwordLong, RightHand},
{{Fighter}, ArmorStuddedLeather, Body},
{{Ranger}, SwordLong, RightHand},
{{Ranger}, SwordShort, LeftHand},
{{Cleric}, ArmorLeather, Body},
{{Cleric}, Mace, RightHand},
{{Theif}, ArmorLeather, Body},
{{Mage}, Staff, RightHand},
{{Mage}, Robe, Body},
{{}, Dagger, RightHand},
{{}, ArmorLeather, Body},
{{}, RationIron},
{{Fighter}, Ration},
{{Theif}, TheifTools},
{{Cleric}, {BluePotion, CureLightWounds}},
};
INSTELEM(equipmenti);

void creature::random_equipment() {
	for(auto& e : bsdata<equipmenti>()) {
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
		item it = e.object; it.finish();
		if(e.slot) {
			if(wears[e.slot])
				continue;
			else
				wears[e.slot] = it;
		} else
			add(it);
	}
}