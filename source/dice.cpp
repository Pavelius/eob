#include "crt.h"
#include "dice.h"
#include "stringbuilder.h"

int dice::roll(int c, int d) {
	if(!d)
		return c;
	int result = 0;
	for(int i = 0; i < c; i++)
		result += 1 + (rand() % d);
	return result;
}

void dice::print(stringbuilder& sb) const {
	if(c == 0 && d == 0 && b==0)
		sb.adds("None");
	else if(d == 0)
		sb.adds("%1i", c);
	else
		sb.adds("%1id%2i", c, d);
	if(b != 0)
		sb.add("%+1i", b);
}

const char* dice::range(char* result, const char* result_maximum) const {
	stringbuilder sc(result, result_maximum);
	sc.add("%1i-%2i", c + b, c*d + b);
	return result;
}