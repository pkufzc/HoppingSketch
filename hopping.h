#pragma once
#include "bloom_filter.h"
#include "cmsketch.h"
#include "cusketch.h"
#include "countsk.h"
#include "hpwin_sketch.h"

class hopping_bf : hpwin_sketch<bool> {
public:
	hopping_bf(int memory, int size, int k, int per = 1);
	~hopping_bf();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	bool query(int l, int r, elem_t e) const;
	bool query(int win, elem_t e) const;

private:
	bloom_filter **bf;
	int size, k, last_win, period, counter;
};

class hopping_cm : hpwin_sketch<int> {
public:
	hopping_cm(size_t memory, int size, int k, int per = 1);
	~hopping_cm();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	int query(int win, elem_t e) const;

private:
	cmsketch **cm;
	int size, k, last_win, period, counter;
};

class hopping_cnt : hpwin_sketch<int> {
public:
	hopping_cnt(size_t memory, int size, int k, int per = 1);
	~hopping_cnt();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	int query(int win, elem_t e) const;

private:
	countsk **cnt;
	int size, k, last_win, period, counter;
};

class hopping_cu : hpwin_sketch<int> {
public:
	hopping_cu(size_t memory, int size, int k, int per = 1);
	~hopping_cu();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	int query(int win, elem_t e) const;

private:
	cmsketch **cu; // No that's right
	int size, k, last_win, period, counter, *tmp;
};