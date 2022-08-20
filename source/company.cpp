#include "main.h"
#include "log.h"

using namespace log;

unsigned historyi::gethistorymax() const {
	for(unsigned i = 0; i < history_max; i++)
		if(!history[i])
			return i;
	return history_max;
}