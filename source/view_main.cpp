#include "draw.h"
#include "main.h"

static menu	menu_main[] = {{NewGame, "Create New Game"},
{LoadGame, "Load Saved game"},
{Cancel, "Exit game"},
};

command_s draw::mainmenu() {
	auto focus = NewGame;
	while(ismodal()) {
		draw::background(MENU);
		auto x = 80;
		auto y = 110;
		auto w = 170;
		draw::state push;
		draw::fore = colors::white;
		draw::setbigfont();
		for(auto& e : menu_main) {
			y += draw::linetext(x, y, w, e.id,
				AlignCenter | draw::getfstate(e.id, focus),
				e.text);
		}
		domodal();
		switch(hot::key) {
		case 0:
		case KeyEscape:
			breakmodal(0);
			break;
		case KeyDown:
		case KeyUp:
			focus = (command_s)menu_main->getnextid(focus, hot::key);
			break;
		case KeyEnter:
			breakmodal(focus);
			break;
		}
	}
	return (command_s)getresult();
}