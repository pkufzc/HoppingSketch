#include "types.h"
#include "utils.h"
#include "hash.h"

class mpbf {
public:
	mpbf(int memory, int hash_num, int win_num) : hash_num(hash_num) {
		hf = new hash_func[hash_num];
		int L = (win_num - 1) / bsize + 1;
		N = memory / L / 8;
		
		cells = new uint64_t*[N];
		for (int i = 0; i < cells; ++i)
			cells[i] = new uint64_t[L];
	}

	void add(int i, elem_t e) {
		int bid = (i-1) / bsize;
		int rem = (i-1) % bsize;
		if (rem == 0) {
			for (int k = 0; k < hash_num; ++k) {
				int idx = hf[i](e) % N;
				cells[idx][bid] = (cells[idx][bid] & ~65535) | i;
			}
		} else {
			for (int k = 0; k < hash_num; ++k) {
				int idx = hf[i](e) % N;
				cells[idx][bid] |= 1ul << (rem + 15);
			}
		}
	}
private:

	bool query(int l, int r, elem_t e) {
		int bid = (r-1) / bsize;
		int rem = (r-1) % bsize;
		
		if (rem == 0) {
			for (int k = 0; k < hash_num; ++k) {
				int idx = hf[i](e) % N;
				if ((cells[idx][bid] & 65535) < l) return false;
			}
			return true;
		} else {
			for (int k = 0; k < hash_num; ++k) {
				int idx = hf[i](e) % N;
				if ((cells[idx][bid] & 65535) < l && 
					((cells[idx][bid] >> 16) & ((1ul << rem) - 1))) return false;
			}
			return true;
		}
	}

	int hash_num;
	const int bsize = 49;
	uint64_t **cells;
	hash_func* hf;
};