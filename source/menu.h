//Copyright 2017 by Pavel Chistyakov
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

#include "rect.h"

#pragma once

struct menu {
	int					id;
	const char*			text;
	rect				rc;
	//
	operator bool() const { return id != 0; }
	const menu*			find(int id) const;
	int					getnextid(int focus, int direction) const;
};
