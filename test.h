#pragma once
#include "hpwin_sketch.h"
#include "config.h"
#include "env.h"
#include "utils.h"
#include <cmath>
#include <unordered_map>
#include <chrono>

double test(hpwin_sketch<bool> *sk, int l, int r) {
	int fp = 0;
	//fprintf(stderr, "%d %d %d\n", l,r,cfg.test_elem_num);
	for (int e = 1; e <= cfg.test_elem_num; ++e)
		if (sk->query(l, r, e))
			++fp;
	return 1.0 * fp / cfg.test_elem_num;
}

void test_wfpr(hpwin_sketch<bool> *sk, double* result) {
	for (int len = 1; len <= cfg.qy_win; len <<= 1) {
		int fp = 0;
		for (int k=1; k<=cfg.test_elem_num; ++k) {
			int l = rand() % (cfg.win_num - len + 1) + 1;
			int r = l + len - 1;
			int e = rand() % 1000000;
			fp += sk->query(l,r,e);
		}
		result[len] = 1.0 * fp / cfg.test_elem_num;
	}
}

void test_are(hpwin_sketch<int> *sk, double *result, double *aae) {
	int j = 0;
	double fir = packets[0].ts;
	hashmap ecnt;

	for (int len = 1; len <= cfg.qy_win; len <<= 1) {
		double sum = 0, sumaae = 0;
		int tot = 0;
		for (int i = 1; i <= cfg.win_num; ++i) {
			//fprintf(stderr, "procing %d\n", i);
			for (int k = win_beg[i]; k < win_beg[i + 1]; ++k) 
				ecnt.add(packets[k].e, 1);

			if (i > len) {
				for (int k = win_beg[i - len]; k < win_beg[i - len + 1]; ++k) 
					ecnt.add(packets[k].e, -1);
			}

			if (i % 1000 == 0) fprintf(stderr, "testing %d %d\n", i, len);

			int times = (cfg.test_elem_num + cfg.win_num - 1) / cfg.win_num;
			if (ecnt.empty()) times = 0;
			for (int k = 0; k < times; ++k) {
				auto pr = ecnt.sample();
				int ret = sk->query(i, i + len - 1, pr.first), real = pr.second;
				//printf("%d %d\n", ret, real);
				sum += fabs(ret - real) / real;
				sumaae += fabs(ret - real);
				tot++;
			}
		}
		result[len] = sum / tot;
		aae[len] = sumaae / tot;
	}
}


void test_cntspeed(hpwin_sketch<int> *sk, double *result) {
	using namespace std::chrono;
	for (int len = 1; len <= cfg.qy_win; len <<= 1) {
		const int NN = 100000;
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		for (int i = 0; i < NN; ++i) {
			int l = rand() % (cfg.win_num - len + 1) + 1;
			elem_t e = 0;
			int ret = sk->query(l, l + len - 1, e);

		}
		 high_resolution_clock::time_point t2 = high_resolution_clock::now();
	  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		result[len] = time_span.count() / NN * 1e6;
	}
}
double test_single(hpwin_sketch<bool> *sk) {
	int len = cfg.qy_win;
	int fp = 0;
	for (int k=1; k<=cfg.test_elem_num; ++k) {
		int l = rand() % (cfg.win_num - len + 1) + 1;
		int r = l + len - 1;
		int e = rand() % 1000000;
		fp += sk->query(l,r,e);
	}
	return 1.0 * fp / cfg.test_elem_num;
}


double test_single_cnt(hpwin_sketch<int> *sk) {
	int j = 0;
	double fir = packets[0].ts;
	hashmap ecnt;
	int len = cfg.qy_win;
		double sum = 0, sumaae = 0;
		int tot = 0;
		for (int i = 1; i <= cfg.win_num; ++i) {
			//fprintf(stderr, "procing %d\n", i);
			for (int k = win_beg[i]; k < win_beg[i + 1]; ++k) 
				ecnt.add(packets[k].e, 1);

			if (i > len) {
				for (int k = win_beg[i - len]; k < win_beg[i - len + 1]; ++k) 
					ecnt.add(packets[k].e, -1);
			}

			int times = (cfg.test_elem_num + cfg.win_num - 1) / cfg.win_num;
			if (ecnt.empty()) times = 0;
			for (int k = 0; k < times; ++k) {
				auto pr = ecnt.sample();
				int ret = sk->query(i, i + len - 1, pr.first), real = pr.second;
				//printf("%d %d\n", ret, real);
				sum += fabs(ret - real) / real;
				sumaae += fabs(ret - real);
				tot++;
			}
		}
		
	return sum / tot;
}