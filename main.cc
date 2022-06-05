#include "test.h"
#include "file_reader.h"
#include "pbf.h"
#include "hpbsc_csk.h"
#include "hopping.h"
#include "hop_basic.h"
#include "hpwin_sketch.h"
#include "dct.h"
#include "idea.h"
#include "env.h"
#include "hpcsk.h"
#include "hpcm.h"
#include "hpbsc_cm.h"
#include "hpbsc_cu.h"
#include <unordered_set>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <chrono>

config last_cfg;

std::unordered_map<elem_t, int> uu;

template<typename T>
void build_sketch(hpwin_sketch<T> *sketch) {
	int j = 0;
	double fir = packets[0].ts;
	long long tot = 0;
	for (int i = 0; i < _MOD; ++i)
		latest[i] = std::make_pair(0, -1);
	for (int i = 1; i <= cfg.win_num; ++i) {

		if (i % 1000 == 0) fprintf(stderr, "building %d\n", i);
		for (sketch->new_win(); 
			j < packet_num && packets[j].ts - fir < i * cfg.win_length; ++j) {
			elem_t e = packets[j].e;
			sketch->ins_latest(e, 1);
			/*std::pair<elem_t, int> &pr = latest[e % _MOD];
			if (pr.first != e) {
				if (pr.second != -1) sketch->ins_latest(pr.first, pr.second);
				pr = std::make_pair(e, 1);
			} else pr.second++;*/
			//if (i>2) assert(sketch->query(i-2,i,packets[j].e));
		}

		/*for (int k = 0; k < _MOD; ++k)
			if (latest[k].second != -1) {
				sketch->ins_latest(latest[k].first, latest[k].second);
				latest[k] = std::make_pair(0, -1);
			}*/
	}
	/*for (std::pair<elem_t, int> i : uu) {
		tot += (i.second + 7) / 8 * 24;
	}*/
	//tot += uu.size()*sizeof(elem_t)
	
}

void prepare_data() {
	file_reader fr;
	std::vector<packet> pkts;
	packet pkt; fr.read(pkt); pkts.push_back(pkt);
	double fir = pkt.ts;
	while (fr.read(pkt) && pkt.ts - fir < cfg.win_length * cfg.win_num) pkts.push_back(pkt);
	if (packets) delete[] packets;
	packets = new packet[packet_num = pkts.size()];
	for (int i = 0; i < pkts.size(); ++i)
		packets[i] = pkts[i];

	win_beg = new int[cfg.win_num + 2];
	int j = 0;
	for (int i = 1; i <= cfg.win_num; ++i) 
		for (win_beg[i] = j; j < packet_num && packets[j].ts - fir < i * cfg.win_length; ++j);
	win_beg[cfg.win_num + 1] = packet_num;
	//printf("%ld\n", (size_t) packet_num * (sizeof(elem_t) + sizeof(double)));
}

void run_test() {
	;
}

void test_mem_bf(int memory) {
	const int k = 16; // 128
	const int k2 = 13;
	cfg.memory = memory;
	//pbf1<hopping_bf>* pbf = new pbf1<hopping_bf>(cfg.memory, k2);
	auto _pbf = new pbf1<hop_basic_bf>(cfg.memory, k2);
	hpwin_sketch<bool>* ds = (hpwin_sketch<bool>*) _pbf;
	build_sketch(ds);

	printf("%d,%.6f\n", memory>>20, test_single(ds));
}


void test_mem_cnt(size_t memory, int k2) {
	cfg.memory = memory;
	//fprintf(stderr, "??\n");
	//pbf1<hopping_bf>* pbf = new pbf1<hopping_bf>(cfg.memory, k2);
	auto pcm1 = new pcnt1<hopping_cnt>(cfg.memory, k2);
	hpwin_sketch<int>* ds = (hpwin_sketch<int>*) pcm1;
	build_sketch(ds);

	printf("%lu,%.6f\n", memory>>20, test_single_cnt(ds));
	delete pcm1;
}

void mem_test() {
	const int k = 16; // 128
	const int k2 = 13;
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(500), 1e5);
	configure("zipf", 1<<k, 60. / (1<<k), 1<<k2, MB(200), 1e5);
	prepare_data();
	printf("Memory(MB), FPR\n");
	//for (int i=400; i<=1000;i+=100)
	for (int i=200; i<=500;i+=50)
		test_mem_bf(MB(i));
}

void mem_test_cnt() {
	const int k = 16; // 128
	const int k2 = 13;
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(6000), 3e5);
	configure("webpage", 1<<k, 60. / (1<<k), 1<<k2, MB(1500), 1e5);
	prepare_data();
	printf("Memory(MB), ARE\n");
	for (int i=1000; i<=4000;i+=500)
		test_mem_cnt(MB(i), k2);
}

