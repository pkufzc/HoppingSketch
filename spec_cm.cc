#include <cassert>
#include <algorithm>
#include "spec_cm.h"

static void upd_val(uint32_t &x, uint32_t y) {
	if (x > y) x = y;
}

spec_cm::spec_cm(int cells, int hf_num, uint32_t init_val) 
	: cells(cells), hf_num(hf_num), hf(new hash_func[hf_num]) {
	pool = new uint32_t[cells];
	for (int i = 0; i < cells; ++i) pool[i] = init_val;
	_acs_cnt = 0;
}

void spec_cm::update(elem_t e, int offset, uint32_t val) {
	for (int i = 0; i < hf_num; ++i) {
		upd_val(pool[(hf[i](e) + offset) % cells], val);
	}
}

uint32_t spec_cm::query(elem_t e, int offset) const {
	uint32_t res = 0;
	for (int i = 0; i < hf_num; ++i)
		res = std::max(res, pool[(hf[i](e) + offset) % cells]);
	return res;
}
