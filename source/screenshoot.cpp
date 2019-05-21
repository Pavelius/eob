#include "crt.h"
#include "draw.h"
#include "screenshoot.h"

draw::screenshoot::screenshoot(rect rc, bool fade) : surface(rc.width(), rc.height(), getbpp())
{
	x = rc.x1;
	y = rc.y1;
	if(draw::canvas)
	{
		blit(*this, 0, 0, width, height, 0, *draw::canvas, x, y);
		if(fade)
		{
			draw::state push;
			draw::canvas = this;
			draw::setclip();
			draw::rectf({0, 0, width, height}, colors::black, 128);
		}
	}
}

draw::screenshoot::screenshoot(bool fade) : screenshoot({0, 0, getwidth(), getheight()}, fade)
{
}

draw::screenshoot::~screenshoot()
{
}

void draw::screenshoot::restore()
{
	setclip();
	if(draw::canvas)
		blit(*draw::canvas, x, y, width, height, 0, *this, 0, 0);
}

void draw::screenshoot::blend(draw::surface& e, unsigned delay)
{
	if(e.width != width || e.height != height || !delay)
		return;
	unsigned t0 = clock();
	unsigned t = t0;
	while(t < t0 + delay)
	{
		auto pd = (color*)draw::canvas->bits;
		auto p1 = (color*)bits;
		auto p2 = (color*)e.bits;
		unsigned char alpha = ((t - t0) * 255) / delay;
		for(int i = 0; i < width * height; i++)
			pd[i] = p2[i].mix(p1[i], alpha);
		draw::input(true);
		t = clock();
	}
}