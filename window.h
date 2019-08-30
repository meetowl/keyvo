#ifndef WINDOW_H_
#define WINDOW_H_ 

#include <ncurses.h>

#define WRONG_L COLOR_PAIR(1)
#define WRONG_HL COLOR_PAIR(2)
#define CORRECT_L COLOR_PAIR(3)
#define REWRITE_L COLOR_PAIR(4)
#define REWRITE_HL COLOR_PAIR(5)

/* Windows to be used */
struct window{
	WINDOW *window_p;
	WINDOW *border_p;
	int height;
	int	length;
	int start_curs_y;
	int start_curs_x;
};

void exit_application(int stat);

//int getscrollstate(struct window win);
	
int *get_std_height_length();

void create_w(struct window *op_window);

void create_w_brdr(struct window *op_window);

void start_curses();

int scaled_height(int scr_height);

int scaled_length(int scr_length);

void configure_window(struct window *op_window);

void show_window(struct window *op_window);

int getshownchar(struct window *op_window);
#endif
