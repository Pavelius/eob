#include "crt.h"
#include "draw.h"
#include "sprite.h"

sprite::operator bool() const {
	return issignature(name, "PMA");
}

// –азмер секции дополнительной информации,
// котора€ находитс€ между началом данных изображени€ и концом указателей на них
int sprite::esize() const {
	return frames[0].offset - (sizeof(sprite) + sizeof(frame)*(count - 1));
}

const unsigned char* sprite::edata() const {
	return (const unsigned char*)this + sizeof(sprite) + sizeof(frame)*(count - 1);
}

int sprite::ganim(int index, int tick) {
	if(!cicles)
		return 0;
	cicle* c = gcicle(index);
	if(!c->count)
		return 0;
	if(flags&NoIndex)
		return c->start + tick % c->count;
	return gindex(c->start + tick % c->count);
}

const sprite::frame& sprite::get(int id) const {
	if(id >= count)
		return frames[0];
	return frames[id];
}

int sprite::glyph(unsigned sym) const {
	// First interval (latin plus number plus ASCII)
	unsigned* pi = (unsigned*)edata();
	unsigned* p2 = pi + esize() / sizeof(unsigned);
	unsigned n = 0;
	while(pi < p2) {
		if(sym >= pi[0] && sym <= pi[1])
			return sym - pi[0] + n;
		n += pi[1] - pi[0] + 1;
		pi += 2;
	}
	return 't' - 0x21; // Unknown symbol is question mark
}

rect sprite::frame::getrect(int x, int y, unsigned flags) const {
	int x2, y2;
	if(!offset)
		return{0, 0, 0, 0};
	if(flags&ImageMirrorH) {
		x2 = x;
		if((flags&ImageNoOffset) == 0)
			x2 += ox;
		x = x2 - sx;
	} else {
		if((flags&ImageNoOffset) == 0)
			x -= ox;
		x2 = x + sx;
	}
	if(flags&ImageMirrorV) {
		y2 = y;
		if((flags&ImageNoOffset) == 0)
			y2 += oy;
		y = y2 - sy;
	} else {
		if((flags&ImageNoOffset) == 0)
			y -= oy;
		y2 = y + sy;
	}
	return{x, y, x2, y2};
}