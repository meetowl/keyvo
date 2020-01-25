#ifndef STAT_H_
#define STAT_H_

#include "text.h"

struct keystat{
	char *test_str;
	int test_str_size;
	char entered_str[TEST_STR_SIZE*4];
	int timings[TEST_STR_SIZE];
	char char_input[TEST_STR_SIZE];
	int bs[TEST_STR_SIZE];
};

void display_results(struct keystat *stats);

void init_keystat(struct keystat *stats);
#endif
