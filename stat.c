#include <stdint.h>

#include "stat.h"

int getavgms(struct keystat *stats){
	uint64_t sum_t = 0;
	for(int i = 1; i < stats->test_str_size; i++){
		sum_t += stats->timings[i] - stats->timings[i-1];
	}
	printw("%d", sum_t);
	return sum_t/stats->test_str_size;
}


void display_results(struct keystat *stats){
	struct window results_window;
	configure_window(&results_window);


	
	int avg_t = getavgms(stats);
	int t = 0;
	// TODO
	int wpm = 0,
		acc = 0,
		mstks = 0,
		mstks_fxd = 0;

	FILE *debug = fopen("/home/meetowl/program/keyvo/test.txt", "w");
	for(int i = 0; i < stats->test_str_size; i++){
		fprintf(debug, "%d, ", stats->timings[i]);
	}
	
	fclose(debug);
	
	print_results(&results_window, t,					\
				  avg_t, wpm, acc, mstks, mstks_fxd);
	getchar();
}
