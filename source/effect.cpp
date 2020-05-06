#include "main.h"

static int getduration(duration_s duration, int level) {
	return bsdata<durationi>::elements[duration].get(level);
}

void effecti::apply(creature* target, int level) const {
	int value;
	switch(type.type) {
	case Damage:
		value = damage.roll();
		if(damage_increment) {
			auto mi = level / damage_increment;
			if(mi > damage_maximum)
				mi = damage_maximum;
			for(int i = 0; i < mi; i++)
				value += damage_per.roll();
		}
		if(!target->save(value, SaveVsMagic, save, save_bonus))
			target->damage((damage_s)type.value, value);
		break;
	case Spell:
		value = getduration(duration, level);
		target->add((spell_s)type.value, value, save, save_bonus);
		break;
	case Item:
		target->setweapon((item_s)type.value, xrand(1, 4) + level);
		break;
	}
}