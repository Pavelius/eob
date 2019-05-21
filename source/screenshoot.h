//Copyright 2014 by Pavel Chistyakov
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

#include "point.h"
#include "surface.h"

#pragma once

namespace draw
{
	struct screenshoot : public point, public surface
	{
		screenshoot(bool fade = false);
		screenshoot(rect rc, bool fade = false);
		~screenshoot();
		void				blend(draw::surface& e, unsigned delay);
		void				restore();
	};
}