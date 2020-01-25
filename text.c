#include <ncurses.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#include "window.h"
#include "text.h"
#include "stat.h"

void set_fonts(int theme){ // TODO: Allow different themes
	switch(theme){
	case 0: // Monochrome monitor
		init_pair(1, COLOR_BLACK, COLOR_WHITE); /* Wrong letter typed */
		init_pair(2, COLOR_BLACK, COLOR_WHITE); /* Wrong space / newline */ 
		init_pair(3, COLOR_WHITE, COLOR_BLACK); /* Correct letter typed */
		init_pair(4, COLOR_WHITE, COLOR_WHITE); /* Backspaced to letter */
		init_pair(5, COLOR_WHITE, COLOR_WHITE); /* Backspaced to whitespace */
		break;
	case 1:
		init_pair(1, COLOR_RED, COLOR_BLACK); /* Wrong letter typed */
		init_pair(2, COLOR_RED, COLOR_RED); /* Wrong whitespace */ 
		init_pair(3, COLOR_GREEN, COLOR_BLACK); /* Correct letter typed */
		init_pair(4, COLOR_YELLOW, COLOR_BLACK); /* Backspaced to letter */
		init_pair(5, COLOR_YELLOW, COLOR_YELLOW); /* Backspaced to whitespace */
		break;
	}
}

void print_option_menu(struct window *op_window, char prev_slct){
	// Top of screen
	const char welcome[] = "Welcome to Keyvo!";
	const char directive[] = "Pick an option:";
	const char opt1[] = "1. Generated string";
	const char opt2[] = "2. Word mash";
	const char opt3[] = "3. Paragraph";

	// Bottom of screen
	// (Previous selection entry is defined below)
	const char pref[] = "o: Preferences";
	const char quit[] = "q: Quit application";

	WINDOW * win = op_window->window_p;
	int win_c = op_window->max_columns;
	int win_r = op_window->max_rows;
	
	mvwprintw(win, 0, (win_c - strlen(welcome))/2, "%s", welcome);
	mvwprintw(win, 2, (win_c - strlen(directive))/2, "%s", directive);
	mvwprintw(win, 3, (win_c - strlen(opt1))/2, "%s", opt1);
	mvwprintw(win, 4, (win_c - strlen(opt2))/2, "%s", opt2);
	mvwprintw(win, 5, (win_c - strlen(opt3))/2, "%s", opt3);

	mvwprintw(win, (win_r-2), (win_c - strlen(pref))/2, "%s", pref);
	mvwprintw(win, (win_r-1), (win_c - strlen(quit))/2, "%s", quit);

	// If we didn't select anything prior, don't print previous selection text
	if (prev_slct != 0) {
		char *pselect = malloc(27);
		sprintf(pselect, "p: Previous selection (%c)", prev_slct + '0');
		mvwprintw(win, (win_r-3), (win_c - strlen(pselect))/2, "%s", pselect);
		free(pselect);
	}
}

void print_gen_menu(struct window *op_window){
	const char subname[] = "Generated string";
	const char directive[] = "Pick an option:";
	const char opt1[] = "1. Letters only";
	const char opt2[] = "2. Letters + Numbers";
	const char opt3[] = "3. Letters + Numbers + Symbols";

	WINDOW *win = op_window->window_p;
	int win_l=op_window->max_columns;
	
	mvwprintw(win, 0, (win_l - strlen(subname))/2, "%s", subname);
	mvwprintw(win, 2, (win_l - strlen(directive))/2, "%s", directive);
	mvwprintw(win, 3, (win_l - strlen(opt1))/2, "%s", opt1);
	mvwprintw(win, 4, (win_l - strlen(opt2))/2, "%s", opt2);
	mvwprintw(win, 5, (win_l - strlen(opt3))/2, "%s", opt3);
}

void print_typing_string(struct window *op_window){
	const char line_0[] = "Keyvo (by meetowl)";
	const char line_1[] = "Type the characters.";
	const char line_2[] = "Ctrl-X to return to main menu";

	WINDOW *win = op_window->window_p;
	int win_l=op_window->max_columns;
	op_window->init_cursor_row = 4;
	op_window->init_cursor_col = 0;

	mvwprintw(win, 0, (win_l - strlen(line_0))/2, "%s", line_0);
	mvwprintw(win, 1, (win_l - strlen(line_1))/2, "%s", line_1);
	mvwprintw(win, 2, (win_l - strlen(line_2))/2, "%s", line_2);
}

void print_test_string(struct window *op_window, char * str){
	wmove(op_window->window_p, op_window->init_cursor_row, 0);
	wprintw(op_window->window_p, "%s", str);
}