void test_win_num() {
	const int k = 16; // 128
	const int k2 = 13;
	configure("zipf", 1<<k, 60. / (1<<k), 1<<k2, MB(200), 1e5);
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(500), 1e5);
	
	prepare_data();
	/*hopping_bf* hbf = new hopping_bf(cfg.memory, cfg.win_num, 2);
	hop_basic_bf *bbf = new hop_basic_bf(cfg.memory, cfg.win_num, 2);
	pbf1<hopping_bf>* pbf = new pbf1<hopping_bf>(cfg.memory, k2);
	pbf1<hop_basic_bf>* _pbf = new pbf1<hop_basic_bf>(cfg.memory, k2);
	pbf2 *pbf2_ = new pbf2(cfg.memory, k);*/
	pbf1<hopping_bf>* pbf = new pbf1<hopping_bf>(cfg.memory, k2);
	pbf1<hop_basic_bf>* _pbf = new pbf1<hop_basic_bf>(cfg.memory, k2);
	//mpbf* _a = new mpbf(cfg.memory, 2, 1<<k);
	
	
	//pbf2 *pbf2_ = new pbf2(cfg.memory, k);
	//mpbf *_mpbf = new mpbf(cfg.memory, 2, 1<<k);
	//dct *tt = new dct(cfg.memory, cfg.test_win);
	hpwin_sketch<bool>* ds = (hpwin_sketch<bool>*) _pbf;
	build_sketch(ds);
	double *result = new double[cfg.qy_win+1];
	test_wfpr(ds, result);
	for (int i = 1; i <= cfg.qy_win; i <<= 1) {
		printf("%d %.8f\n", i, result[i]);
	}
}

void test_win_num_count() {
	const int k = 16; // 128
	const int k2 = 13;
	configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(6000), 3e5);
	//configure("webpage", 1<<k, 60.0 / (1<<k), 1<<k2, MB(1500), 3e5);
	prepare_data();
	/*hopping_bf* hbf = new hopping_bf(cfg.memory, cfg.win_num, 2);
	hop_basic_bf *bbf = new hop_basic_bf(cfg.memory, cfg.win_num, 2);
	pbf1<hopping_bf>* pbf = new pbf1<hopping_bf>(cfg.memory, k2);
	pbf1<hop_basic_bf>* _pbf = new pbf1<hop_basic_bf>(cfg.memory, k2);
	pbf2 *pbf2_ = new pbf2(cfg.memory, k);*/
	//auto flat1 = new hpcsk(cfg.memory, 2, cfg.win_num);
	//auto flat2 = new hpcm(cfg.memory, 2, cfg.win_num);
	auto pcm1 = new pcnt1<hopping_cu>(cfg.memory, k2);
	//auto pcm2 = new pcnt1<hop_basic_cu>(cfg.memory, k2);

	//auto flat1 = new hpcm(cfg.memory, 2, cfg.win_num);
	
	//pbf2 *pbf2_ = new pbf2(cfg.memory, k);
	//mpbf *_mpbf = new mpbf(cfg.memory, 2, 1<<k);
	//dct *tt = new dct(cfg.memory, cfg.test_win);
	hpwin_sketch<int>* ds = (hpwin_sketch<int>*) pcm1;
	build_sketch(ds);
	double *result = new double[cfg.qy_win+1];
	double *aae = new double[cfg.qy_win+1];
	test_are(ds, result, aae);
	printf("win num,ARE,AAE\n");
	for (int i = 1; i <= cfg.qy_win; i <<= 1) {
		printf("%d,%.8f,%.8f\n", i, result[i],aae[i]);
	}
}

void test_speed(int k) {
	using namespace std::chrono;
	int k2 = k;
	configure("zipf", 1<<k, 60. / (1<<k), 1<<k2, MB(200), 1e5);
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(500), 1e5);
	prepare_data();
	auto pcm1 = new pcnt1<hopping_cm>(cfg.memory, k2);
	auto pcm2 = new pcnt1<hop_basic_cm>(cfg.memory, k2);

	//auto flat1 = new hpcm(cfg.memory, 2, cfg.win_num);
	
	//pbf2 *pbf2_ = new pbf2(cfg.memory, k);
	//mpbf *_mpbf = new mpbf(cfg.memory, 2, 1<<k);
	//dct *tt = new dct(cfg.memory, cfg.test_win);
	hpwin_sketch<int>* ds = (hpwin_sketch<int>*) pcm1;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
		
	build_sketch(ds);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	printf("%d,%.10f\n", 1<<k, time_span.count() / packet_num * 1e6);
}

void test_qspeed() {
	using namespace std::chrono;
	const int k = 16; // 128
	const int k2 = 13;
	configure("zipf", 1<<k, 60. / (1<<k), 1<<k2, MB(200), 1e5);
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(500), 1e5);
	prepare_data();
	auto pcm1 = new pcnt1<hopping_cm>(cfg.memory, k2);
	auto pcm2 = new pcnt1<hop_basic_cm>(cfg.memory, k2);

	//auto flat1 = new hpcm(cfg.memory, 2, cfg.win_num);
	
	//pbf2 *pbf2_ = new pbf2(cfg.memory, k);
	//mpbf *_mpbf = new mpbf(cfg.memory, 2, 1<<k);
	//dct *tt = new dct(cfg.memory, cfg.test_win);
	hpwin_sketch<int>* ds = (hpwin_sketch<int>*) pcm1;
		
	build_sketch(ds);
	
	double *result = new double[cfg.qy_win+1];
	test_cntspeed(ds, result);
	printf("query length, query time(ms)\n");
	for (int i = 1; i <= cfg.qy_win; i <<= 1) {
		printf("%d,%.8f\n", i, result[i]);
	}
}

int main() {
	srand(1214);
	/**/
	//test_win_num_count();
	/*printf("max query length, insert time(ms)\n");
	for (int k=6; k<=13; ++k)
		test_speed(k);*/
	//	test_qspeed();
	mem_test_cnt();
	return 0;
}