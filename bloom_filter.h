#pragma once
#include "utils.h"
#include "hash.h"
#include "types.h"

class bloom_filter {
public:	
	bloom_filter(int memory, int hf_num);
	~bloom_filter();

	bloom_filter(const bloom_filter &) = delete;
	void operator=(const bloom_filter &) = delete;

	void add(elem_t e, int offset = 0, int delta=1);
	bool query(elem_t e, int offset = 0) const;
	void shrink();
	void copy(bloom_filter *bf);
	void merge(bloom_filter *bf);
	void clear(bool reset_hf = true);

	int memory() const { return bs->memory(); }
    uint64_t acs_cnt() { return _acs_cnt; }
private:
	mutable uint64_t _acs_cnt;
	int bits, hf_num;
	bitset* bs;
	hash_func* hf;
};