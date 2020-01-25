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


void init_keystat(struct keystat *stats) {
	// Zero out backspace array
	for(int i = 0; i < sizeof(stats->bs); i++){
		stats->bs[i] = 0;
	}
}
int get_avg_ms(struct keystat *stats){
	uint64_t sum_t = 0;
	for(int i = 1; i < stats->test_str_size; i++){
		sum_t += stats->timings[i] - stats->timings[i-1];
	}
	return sum_t/stats->test_str_size;
}

int get_words_per_minute(int strlen, int avgms){
	int ttime = avgms * strlen;
	double wpms = ((double)ttime)/((double)strlen/(double)AVG_WORD_SIZE);
	double wps = 1000.0/wpms;
	int wpm = wps*60;
	return wpm;
}

int get_total_mistakes_wrt_chars(struct keystat *stats){
	int mistake_total = 0;
	for (int i = 0; i < stats->test_str_size; i++){
		if (stats->char_input[i] != *(stats->test_str+i)){
			mistake_total++;
		}
	}
	return mistake_total;	
}

int get_total_mistakes_wrt_backspace(struct keystat *stats){
	int mistaket = 0;
	for (int i = 0; i < stats->test_str_size; i++){
		mistaket += stats->bs[i];
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

int insert(char c, struct mistakes *mistake_list){
	if (mistake_list->maxindex+1 < mistake_list->size) {
		mistake_list->maxindex++;
		struct mistake *tmp = mistake_list->list[mistake_list->maxindex];
		tmp->correctc = c;
		tmp->count = 1;
		return 0;
	}
	// Not enough space in the array
	return -1;
}

void insert_and_sort(char c, struct mistakes *mistake_list){
	int exitcode = insert(c, mistake_list);
	if (exitcode < 0){
		fatal_exit_w_msg("mistake array size is too small!");
		return;
	}

	// insertion sort
	int j = mistake_list->maxindex;
	while (j > 0 &&														\
		   mistake_list->list[j-1]->correctc >							\
		   mistake_list->list[j]->correctc) {
		struct mistake *tmp = mistake_list->list[j];
		mistake_list->list[j] = mistake_list->list[j-1];
		mistake_list->list[j-1] = tmp;
		j--;
	}
}

void add_mistake(char c, struct mistakes *mistake_list){
	int pos = binaryfind(c, mistake_list);
	if (pos >= 0) { 
		mistake_list->list[pos]->count++;
	} else { 
		insert_and_sort(c, mistake_list);
	}
}

struct mistakes *init_mistake_list(int size){
	struct mistakes *mistake_list = malloc(sizeof(struct mistakes));
	mistake_list->maxindex = -1;
	mistake_list->size = size;
	for (int i = 0; i < size; i++) {
		mistake_list->list[i] = malloc(sizeof(struct mistake));
	}
	return mistake_list;
}
                                                                                
void sort_by_count(struct mistakes *mistake_list){
	for (int i = 1; i <= mistake_list->maxindex; i++) {
		int j = i;
		while (j > 0 &&							\
			   mistake_list->list[j-1]->count < mistake_list->list[j]->count) {
			printw("yeah");
			struct mistake *tmp = mistake_list->list[j];
			mistake_list->list[j] = mistake_list->list[j-1];
			mistake_list->list[j-1] = tmp;
			j--;
		}
	}
}

struct mistakes *get_error_chars(struct keystat *stats){
	struct mistakes *mistake_list = init_mistake_list(LIST_SIZE);
	int entered_i = 0;
	int str_i = 0;

	while (stats->entered_str[entered_i] != '\0'){
		if (is_backspace(stats->entered_str[entered_i])) {
	 		str_i--;
		} else if (stats->entered_str[entered_i] != stats->test_str[str_i]) {
			add_mistake(stats->test_str[str_i], mistake_list);
			str_i++;
		} else {
			str_i++;
		}
		entered_i++;
	}
	sort_by_count(mistake_list);
	return mistake_list;
}

void display_results(struct keystat *stats){
	struct window results_window;
	init_centre_window(&results_window);
	
	int avg_time = get_avg_ms(stats);
	int time = stats->timings[stats->test_str_size-1]/1000;
	int words_per_minute = get_words_per_minute(stats->test_str_size, avg_time);
	int mistakes_fixed = get_total_mistakes_wrt_backspace(stats);
	int mistakes_remaining =  get_total_mistakes_wrt_chars(stats);
	int mistakes_all = mistakes_remaining + mistakes_fixed;
	double accuracy =													\
		((double)(stats->test_str_size - mistakes_all)) /				\
		((double)stats->test_str_size);
	double accuracy_fixed =												\
		((double)(stats->test_str_size - mistakes_remaining) /			\
		 ((double) (stats->test_str_size)));
	accuracy *= 100;
	accuracy_fixed *= 100;
	struct mistakes *mistake_list = get_error_chars(stats);
	// TODO: screen for mistakes
	print_results(&results_window, time, avg_time, words_per_minute, \
				  accuracy, accuracy_fixed, mistakes_all, mistakes_fixed);

	// Yes, I know this is awful, I will fix this later with a whole different representation.
	switch (mistake_list->maxindex) {
	case -1:
		// Make sure nothing weird happens
		break;
	case 0:
		mvwprintw(results_window.window_p, 8, 0, "Most mistaken:     %10c:%d",	\
				  mistake_list->list[0]->correctc, mistake_list->list[0]->count);
		break;
	case 2:
		mvwprintw(results_window.window_p, 8, 0, "Most mistaken:     %10c:%d;%10c:%d",	\
				  mistake_list->list[0]->correctc, mistake_list->list[0]->count,		\
				  mistake_list->list[1]->correctc, mistake_list->list[1]->count);
		break;
	default:
		mvwprintw(results_window.window_p, 8, 0, "Most mistaken:     %10c:%d; %c:%d; %c:%d;",	\
				  mistake_list->list[0]->correctc, mistake_list->list[0]->count,		\
				  mistake_list->list[1]->correctc, mistake_list->list[1]->count,		\
				  mistake_list->list[2]->correctc, mistake_list->list[2]->count);
		break;
	}

	wrefresh(results_window.window_p);
	getchar();
}
