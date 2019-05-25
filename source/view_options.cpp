#include "draw.h"
#include "main.h"

static command_s choosemenu(aref<command_s> options, command_s focus) {
	while(true) {
		draw::animation::render(0, false);
		if(true) {
			draw::state push;
			draw::setbigfont();
			draw::form({0, 0, 22 * 8 + 2, 174}, 2);
			draw::fore = colors::title;
			draw::textb(6, 6, "Game Options:");
			draw::fore = colors::white;
			for(int i = 0; options[i]; i++)
				draw::button(4, 17 + i * 15, 166,
					options[i],
					draw::getfstate(options[i], focus),
					getstr(options[i]));
		}
		int id = draw::input();
		switch(id) {
		case KeyEscape:
		case Cancel:
			return Cancel;
		case KeyUp:
			if(focus == options[0])
				focus = options[options.count - 1];
			else {
				auto n = options.indexof(focus);
				if(n != -1)
					focus = options[n - 1];
			}
			break;
		case KeyDown:
			if(focus == options[options.count - 1])
				focus = options[0];
			else {
				auto n = options.indexof(focus);
				if(n != -1)
					focus = options[n + 1];
			}
			break;
		case KeyEnter:
			return focus;
		}
	}
}

command_s game::action::options() {
	static command_s main[] = {PrayForSpells, MemorizeSpells, NewGame, LoadGame, SaveGame, Settings, QuitGame};
	auto focus = PrayForSpells;
	while(true) {
		auto id = choosemenu(main, focus);
		switch(id) {
		case Cancel:
			return NoCommand;
		case SaveGame:
			game::write();
			return NoCommand;
		case LoadGame:
			game::read();
			return NoCommand;
		case NewGame:
			if(!dlgask("Area you sure want to start new game?"))
				break;
			return NewGame;
		case QuitGame:
			if(!dlgask("Area you sure want to quit game?"))
				break;
			exit(0);
			break;
		case MemorizeSpells:
			game::action::preparespells(Mage);
			break;
		case PrayForSpells:
			game::action::preparespells(Cleric);
			break;
		}
	}
}