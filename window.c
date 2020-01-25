#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

#include "text.h"

void fatal_exit_w_msg(const char* msg){
	endwin();
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void exit_application(int stat){
	endwin();
	exit(stat);
}

void start_curses(){
	initscr(); // Initialize curses
	if(has_colors() == TRUE){
		start_color();
		set_fonts(1);
	}else{
		set_fonts(0);
	}

	noecho(); /* doesn't print what user types */
	cbreak(); /* reads input as soon as key is pressed, allows ctrl-c */
	keypad(stdscr, TRUE); /* allows thing like function keys */

}


int scaled_rows(int screen_rows){
	if(screen_rows < 8 ){
		return -1;
	}
	if(screen_rows < 15){
		return screen_rows;
	}
	if(screen_rows < 25){
		return (int)((double)screen_rows * 0.75);
	}
	return screen_rows / 2;
}

int scaled_columns(int screen_columns){
	if(screen_columns < 40){
		return -1;
	}
	
	if(screen_columns < 60){
		return screen_columns;
	}
	
	if(screen_columns < 100){
		return (int)((double)screen_columns * 0.75);
	}
	
	return screen_columns / 2;
}

void draw_box_around_win(struct window *win){
	WINDOW *box = newwin(win->max_rows+2, win->max_columns+2, win->row-1, win->col-1);
	box(box, '|', '_');
	wrefresh(box);
	delwin(box);
	box = 0;
}

void configure_centre_window(struct window *op_window){
	int std_row = 0;
	int std_col = 0;
	getmaxyx(stdscr, std_row, std_col);

	op_window->max_rows = scaled_rows(std_row);
	op_window->max_columns = scaled_columns(std_col);
	
	if(op_window->max_rows == -1 || op_window->max_columns == -1){
		fatal_exit_w_msg("terminal requirements: >= 40 columns and >= 8 lines");
	}
	
	op_window->row = (std_row/2) - (op_window->max_rows/2);
	op_window->col = (std_col/2) - (op_window->max_columns/2);
	
	op_window->window_p = newwin(op_window->max_rows, op_window->max_columns,	\
								 op_window->row, op_window->col);
	
}

// Get the total number of characters we can fit on screen
int getshownchar(struct window *win){
	return win->max_columns * (win->max_rows - win->init_cursor_row);
}

