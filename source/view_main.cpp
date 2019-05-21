#include "view.h"

static menu	menu_main[] = {{NewGame, {"Create New Game", "Начать новую игру"}},
{LoadGame, {"Load Saved game", "Загрузить сохраненную игру"}},
{Cancel, {"Exit game", "Выйти из игры"}}
};

static int buttons(int x, int y, int w, int focus, aref<menu> elements) {
	int y0 = y;
	draw::state push;
	draw::fore = colors::white;
	draw::setbigfont();
	for(auto& e : elements) {
		y += draw::linetext(x, y, w, e.id,
			AlignCenter | draw::getfstate(e.id, focus),
			e.text[0]);
	}
	return y - y0;
}

command_s draw::mainmenu() {
	auto focus = NewGame;
	while(true) {
		draw::background(MENU);
		buttons(80, 110, 170, focus, menu_main);
		int id = draw::input();
		switch(id) {
		case 0:
		case KeyEscape:
			return NoCommand;
		case KeyDown:
		case KeyUp:
			focus = (command_s)menu_main->getnextid(focus, id);
			break;
		case KeyEnter:
			return focus;
		}
	}
}