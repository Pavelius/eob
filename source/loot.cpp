#include "crt.h"
#include "loot.h"

looti last_loot;

void looti::clear() {
	memset(this, 0, sizeof(*this));
}