void redraw_char(struct window *op_window, const char out,	\
				 const char in){
	if (is_backspace(in)) {
		if (out == ' ') {
			waddch(op_window->window_p, out | REWRITE_HL);
		} else {
			waddch(op_window->window_p, out | REWRITE_L);
		}
	} else {
		if (out == in) {
			waddch(op_window->window_p, in | CORRECT_L);
		} else {
			if (out == ' ') {
				waddch(op_window->window_p, out | WRONG_HL);
			} else {
				waddch(op_window->window_p, out | WRONG_L);
			}
		}
	}
}

test_type_t generate_category_wrt_type(test_type_t type) {
	switch (type) {
	case CHAR_BOTH:
		return rand() % 2;
	case CHAR_NUM:
		return rand() % 3;
	case CHAR_SYMBOL:
		if (rand() % 2) return (rand() % 2);
		else  		    return (3);
	case NUM_SYMBOL:
		return rand() % 2 + 2;
	case CHAR_SYMBOL_NUM:
		return rand() % 4;
	default:
		return type;
	}
}

char generate_random_char_wrt_type(test_type_t type) {
	const char symbol_array[] = "~`!@#$%^&*(){[}]+=?/_-:;\\|\"'";
	switch(generate_category_wrt_type(type)){
	case CHAR_LOWER_CASE:
		return 97 + (rand() % 26);
	case CHAR_UPPER_CASE:
		return 65 + (rand() % 26);
	case NUM:
		return 48 + (rand() % 10);
	case SYMBOL:
		return symbol_array[rand() % (sizeof(symbol_array)-1)];
	default:
		return 0;
	}	
}
char *generate_random_string(test_type_t type){
	struct timeval rand_tv;
	gettimeofday(&rand_tv, 0);
	srand(rand_tv.tv_sec+rand_tv.tv_usec);
	
	char *rdm_str = malloc(TEST_STR_SIZE*sizeof(char)+1);
	int next_space = 3+(rand()%6);
	for(int i = 0; i < TEST_STR_SIZE-1; i++){
		rdm_str[i] = generate_random_char_wrt_type(type);
		if(i+1 == next_space){
			*(rdm_str+i+1) = ' ';
			next_space = i + 3 + (rand() % AVG_WORD_SIZE);
			i++;
		}
	}
	*(rdm_str+TEST_STR_SIZE-1) = ' ';
	*(rdm_str+TEST_STR_SIZE) = '\0';
	return rdm_str;
}

void print_results(struct window *op_window, int t, int avg_t, int wpm, \
				   double acc, double acc_fx, int mstks, int mstks_fx){
				   //				   struct mistakes *mlist){
	const char line0[] = "Test finished!";

	WINDOW *win = op_window->window_p;
	int win_l = op_window->max_columns;

	mvwprintw(win, 0, (win_l - strlen(line0))/2, "%s", line0);
	mvwprintw(win, 1, 0, "Avg. Time / character: %10dms", avg_t);
	mvwprintw(win, 2, 0, "Time taken:            %10d seconds", t);
	mvwprintw(win, 3, 0, "Words per minute:      %10d", wpm);
	mvwprintw(win, 4, 0, "Accuracy:              %10.2f%%", acc);
	mvwprintw(win, 5, 0, "Accuracy fixed:        %10.2f%%", acc_fx);
	mvwprintw(win, 6, 0, "Mistakes total:        %10d", mstks);
	mvwprintw(win, 7, 0, "Mistakes fixed:        %10d", mstks_fx);

	/* switch (mlist->maxindex) { */
	/* case -1: */
	/* 	// Make sure nothing weird happens */
	/* 	break; */
	/* case 0: */
	/* 	mvwprintw(win, 8, 0, "Most mistaken:     %10c:%d",	\ */
	/* 			  mlist->list[0]->correctc, mlist->list[0]->count); */
	/* 	break; */
	/* case 2: */
	/* 	mvwprintw(win, 8, 0, "Most mistaken:     %10c:%d;%10c:%d",	\ */
	/* 			  mlist->list[0]->correctc, mlist->list[0]->count, */
	/* 			  mlist->list[1]->correctc, mlist->list[1]->count); */
	/* 	break; */
	/* default: */
	/* 	mvwprintw(win, 8, 0, "Most mistaken:     %10c:%d; %c:%d; %c:%d;",	\ */
	/* 			  mlist->list[0]->correctc, mlist->list[0]->count, */
	/* 			  mlist->list[1]->correctc, mlist->list[1]->count, */
	/* 			  mlist->list[2]->correctc, mlist->list[2]->count); */

	wrefresh(op_window->window_p);
}

