//Copyright 2013 by Pavel Chistyakov
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

struct color {
	unsigned char			b;
	unsigned char			g;
	unsigned char			r;
	unsigned char			a;
	constexpr bool operator==(const color& e) const { return b == e.b && g == e.b && r == e.r; }
	inline void				clear() { *((int*)this) = 0; }
	static void				convert(void* output, int width, int height, int output_bpp, const void* output_pallette, const void* input, int input_bpp, const void* input_pallette = 0, int input_scanline = 0);
	static color			create(unsigned char r, unsigned char g, unsigned char b);
	color					darken() const;
	int						find(const void* pallette, int count) const;
	color					gray() const;
	color					lighten() const;
	color					mix(const color c1, unsigned char s = 128) const;
	color					negative() const;
	void					read(const void* scanline, int x, int bpp, const void* pallette = 0);
	static void				rgb2bgr(color* source, int count);
	static int				scanline(int width, int bpp);
	void					write(void* scanline, int x, int bpp, const void* pallette = 0, int color_count = 0) const;
};
namespace colors {
extern color				black;
extern color				blue;
extern color				gray;
extern color				green;
extern color				red;
extern color				yellow;
extern color				white;
}