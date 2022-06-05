#pragma once
#include "bloom_filter.h"
#include "hpwin_sketch.h"
#include <cassert>

template<class sketch>
class pbf1 : hpwin_sketch<bool> {
public:
	pbf1(int memory, int depth);
	~pbf1() {
		for (int i = 0; i < depth; ++i)
			delete sk[i];
		delete[] sk;
	}
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	bool query(int l, int r, elem_t e) const;
	
private:
	bool seg_query(int l, int r, int ql, int qr, int idx, int cur_depth, elem_t e) const;
	
	sketch **sk;
	int depth;
	int rt_depth;
	int last_win;
};

template<class sketch>
class pcnt1 : hpwin_sketch<int> {
public:
	pcnt1(size_t memory, int depth) : depth(depth) {
		sk = new sketch*[depth];
		for (int i = 0; i < depth; ++i)
			sk[i] = new sketch(memory / depth, (cfg.win_num + (1<<i) - 1) >> i, 3, 1 << i);
		rt_depth = 0;
		last_win = 0;
	}
	~pcnt1() {
		for (int i = 0; i < depth; ++i) {
			fprintf(stderr, "pcnt free %d\n", i);
			delete sk[i];
		}
		delete[] sk;
	}
	void new_win() {
		last_win++;
		if ((1 << rt_depth) < last_win) ++rt_depth;
		for (int i = 0; i < depth; ++i)
			sk[i]->new_win();
	}
	void ins_latest(elem_t e, int delta = 1) {
		//if (delta>1) fprintf(stderr, "???\n");
		for (int i = 0; i < depth; ++i)
			sk[i]->ins_latest(e, delta);
	}
	int query(int l, int r, elem_t e) const {
		return seg_query(1, 1 << rt_depth, l, r, 0, rt_depth, e);
	}
	
private:
	int seg_query(int l, int r, int ql, int qr, int idx, int cur_depth, elem_t e) const {
		int mid = (l + r) / 2;
		if (ql <= l && r <= qr && cur_depth < depth) {
			assert(0 <= cur_depth && cur_depth < depth);
			//if (l != 1) printf("%d %d %d\n", cur_depth,l,idx);
			return sk[cur_depth]->query(idx + 1, e);
		}

		if (qr <= mid)
			return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e);
		else if (ql > mid)
			return seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
		else
			return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e) +
					seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
	}
	
	sketch **sk;
	int depth;
	int rt_depth;
	int last_win;
};

class pbf2 : hpwin_sketch<bool> {
public:
	pbf2(int memory, int depth) : depth(depth) {
		//printf("pbf2 %d %d\n", memory, depth);
		bf = new bloom_filter*[depth+1];
		for (int i = 0; i <= depth; ++i)
			bf[i] = new bloom_filter(memory / (depth+1), 2);
	}
	void new_win() { ++latest; }
	void ins_latest(elem_t e, int delta = 1) { seg_modify(1, 1<<depth, latest, 0, 0, e); }
	bool query(int l, int r, elem_t e) const { return seg_query(1, 1<<depth, l, r, 0, 0, e); }
	
private:
	bool seg_query(int l, int r, int ql, int qr, int idx, int cur_depth, elem_t e) const {
		//fprintf(stderr,"q %d\n", cur_depth);
		if (ql <= l && r <= qr) return bf[cur_depth]->query(e, idx);
		int mid = (l + r) / 2;
		if (qr <= mid) return seg_query(l, mid, ql, qr, idx*2, cur_depth+1, e);
		else if (ql > mid) return seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth+1, e);
		else return seg_query(l, mid, ql, qr, idx*2, cur_depth+1, e) | 
			seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth+1, e);
	}
	void seg_modify(int l, int r, int pos, int idx, int cur_depth, elem_t e) {
		//fprintf(stderr,"q %d\n", cur_depth);
		
		bf[cur_depth]->add(e, idx);
		if (l == r) return;
		int mid = (l + r) / 2;
		if (pos <= mid) seg_modify(l, mid, pos, idx*2, cur_depth+1, e);
		else seg_modify(mid+1, r, pos, idx*2+1, cur_depth+1, e);
	}
	
	bloom_filter **bf;
	int depth, latest;
};

template<class sketch>
pbf1<sketch>::pbf1(int memory, int depth) : depth(depth) {
	fprintf(stderr, "memory: %d\n", memory);
	sk = new sketch*[depth];
	for (int i = 0; i < depth; ++i)
		sk[i] = new sketch(memory / depth, (cfg.win_num + (1<<i) - 1) >> i, 3, 1 << i);
	rt_depth = 0;
	last_win = 0;
}

template<class sketch>
void pbf1<sketch>::new_win() {
	last_win++;
	if ((1 << rt_depth) < last_win) ++rt_depth;
	for (int i = 0; i < depth; ++i)
		sk[i]->new_win();
}

template<class sketch>
void pbf1<sketch>::ins_latest(elem_t e, int delta) {
	for (int i = 0; i < depth; ++i)
		sk[i]->ins_latest(e);
}

template<class sketch>
bool pbf1<sketch>::seg_query(int l, int r, int ql, int qr, int idx, int cur_depth, elem_t e) const {
	int mid = (l + r) / 2;
	if (ql <= l && r <= qr && cur_depth < depth) {
		assert(0 <= cur_depth && cur_depth < depth);
		//if (l != 1) printf("%d %d %d\n", cur_depth,l,idx);
		return sk[cur_depth]->query(idx + 1, e);
	}

	if (qr <= mid)
		return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e);
	else if (ql > mid)
		return seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
	else
		return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e) |
				seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
}

template<class sketch>
bool pbf1<sketch>::query(int l, int r, elem_t e) const {
	return seg_query(1, 1 << rt_depth, l, r, 0, rt_depth, e);
}
