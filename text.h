#ifndef TEXT_H_
#define TEXT_H_

#include "window.h"

#define TEST_STR_SIZE 256
#define AVG_WORD_SIZE 8

void set_fonts(int theme);

void print_option_menu(struct window *op_window, char prev_slct);

void print_typing_string(struct window *op_window);

void print_test_string(struct window *op_window, char * str);

void redraw_char(struct window *op_window, const char test_char,	\
				 const char in_correct);

char *generate_random_string(int gen_num);

void print_results(struct window *op_window, int t, int avg_t, int wpm, \
				   double acc, double acc_fx, int mstks, int mstks_fx);


#endif
