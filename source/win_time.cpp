#include "win.h"

extern "C" void sleep(unsigned seconds)
{
	Sleep(seconds);
}