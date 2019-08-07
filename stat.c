#include <stdint.h>
#include <math.h>

#include "stat.h"

int getavgms(struct keystat *stats){
	uint64_t sum_t = 0;
	for(int i = 1; i < stats->test_str_size; i++){
		sum_t += stats->timings[i] - stats->timings[i-1];
	}
	return sum_t/stats->test_str_size;
}

int getwpm(int strlen, int avgms){
	int ttime = avgms * strlen;
	double wpms = ((double)ttime)/((double)strlen/(double)AVG_WORD_SIZE);
	double wps = 1000.0/wpms;
	int wpm = wps*60;
	return wpm;
}

int getmistakes(struct keystat *stat){
	int mistaket = 0;
	for (int i = 0; i < stat->test_str_size; i++){
		if (stat->bs[i] ||									\
			stat->char_input[i] != *(stat->test_str+i)){
			mistaket++;
		}
	}
	return mistaket;
}
int getcharmstk(struct keystat *stat){
	int mistaket = 0;
	for (int i = 0; i < stat->test_str_size; i++){
		if (stat->char_input[i] != *(stat->test_str+i)){
			mistaket++;
		}
	}
	return mistaket;	
}
int getbsmstk(struct keystat *stat){
	int mistaket = 0;
	for (int i = 0; i < stat->test_str_size; i++){
		if (stat->bs[i]){
			mistaket += stat->bs[i];
		}
	}
	return mistaket;
}

void display_results(struct keystat *stats){
	struct window results_window;
	configure_window(&results_window);


	
	int avg_t = getavgms(stats);
	int t = stats->timings[stats->test_str_size-1]/1000;
	int wpm = getwpm(stats->test_str_size, avg_t);
	int mstk = getcharmstk(stats) + getbsmstk(stats);
	int mstk_fxd = getbsmstk(stats);
	double acc = ((double)(stats->test_str_size-mstk))/((double)stats->test_str_size);
	double acc_fxd = ((double)(stats->test_str_size-getcharmstk(stats))) / \
		((double) (stats->test_str_size));
	acc *= 100;
	acc_fxd *= 100;

	
		

	/* FILE *debug = fopen("/home/meetowl/program/keyvo/test.txt", "w"); */
	/* for(int i = 0; i < stats->test_str_size; i++){ */
	/* 	fprintf(debug, "%d, ", stats->timings[i]); */
	/* } */
	
	/* fclose(debug); */
	
	print_results(&results_window, t, avg_t, wpm, acc, acc_fxd, mstk, mstk_fxd);
	getchar();
}
