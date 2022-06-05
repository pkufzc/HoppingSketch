#include "cmsketch.h"
#include "hpwin_sketch.h"

#include <cassert>

class hop_basic_cm : hpwin_sketch<int> {
public:
	hop_basic_cm(size_t memory, int size, int hf_num, int per = 1) : size(size), period(per) {
		size_t mem = memory / size;
		sk = new cmsketch*[size];
		lst = 0;
		counter = 0;
		for (int i = 0; i < size; ++i)
			sk[i] = new cmsketch(mem, hf_num);
	}
	~hop_basic_cm() {
		for (int i = 0; i < size; ++i)
			delete sk[i];
		delete sk;
	}
	int query(int win, elem_t e) const {
		assert(lst - size < win && win <= lst);
		return sk[win % size]->query(e);
	}
	int query(int l, int r, elem_t e) const {
		for (int i = l; i <= r; ++i) if (query(i, e)) return true;
		return false;
	}
	void new_win() {
		if (counter % period == 0) {
			lst++; sk[lst % size]->clear();
		}
		counter++;
	}
	void ins_latest(elem_t e, int delta = 1) { sk[lst % size]->add(e, 0, delta); }
private:
	cmsketch** sk;
	int size, lst, period;
	int counter;
};