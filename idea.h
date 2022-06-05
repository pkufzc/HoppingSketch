#pragma once
#include "types.h"
#include "utils.h"
#include "hash.h"

const int M = 8;

class mpbf : hpwin_sketch<bool> {
public:
	mpbf(size_t memory, int hash_num, int win_num) : hash_num(hash_num) {
		hf = new hash_func[hash_num];
		N = memory / (M+1);
		Bsize = win_num / M;
		
		lat = 0;
		cells = new uint64_t*[M];
		
		for (int i = 0; i < M; ++i)
			cells[i] = new uint64_t[memory / (M+1)];
		backup = new uint64_t[memory / (M+1)];
	}

	void ins_latest(elem_t e, int dt) { ins_latest(e); }

	void ins_latest(elem_t e) {
		int i = ((lat-1) / Bsize) % M;
		int bid = (i-1) / bsize;
		int rem = (i-1) % bsize;
		if (rem == 0) {
			for (int k = 0; k < hash_num; ++k) {
				int idx = (hf[k](e) + bid) % N;
				cells[i][idx] = (cells[i][idx] & ~65535) | lat;
			}
		} else {
			for (int k = 0; k < hash_num; ++k) {
				int idx = (hf[k](e) + bid) % N;
				cells[i][idx] |= 1ul << (rem + 15);
			}
		}
	}


	bool query(int l, int r, elem_t e) const {
		uint64_t *arr = ((lat-1) / Bsize - (r-1) / Bsize == M ? backup : cells[(r-1) / Bsize % M]);

		int bid = (r-1) / bsize;
		int rem = (r-1) % bsize;

	//	fprintf(stderr, "%d %d %d\n",l,r,rem);
		
		if (rem == 0) {
			for (int k = 0; k < hash_num; ++k) {
				int idx = (hf[k](e) + bid) % N;
				if ((arr[idx] & 65535) < l) return false;
			}
			return true;
		} else {
			for (int k = 0; k < hash_num; ++k) {
				int idx = (hf[k](e) + bid) % N;
				uint64_t tmp = (arr[idx] >> 16) & ((1ul << rem) - 1);
				int lst = tmp ? bid * bsize + 65 - __builtin_clzll(tmp) : (arr[idx] & 65535);
				//fprintf(stderr, "%d %d %d\n",l,r,lst);
				if (lst < l) return false;
			}
			return true;
		}
	}

	void new_win() {
		if (lat % bsize == 0 && lat) {
			int bid = lat / bsize;
			for (int i = 0; i < N; ++i) {
				uint64_t pv = cells[i][bid-1];
				if (pv >> 16) cells[i][bid] = (bid-1) * bsize + 65 - __builtin_clzll(pv>>16);
				else cells[i][bid] = pv & 65535;
			}
		}
		++lat;
	}

private:
	int hash_num, lat;
	size_t N, Bsize;
	const int bsize = 49;
	uint64_t **cells, *backup;
	hash_func* hf;
};