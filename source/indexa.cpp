#include "main.h"

static unsigned char push_stack;
static unsigned char pop_stack;
static indext stack[256];
static unsigned short stack_cost[mpx * mpy];

static void pushv(indext v) {
	if(v == Blocked)
		return;
	stack[push_stack++] = v;
}

static indext popv() {
	return stack[pop_stack++];
}

static void clear_stack() {
	push_stack = pop_stack = 0;
	memset(stack_cost, 0xFF, sizeof(stack_cost));
}

static unsigned short getcost(indext i) {
	if(i == Blocked)
		return Blocked;
	return stack_cost[i];
}

static void setcost(indext i, unsigned short v) {
	if(i == Blocked)
		return;
	stack_cost[i] = v;
}

static void pushv(indext i, unsigned short c) {
	if(i == Blocked)
		return;
	auto c1 = getcost(i);
	if(c >= c1)
		return;
	pushv(i);
	setcost(i, c);
}

void indexa::select(const dungeoni& e, indext i, cell_s id, int r) {
	auto p = begin();
	auto pe = endof();
	clear_stack();
	pushv(i, 0);
	while(push_stack != pop_stack) {
		auto i1 = popv();
		auto c1 = getcost(i1);
		auto t1 = e.get(i1);
		if(t1 != id || c1 > r)
			continue;
		if(p >= pe)
			break;
		*p++ = i1;
		pushv(to(i1, Left), c1 + 1);
		pushv(to(i1, Right), c1 + 1);
		pushv(to(i1, Up), c1 + 1);
		pushv(to(i1, Down), c1 + 1);
	}
	count = p - begin();
}