#include "spec_cm.h"
#include "hpwin_sketch.h"

#include <algorithm>
#include <vector>
#include <cstdio>

class dct : hpwin_sketch<bool> {
public:
	dct(int memory, int size) : size(size) {
		build(1, size, 0);
		int sum = 0;
		for (int i = 0; i < lsize.size(); ++i) {
			int k = 0;
			for (; (1<<k) < lsize[i]; ++k);
			printf("%d(%d) ", k, lsize[i]);
			sum += k; lsize[i] = k;
			
		}
		puts("");
		int unit_size = memory / sum;
		cm = new spec_cm*[lsize.size()];
		for (int i = 0; i < lsize.size(); ++i)
			cm[i] = new spec_cm(unit_size, 3, size+10);
		lst = 0;
	}

	void build(int l, int r, int cur_depth) {
		if (cur_depth >= lsize.size())
			lsize.resize(cur_depth + 1);
		lsize[cur_depth] = std::max(lsize[cur_depth], r - l + 2);
		if (r - l <= 1) return;
		int mid = (l + r) / 2;
		build(l, mid - 1, cur_depth + 1);
		build(mid + 2, r, cur_depth + 1);
	}

	void new_win() { ++lst; }
	void ins_latest(elem_t e) { _insert(1, size, lst, 0, 0, e); }
	bool query(int l, int r, elem_t e) const { return _query(1, size, l, r, 0, 0, e); }

private:
	spec_cm **cm;
	std::vector<int> lsize;
	int lst, size;

	void _insert(int l, int r, int pos, int cur_depth, int idx, elem_t e) {
		int mid = (l + r) / 2;
		if (pos <= mid) cm[cur_depth]->update(e, idx * 2, mid - pos);
		else cm[cur_depth]->update(e, idx * 2 + 1, pos - mid - 1);
		if (pos == mid || pos == mid + 1) return;
		if (pos < mid) _insert(l, mid - 1, pos, cur_depth + 1, idx * 2, e);
		else _insert(mid + 2, r, pos, cur_depth + 1, idx * 2 + 1, e);
	}

	bool _query(int l, int r, int ql, int qr, int cur_depth, int idx, elem_t e) const {
		int mid = (l + r) / 2;

		if (ql <= mid + 1 && mid <= qr) {
			int lp = mid - cm[cur_depth]->query(e, idx * 2);
			if (ql <= lp) return true;
			if (qr <= mid) return false;
			int rp = mid + 1 + cm[cur_depth]->query(e, idx * 2 + 1);
			return rp <= qr;
		}

		if (ql < mid) return _query(l, mid-1, ql, qr, cur_depth + 1, idx * 2, e);
		else return _query(mid+2, r, ql, qr, cur_depth + 1, idx * 2 + 1, e);
	}
};