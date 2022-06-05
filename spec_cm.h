#pragma once
#include "utils.h"
#include "hash.h"
#include "types.h"

class spec_cm {
public:	
	spec_cm(int cells, int hf_num, uint32_t init_val);

	void update(elem_t e, int offset, uint32_t val);
	uint32_t query(elem_t e, int offset = 0) const;
    uint64_t acs_cnt() { return _acs_cnt; }
private:
	mutable uint64_t _acs_cnt;
	int cells, hf_num;
	uint32_t *pool;
	hash_func* hf;
};