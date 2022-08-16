#include "main.h"

static void enter_quest() {
}

void gamei::playcity() {
	imagei image;
	image.res = BUILDNGS;
	image.frame = 17;
	answers aw;
	aw.add((int)enter_quest, "Quest");
	auto p = (fnevent)aw.choosebg("You visit your base city of Waterdeep. Spend some money and rest for a moment.", image, true);
}