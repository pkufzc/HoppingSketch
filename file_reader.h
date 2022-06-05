#pragma once

#include <cstdio>
#include <cassert>
#include <string>
#include "config.h"
#include "types.h"

class file_reader {
public:
	file_reader() : dataset(cfg.dataset), total_time(cfg.win_num * cfg.win_length) {
		if (dataset == "webpage") {
			unit_size = 16, ts_offset = 8, elen = 8;
			file_pattern = "/share/datasets/webdocs_timestamp/webdocs_timestamp%02d.dat";
			id_begin = 0; id_end = 0; id_step = 1;
		} else if (dataset == "zipf") {
			unit_size = 12, ts_offset = 4, elen = 4;
			file_pattern = "/share/datasets/zipf_timestamp/zipf_timestamp%03d.dat";
			id_begin = 3; id_end = 3; id_step = 3;
		} else if (dataset == "CAIDA") {
			unit_size = 21, ts_offset = 13, elen = 4;
			file_pattern = "/share/datasets/CAIDA2018/dataset/13%02d00.dat";
			id_begin = 0; id_end = 59; id_step = 1;
		} else assert(0);
		buf = new char[BUF_SIZE];

		cur = end = buf;
		id = id_begin;
		sprintf(file_name, file_pattern.c_str(), id);
		fprintf(stderr, "filereader: opening file %s\n", file_name);
		fp = fopen(file_name, "rb");
	}

	file_reader(const file_reader &) = delete;

	~file_reader() { fclose(fp); delete[] buf; }

	bool read(packet &pkt) {
		if (!_read(pkt)) {
			int len = fread(buf, 1, BUF_SIZE / unit_size * unit_size, fp);
			assert(len % unit_size == 0);
			if (len == 0) {
				if (id == id_end) return false;
				id += id_step;
				
				sprintf(file_name, file_pattern.c_str(), id);
				fprintf(stderr, "filereader: opening file %s\n", file_name);
				fclose(fp);
				fp = fopen(file_name, "rb");
				len = fread(buf, 1, BUF_SIZE / unit_size * unit_size, fp);
			}
			cur = buf; end = buf + len;
			_read(pkt);
		}
		return true;
	}

private:
	bool _read(packet &pkt) {
		int rl;
		if (cur >= end) return false;
		pkt.e = elen == 8 ? *((uint64_t*) cur) : *((uint32_t*) cur);
		pkt.ts = *((double*) (cur + ts_offset));
		cur += unit_size;
		if (dataset != "CAIDA") pkt.ts *= total_time;
		return true;
	}

	FILE *fp;
	const std::string dataset;
	const double total_time;
	int unit_size, ts_offset, elen;
	std::string file_pattern;
	int id_begin, id_end, id_step, id;

	static const int BUF_SIZE = 2e7;
	char *buf, file_name[100];
	char *cur, *end;
};