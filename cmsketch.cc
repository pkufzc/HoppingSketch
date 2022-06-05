#include <cassert>
#include <cstring>
#include <climits>
#include <cstdio>
#include "cmsketch.h"

cmsketch::cmsketch(size_t memory, int hash_num) 
	: size(memory / 4), hash_num(hash_num), hf(new hash_func[hash_num]) {
	_acs_cnt = 0;
	cells = (unsigned*) malloc(memory);
	memset(cells, 0, memory);
}

cmsketch::~cmsketch() {
	delete[] cells;
	delete[] hf;
}

void cmsketch::add(elem_t e, int offset, int delta) {
	//if (delta>1) fprintf(stderr, "???\n");
	for (int i = 0; i < hash_num; ++i) 
		cells[(hf[i](e) + offset) % size] += delta;
}

int cmsketch::query(elem_t e, int offset) const {
	_acs_cnt += hash_num;
	unsigned ret = INT_MAX;
	for (int i = 0; i < hash_num; ++i) 
		ret = std::min(ret, cells[(hf[i](e) + offset) % size]);
	return ret;
}

void cmsketch::copy(cmsketch *oth) {
	assert(oth->size == size);
	for (int i = 0; i < size; ++i)
		cells[i] = oth->cells[i];
	for (int i = 0; i < hash_num; ++i)
		hf[i] = oth->hf[i];
}

void cmsketch::clear(bool reset_hf) {
	if (reset_hf) {
		for (int i = 0; i < hash_num; ++i)
			hf[i].reset(); // Use new hash functions
	}
	memset(cells, 0, sizeof(unsigned) * size);
}