#include "crt.h"
#include "pma.h"

const pma* pma::getheader(const char* id) const
{
	auto p = this;
	while(p->name[0])
	{
		if(p->name[0] == id[0]
			&& p->name[1] == id[1]
			&& p->name[2] == id[2])
			return p;
		p = (pma*)((char*)p + p->size);
	}
	return 0;
}

const char* pma::getstring(int id) const
{
	auto p = getheader("STR");
	if(!p || id > count)
		return "";
	return (char*)this + ((unsigned*)((char*)this + sizeof(*this)))[id];
}

int pma::find(const char* name) const
{
	auto p = getheader("STR");
	for(int i = 1; i <= count; i++)
	{
		if(strcmp(getstring(i), name) == 0)
			return i;
	}
	return 0;
}