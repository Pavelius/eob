#pragma once

enum event_s : unsigned char {
	TimeEvent, VariableExpandEvent, VariableOverwelmedEvent
};
struct eventi {
	event_s				type;
	unsigned			param1, param2;
	const char*			script;
	static eventi*		add(event_s type);
	void				clear();
	static eventi*		findtimer();
	unsigned			getfinish() const;
	unsigned			getstart() const;
};