#pragma once
#include <string>

struct config {
	std::string dataset;
	int win_num;
	double win_length;
	int qy_win;
	size_t memory;
	int test_elem_num;
};

config cfg;

void configure(std::string dataset, int win_num, double win_length, int qy_win, size_t memory, int test_elem_num) {
	cfg = (config) { dataset, win_num, win_length, qy_win, memory, test_elem_num };
}