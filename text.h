#ifndef TEXT_H_
#define TEXT_H_

#include "window.h"

#define TEST_STR_SIZE 128
#define AVG_WORD_SIZE 8


typedef enum {CHAR_LOWER_CASE = 0, CHAR_UPPER_CASE = 1, NUM = 2, SYMBOL = 3, \
			  CHAR_BOTH = 49, CHAR_NUM = 50, CHAR_SYMBOL = 8, NUM_SYMBOL = 7, \
			  CHAR_SYMBOL_NUM = 51, DICTIONARY_WORDS = 9				\
} test_type_t;

void set_fonts(int theme);

void print_option_menu(struct window *op_window, char prev_slct);

void print_typing_string(struct window *op_window);

void print_test_string(struct window *op_window, char * str);

void redraw_char(struct window *op_window, const char test_char,	\
				 const char in_correct);

char *generate_random_string(test_type_t type);

void print_results(struct window *op_window, int t, int avg_t, int wpm, \
				   double acc, double acc_fx, int mstks, int mstks_fx);
				   //		   struct mistakes *mlist);

void print_gen_menu(struct window *op_window);

#endif
