#pragma once

struct campaigni {
	const char*		name;
	const char*		intro;
	const char*		city;
	const char*		inn;
	const char*		temple;
	const char*		tavern;
	const char*		feast;
	short			city_frame, inn_frame, temple_frame, tavern_frame;
	unsigned		lose_round;
	void			clear();
	void			readc(const char* name);
};
extern campaigni	campaign;