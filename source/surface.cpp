/* Copyright 2013 by Pavel Chistyakov
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "crt.h"
#include "draw.h"
#include "drawbits.h"
#include "surface.h"

draw::surface*			draw::canvas;

draw::surface::surface() : width(0), height(0), scanline(0), bpp(32), bits(0) {}

draw::surface::surface(int width, int height, int bpp) : surface() {
	resize(width, height, bpp, true);
}

draw::surface::plugin* draw::surface::plugin::first;

draw::surface::plugin::plugin(const char* name, const char* filter) : name(name), filter(filter), next(0) {
	seqlink(this);
}

unsigned char* draw::surface::ptr(int x, int y) {
	return bits + y * scanline + x * (bpp / 8);
}

draw::surface::~surface() {
	resize(0, 0, 0, true);
}

void draw::surface::resize(int width, int height, int bpp, bool alloc_memory) {
	if(this->width == width && this->height == height && this->bpp == bpp)
		return;
	this->bpp = bpp;
	this->width = width;
	this->height = height;
	this->scanline = color::scanline(width, bpp);
	if(width) {
		unsigned size = (height + 1)*scanline;
		if(bits) {
			delete bits;
			bits = 0;
		}
		if(alloc_memory)
			bits = new unsigned char[size];
	} else {
		delete bits;
		bits = 0;
	}
}

void draw::surface::convert(int new_bpp, color* pallette) {
	if(bpp == new_bpp) {
		bpp = iabs(new_bpp);
		return;
	}
	auto old_scanline = scanline;
	scanline = color::scanline(width, new_bpp);
	if(iabs(new_bpp) <= bpp)
		color::convert(bits, width, height, new_bpp, 0, bits, bpp, draw::palt, old_scanline);
	else {
		unsigned char* new_bits = new unsigned char[(height + 1)*scanline];
		color::convert(
			new_bits, width, height, new_bpp, draw::palt,
			bits, bpp, draw::palt, old_scanline);
		delete bits;
		bits = new_bits;
	}
	bpp = iabs(new_bpp);
}

int draw::getbpp() {
	return canvas ? canvas->bpp : 1;
}

int draw::getwidth() {
	return canvas ? canvas->width : 0;
}

int draw::getheight() {
	return canvas ? canvas->height : 0;
}

unsigned char* draw::ptr(int x, int y) {
	return canvas ? (canvas->bits + y * canvas->scanline + x * canvas->bpp / 8) : 0;
}