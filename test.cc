#include "hpwin_sketch.h"

struct test_env_bf {
public:
	void test_wfpr(double* wfpr) {
		for (int len = 1; len <= cfg.ds_size; ++len) {
			double sum = 0; int cnt = 0;
			for (int l = cfg.test_size - cfg.ds_size + 1; l <= cfg.test_size - len + 1; ++l) {
				sum += test(l, l + len - 1) * wfpr(l, l + len - 1);
				cnt++;
			}
			wfpr[len] = sum / cnt;
		}
	}

	double test(int l, int r) {
		int fp = 0;
		for (int e = 1; e <= test_elem_num; ++e)
			if (sk->query(l, r, e))
				++fp;
		return 1.0 * fp / test_elem_num;
	}

private:	
	hpwin_sketch<bool> *sk;
	int test_elem_num;
};