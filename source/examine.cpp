#include "main.h"

static chati messages[] = {
	{TalkAbout, {BRICK, CellDecor2, Uncommon}, "Dirty drainage gate"},
	{TalkAbout, {BRICK, CellDecor2}, "A drainage gate"},
	{TalkAbout, {CellKeyHole1}, "This is a keyhole"},
	{TalkAbout, {CellKeyHole2}, "This is a keyhole"},
	{TalkAbout, {Uncommon}, "What?"},
	{TalkAbout, {}, "What do you want?"},
	{}
};

void dungeoni::examine(creature* pc, overlayi* po) {
	if(!pc)
		return;
	varianta variants;
	variants.add(head.type);
	variants.add(gettype(po));
	for(auto n = 0; n < pc->getclasscount(); n++)
		variants.add(pc->getclass(pc->getclass(), n));
	auto text = messages->find(variants);
	if(!text)
		return;
	pc->say(text);
}