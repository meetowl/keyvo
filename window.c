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

int *get_std_height_length(){
	int *return_size = malloc(3*sizeof(int));
	getmaxyx(stdscr, *return_size, *(return_size+1));
	*(return_size+2) = '\0';
	return return_size;
}


/* Creates the window in the given window structure pointer */
void create_w(struct window *op_window){
	int *std_dim = get_std_height_length();
	int win_start_h = (*std_dim/2) - (op_window->height/2);
	int win_start_l = (*(std_dim+1)/2) - (op_window->length/2);
	op_window->window_p = newwin(op_window->height, op_window->length,	\
								 win_start_h, win_start_l);
}

/* Creates the window border in the given window structure pointer */
void create_w_brdr(struct window *op_window){
	int *std_dim = get_std_height_length();
	
	int border_h = op_window->height+3;
	int border_l = op_window->length+3;
	int border_start_h = *std_dim/2 - (op_window->height/2) - 1;
	int border_start_l = *(std_dim+1)/2 - op_window->length/2 - 1;

	op_window->border_p = newwin(border_h, border_l,				\
								 border_start_h, border_start_l);
	wborder(op_window->border_p, '|', '|', '_', '_', ' ', ' ', ' ', ' ');
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


int scaled_height(int scr_height){
	if(scr_height < 8 ){
		return -1;
	}
	if(scr_height < 15){
		return scr_height;
	}
	if(scr_height < 25){
		return (int)((double)scr_height * 0.75);
	}
	return scr_height / 2;
}

int scaled_length(int scr_length){
	if(scr_length < 40){
		return -1;
	}
	if(scr_length < 60){
		return scr_length;
	}
	if(scr_length < 100){
		return (int)((double)scr_length * 0.75);
	}
	return scr_length / 2;
}

void configure_window(struct window *op_window){
	int *std_dim = get_std_height_length();
	int wdw_h = scaled_height(*std_dim),		\
		wdw_l = scaled_length(*(std_dim+1));

	// Makes sure screen is sufficient size
	if(wdw_h == -1 || wdw_l == -1){
		fatal_exit_w_msg("terminal requirements: >= 40 columns and >= 8 lines");
	}

	// Set the struct's parameters
	op_window ->length = wdw_l;
	op_window->height = wdw_h;
	create_w(op_window);
	create_w_brdr(op_window);
}

void show_window(struct window *op_window){
	refresh();
	wrefresh(op_window->border_p);
	wrefresh(op_window->window_p);
	refresh();
}

// Get the total number of characters we can fit on screen
int getshownchar(struct window *win){
	return win->length * (win->height - win->start_curs_y);
}

