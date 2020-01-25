#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "stat.h"
#include "window.h"
#include "chars.h"

#define LIST_SIZE 512
struct mistake{
	char correctc;
	char enteredc[LIST_SIZE];
	int count;
	int maxenteredindex;
};

struct mistakes{
	struct mistake *list[LIST_SIZE];
	int maxindex;
	int size;
};


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


int binaryfind(char c, struct mistakes *mlist){
	if (mlist->maxindex < 0){
		return -1;
	}
	
	int l = 0,									\
		r = mlist->maxindex;
	while (l <= r) {
		int m = (l+r)/2;
		if (mlist->list[m]->correctc < c ){
			l = m+1;
		} else if (mlist->list[m]->correctc > c){
			r = m-1;
		} else if (mlist->list[m]->correctc == c){
			return m;
		}
	}
	return -1;
}

int insert(char c, struct mistakes *mlist){
	if (mlist->maxindex+1 < mlist->size) {
		mlist->maxindex++;
		struct mistake *tmp = mlist->list[mlist->maxindex];
		tmp->correctc = c;
		tmp->count = 1;
		return 0;
	}
	// Not enough space in the array
	return -1;
}

void insertandsort(char c, struct mistakes *mlist){
	int exitcode = insert(c, mlist);
	if (exitcode < 0){
		fatal_exit_w_msg("mistake array size is too small!");
		return;
	}

	// Second loop of insertion sort
	int j = mlist->maxindex;
	while (j > 0 && mlist->list[j-1]->correctc > mlist->list[j]->correctc) {
		struct mistake *tmp = mlist->list[j];
		mlist->list[j] = mlist->list[j-1];
		mlist->list[j-1] = tmp;
		j--;
	}
}

void addmistake(char c, struct mistakes *mlist){
	int pos = binaryfind(c, mlist);
	if (pos >= 0) {
		mlist->list[pos]->count++;
	} else {
		insertandsort(c, mlist);
	}
}

struct mistakes *initmistakes(int size){
	struct mistakes *mlist = malloc(sizeof(struct mistakes));
	mlist->maxindex = -1;
	mlist->size = size;
	for (int i = 0; i < size; i++) {
		mlist->list[i] = malloc(sizeof(struct mistake));
	}
	return mlist;
}

void sortbycount(struct mistakes *mlist){
	for (int i = 1; i <= mlist->maxindex; i++) {
		int j = i;
		while (j > 0 &&							\
			   mlist->list[j-1]->count < mlist->list[j]->count) {
			struct mistake *tmp = mlist->list[j];
			mlist->list[j] = mlist->list[j-1];
			mlist->list[j-1] = tmp;
			j--;
		}
	}
}

struct mistakes *getwrongc(struct keystat *stats){
	struct mistakes *mlist = initmistakes(LIST_SIZE);
	int enti = 0;
	int stri = 0;

	while (stats->entered_str[enti] != '\0'){
		if (is_backspace(stats->entered_str[enti])) {
			stri--;
		} else if (stats->entered_str[enti] != stats->test_str[stri]) {
			addmistake(stats->test_str[stri], mlist);
			stri++;
		} else {
			stri++;
		}
		enti++;
	}
	sortbycount(mlist);
	return mlist;
}

void display_results(struct keystat *stats){
	struct window results_window;
	configure_centre_window(&results_window);
	
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
	struct mistakes *mlist = getwrongc(stats);
	// TODO: screen for mistakes
	print_results(&results_window, t, avg_t, wpm, acc, acc_fxd, mstk, mstk_fxd);

	// Yes, I know this is awful, I will fix this later with a whole different representation.
	switch (mlist->maxindex) {
	case -1:
		// Make sure nothing weird happens
		break;
	case 0:
		mvwprintw(results_window.window_p, 8, 0, "Most mistaken:     %10c:%d",	\
				  mlist->list[0]->correctc, mlist->list[0]->count);
		break;
	case 2:
		mvwprintw(results_window.window_p, 8, 0, "Most mistaken:     %10c:%d;%10c:%d",	\
				  mlist->list[0]->correctc, mlist->list[0]->count,		\
				  mlist->list[1]->correctc, mlist->list[1]->count);
		break;
	default:
		mvwprintw(results_window.window_p, 8, 0, "Most mistaken:     %10c:%d; %c:%d; %c:%d;",	\
				  mlist->list[0]->correctc, mlist->list[0]->count,		\
				  mlist->list[1]->correctc, mlist->list[1]->count,		\
				  mlist->list[2]->correctc, mlist->list[2]->count);
		break;
	}
	wrefresh(results_window.window_p);

	getchar();
}
