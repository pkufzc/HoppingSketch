#include <cassert>
#include "bloom_filter.h"

bloom_filter::bloom_filter(int memory, int hf_num) 
	: bits((memory / 8) * 64), hf_num(hf_num), bs(new bitset(bits)), hf(new hash_func[hf_num]) {
	_acs_cnt = 0;
}

bloom_filter::~bloom_filter() {
	delete[] bs;
	delete[] hf;
}

void bloom_filter::add(elem_t e, int offset, int delta) {
	for (int i = 0; i < hf_num; ++i)
		bs->set((hf[i](e) + offset) % bits);
}

bool bloom_filter::query(elem_t e, int offset) const {
	_acs_cnt += hf_num;
	bool ret = true;
	for (int i = 0; i < hf_num; ++i)
		if (!(*bs)[(hf[i](e) + offset) % bits])
			ret = false;
	return ret;
}

void bloom_filter::shrink() {
	bits /= 2;
	assert(bits % 64 == 0);
	bitset *new_bs = new bitset(bits);
	for (int i = 0; i < bits; ++i)
		if ((*bs)[i] || (*bs)[i + bits])
			new_bs->set(i);
	delete bs;
	bs = new_bs;
}

void bloom_filter::copy(bloom_filter *bf) {
	assert(bf->bits == bits);
	bs->clear();
	for (int i = 0; i < bits; ++i)
		if ((*bf->bs)[i]) bs->set(i);
	for (int i = 0; i < hf_num; ++i)
		hf[i] = bf->hf[i];
}

void bloom_filter::merge(bloom_filter *bf) {
	assert(bf->bits == bits);
	for (int i = 0; i < bits; ++i)
		if ((*bf->bs)[i]) bs->set(i);
}

void bloom_filter::clear(bool reset_hf) {
	if (reset_hf) {
		for (int i = 0; i < hf_num; ++i)
			hf[i].reset(); // Use new hash functions
	}
	bs->clear();
}