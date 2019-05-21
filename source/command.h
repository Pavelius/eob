//   Copyright 2016 by Pavel Chistyakov
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#pragma once

// Use this macro and don't have any problem
#define COMMAND(name) static void name();\
extern command* command_##name;\
static command local_##name(&command_##name, name);\
static void name()

struct command
{
	command*		next;
	void			(*proc)();
	command(command** first, void(*proc)());
	void			execute();
};

extern command*		command_app_initialize; // Standart initialization command