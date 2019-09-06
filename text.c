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
	const char line_0[] = "Welcome to Keyvo!";
	const char line_1[] = "by meetowl";
	const char line_2[] = "Pick an option:";
	const char line_3[] = "1. Letters + Spaces";
	const char line_4[] = "2. Letters + Numbers + Spaces";
	const char line_5[] = "3. Letters + Numbers + Symbols + Spaces";
	char *line_6 = malloc(27);
	sprintf(line_6, "p: Previous selection (%c)", prev_slct);
	const char line_7[] = "q: Quit application";

	WINDOW * win = op_window->window_p;
	int win_l = op_window->length;
	op_window->start_curs_y = 7;
	op_window->start_curs_x = (win_l - strlen(line_7))/2 + strlen(line_7) + 1;
	
	mvwprintw(win, 0, (win_l - strlen(line_0))/2, "%s", line_0);
	mvwprintw(win, 1, (win_l - strlen(line_1))/2, "%s", line_1);
	mvwprintw(win, 2, (win_l - strlen(line_2))/2, "%s", line_2);
	mvwprintw(win, 3, (win_l - strlen(line_3))/2, "%s", line_3);
	mvwprintw(win, 4, (win_l - strlen(line_4))/2, "%s", line_4);
	mvwprintw(win, 5, (win_l - strlen(line_5))/2, "%s", line_5);
	mvwprintw(win, 6, (win_l - strlen(line_6))/2, "%s", line_6);
	mvwprintw(win, 7, (win_l - strlen(line_7))/2, "%s", line_7);
}

void print_typing_string(struct window *op_window){
	const char line_0[] = "Keyvo (by meetowl)";
	const char line_1[] = "Type the characters.";
	const char line_2[] = "Ctrl-X to return to main menu";

	WINDOW *win = op_window->window_p;
	int win_l=op_window->length;
	op_window->start_curs_y = 4;
	op_window->start_curs_x = 0;

	mvwprintw(win, 0, (win_l - strlen(line_0))/2, "%s", line_0);
	mvwprintw(win, 1, (win_l - strlen(line_1))/2, "%s", line_1);
	mvwprintw(win, 2, (win_l - strlen(line_2))/2, "%s", line_2);
}

void print_test_string(struct window *op_window, char * str){
	wmove(op_window->window_p, op_window->start_curs_y, 0);
	wprintw(op_window->window_p, "%s", str);

	// This used to be how it worked, leaving here just in case.
	//	int i = 0;
	//	int pgstop = op_window->length*op_window->height;
	/* while(*(str+i) != '\0' || i <= pgstop-6){ */
	/* 	addch(*(str+i)); */
	/* 	waddch(op_window->window_p, *(str+i)); */
	/* 	i++; */
	/* } */
}

void redraw_char(struct window *op_window, const char test_char,	\
				 const char in_correct){
	switch(in_correct){
	case 0:
		if(test_char == ' '){
			waddch(op_window->window_p, test_char | WRONG_HL);
		}else{
			waddch(op_window->window_p, test_char | WRONG_L);
		}
		break;
	case 1:
		waddch(op_window->window_p, test_char | CORRECT_L);
		break;
	case 2:
		if(test_char == ' '){
			waddch(op_window->window_p, test_char | REWRITE_HL);		
		}else{
			waddch(op_window->window_p, test_char | REWRITE_L);
		}
		break;
	default:
		fprintf(stderr,											\
				"error: incorrect option (%d) passed to redraw_char().\n", \
				in_correct);
		exit_application(1);
		break;
	}
}

char *generate_random_string(int gen_num){
	const char char_array[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQq"	\
		"RrSsTtUuWwXxYyZz";
	const char number_array[] = "1234567890";
	const char symbol_array[] = "~`!@#$%^&*(){[}]+=?/_-:;\\|\"'";

	struct timeval rand_tv;
	gettimeofday(&rand_tv, 0);
	srand(rand_tv.tv_sec+rand_tv.tv_usec);
	
	char *rdm_str = malloc(TEST_STR_SIZE*sizeof(char)+1);
	int next_space = 3+(rand()%6);
	for(int i = 0; i < TEST_STR_SIZE-1; i++){
		switch(rand() % gen_num){
		case 0:
			// -1 the size, or '\0' will have a chance of appearing.
			*(rdm_str+i) = char_array[rand() % \
									  (sizeof(char_array)-1)];
			break;
		case 1:
			*(rdm_str+i) = number_array[rand() %					\
										(sizeof(number_array)-1)];

			break;
		case 2:
			*(rdm_str+i) = symbol_array[rand() % \
										(sizeof(symbol_array)-1)];
			break;
		}

		if(i+1 == next_space){
			*(rdm_str+i+1) = ' ';
			next_space = i + 3 + (rand() % 8);
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
	int win_l = op_window->length;

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

	show_window(op_window);
}

