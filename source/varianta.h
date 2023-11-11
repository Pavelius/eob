#include "crt.h"
#include "point.h"
#include "variant.h"

#pragma once

struct varianta : adat<variant, 12> {
};
class variantc : public adat<variant> {
public:
	void	cspells(const creature* p, bool expand);
	int		chooselv(class_s type) const;
	void	exclude(variant v);
	void	match(variant v, bool keep);
	void	match(point start, int radius, bool keep);
	void	matchsl(class_s type, int level);
	void	select(variant_s type);
	void	sort();
};
