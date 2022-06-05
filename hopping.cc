#include "hopping.h"
#include "hpwin_sketch.h"

#include <climits>
#include <cstdio>

hopping_bf::hopping_bf(int memory, int size, int k, int period) : size(size), k(k), period(period) {
	bf = new bloom_filter*[size];
	//printf("hop: %d %d\n", size, k);
	for (int i = 0; i < size; ++i)
		bf[i] = new bloom_filter(memory / size, 1);
	last_win = 0;
	counter = 0;
}

hopping_bf::~hopping_bf() {
	for (int i = 0; i < size; ++i)
		delete bf[i];
	delete[] bf;
}

void hopping_bf::new_win() {
	if (counter % period == 0) {
		last_win++;
		bf[last_win % size]->clear();
	}
	counter++;
}

void hopping_bf::ins_latest(elem_t e, int delta) {
	for (int i = 0; i < k; ++i) bf[(last_win + i) % size]->add(e, last_win, delta);
}

bool hopping_bf::query(int win, elem_t e) const {
	for (int i = win; i < win + k; ++i)
		if (i > last_win + k - 1 - size && !bf[i % size]->query(e, win)) return false;
	return true;
}

bool hopping_bf::query(int l, int r, elem_t e) const {
	for (int i = l; i <= r; ++i) if (query(i, e)) return true;
	return false;
}

hopping_cm::hopping_cm(size_t memory, int size, int k, int period) : size(size), k(k), period(period) {
	cm = new cmsketch*[size];
	//printf("hop: %d %d\n", size, k);
	for (int i = 0; i < size; ++i)
		cm[i] = new cmsketch(memory / size, 1);
	last_win = 0;
	counter = 0;
}

hopping_cm::~hopping_cm() {
	for (int i = 0; i < size; ++i) {
		//fprintf(stderr, "free %d\n", i);
		delete cm[i];
	}
	delete[] cm;
}

void hopping_cm::new_win() {
	if (counter % period == 0) {
		last_win++;
		cm[last_win % size]->clear();
	}
	counter++;
}

void hopping_cm::ins_latest(elem_t e, int delta) {
	for (int i = 0; i < k; ++i) cm[(last_win + i) % size]->add(e, last_win, delta);
}

int hopping_cm::query(int win, elem_t e) const {
	int res = INT_MAX;
	for (int i = win; i < win + k; ++i)
		if (i > last_win + k - 1 - size) res = std::min(res, cm[i % size]->query(e, win));
	return res;
}

int hopping_cm::query(int l, int r, elem_t e) const {
	int sum = 0;
	for (int i = l; i <= r; ++i) sum += query(i, e);
	return sum;
}


hopping_cnt::hopping_cnt(size_t memory, int size, int k, int period) : size(size), k(k), period(period) {
	cnt = new countsk*[size];
	//printf("hop: %d %d\n", size, k);
	for (int i = 0; i < size; ++i)
		cnt[i] = new countsk(memory / size, 1);
	last_win = 0;
	counter = 0;
}

hopping_cnt::~hopping_cnt() {
	for (int i = 0; i < size; ++i)
		delete cnt[i];
	delete[] cnt;
}

void hopping_cnt::new_win() {
	if (counter % period == 0) {
		last_win++;
		cnt[last_win % size]->clear();
	}
	counter++;
}

void hopping_cnt::ins_latest(elem_t e, int delta) {
	for (int i = 0; i < k; ++i) cnt[(last_win + i) % size]->add(e, last_win, delta);
}
#include <cassert>
int hopping_cnt::query(int win, elem_t e) const {
	std::vector<int> res;
	for (int i = win; i < win + k; ++i)
		if (i > last_win + k - 1 - size) res.push_back(cnt[i % size]->query(e, win));
	assert(res.size() > 0);
	std::sort(res.begin(), res.end());
	if (res.size() % 2) return res[res.size()/2];
	else return (res[res.size()/2] + res[res.size()/2-1])/2;
}

int hopping_cnt::query(int l, int r, elem_t e) const {
	int sum = 0;
	for (int i = l; i <= r; ++i) sum += query(i, e);
	return sum;
}

hopping_cu::hopping_cu(size_t memory, int size, int k, int period) : size(size), k(k), period(period) {
	cu = new cmsketch*[size];
	//printf("hop: %d %d\n", size, k);
	for (int i = 0; i < size; ++i)
		cu[i] = new cmsketch(memory / size, 1);
	last_win = 0;
	counter = 0;
	tmp = new int[k];
}

hopping_cu::~hopping_cu() {
	for (int i = 0; i < size; ++i)
		delete cu[i];
	delete[] cu;
}

void hopping_cu::new_win() {
	if (counter % period == 0) {
		last_win++;
		cu[last_win % size]->clear();
	}
	counter++;
}

void hopping_cu::ins_latest(elem_t e, int delta) {
	
	for (int _t = 0; _t < delta; ++_t) {
		int res = INT_MAX;
		for (int i = 0; i < k; ++i) res = std::min(res, tmp[i] = cu[(last_win + i) % size]->query(e, last_win));
		for (int i = 0; i < k; ++i) if (tmp[i] == res) cu[(last_win + i) % size]->add(e, last_win, 1);
	}
}

int hopping_cu::query(int win, elem_t e) const {
	int res = INT_MAX;
	for (int i = win; i < win + k; ++i)
		if (i > last_win + k - 1 - size) res = std::min(res, cu[i % size]->query(e, win));
	return res;
}

int hopping_cu::query(int l, int r, elem_t e) const {
	int sum = 0;
	for (int i = l; i <= r; ++i) sum += query(i, e);
	return sum;
}
