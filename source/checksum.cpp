namespace {
union hashsum {
	unsigned long long l;
	unsigned u[2];
};
}
static_assert(sizeof(hashsum) == 8, "union `hashsum` must be 8 bytes");

unsigned long long calculate_checksum(unsigned char* start, unsigned count) {
	unsigned r = 0;
	for(unsigned i = 0; i < count; i++)
		r += i * start[i];
	hashsum rs = {};
	rs.u[0] = r;
	rs.u[1] = count;
	return rs.l;
}