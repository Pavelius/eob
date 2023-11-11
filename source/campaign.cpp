#include "campaign.h"
#include "crt.h"

campaigni campaign;

void campaigni::clear() {
	memset(this, 0, sizeof(*this));
}