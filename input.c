#include <sys/time.h>

#include "chars.h"
#include "stat.h"
#include "text.h"


void backspace_character(struct window *op_window, char rechar){
	uint8_t y, x;
	// Macro that determines the position of cursor
	getyx(op_window->window_p, y, x);
	if (x == 0) {
		if (y != op_window->init_cursor_row) {
			wmove(op_window->window_p, y-1, op_window->max_columns-1);
			redraw_char(op_window, rechar, '\b');
			wmove(op_window->window_p, y-1, op_window->max_columns-1);
		}
	} else {
		wmove(op_window->window_p, y, x-1);
		redraw_char(op_window, rechar, '\b');
		wmove(op_window->window_p, y, x-1);
	}
}

int is_quit_key(char in){
	// Ctrl-Q keycode
	return (in == 24);
}

// Clears the string part of the typing window
void clear_text(struct window *win){
	wmove(win->window_p, win->init_cursor_row, win->init_cursor_col);
	for(int y = win->init_cursor_row; y <= win->max_rows; y++){
		for(int x = win->init_cursor_col; x <= win->max_columns; x++){
			waddch(win->window_p, ' ');
		}
	}
}


void scroll_text_down(struct keystat *stat, struct window *win, int p){
	clear_text(win);
	wmove(win->window_p, win->init_cursor_row, win->init_cursor_col);
	// Print the first formatted line
	for(int i = win->max_columns; i > 0; i--){
		redraw_char(win, (stat->test_str)[p-i],	(stat->char_input[p-i]));
		//					(stat->char_input[p-i] == *(stat->test_str+(p-i))));
	}
	
	wprintw(win->window_p, "%s", stat->test_str+p);
	wmove(win->window_p, win->init_cursor_row+1, win->init_cursor_col);
}

void scroll_text_up(struct keystat *stat, struct window *win, int p){
	clear_text(win);
	wmove(win->window_p, win->init_cursor_row, win->init_cursor_col);
	int i; 
	for(i = get_max_shown_char(win)-1; i > 0 ; i--){
		redraw_char(win, *(stat->test_str+p-i), (stat->char_input[p-i]));
		//					(stat->char_input[p-i] == *(stat->test_str+(p-i))));
	}
	redraw_char(win, *(stat->test_str+p-i),	'\b');
}

// The most important function in this program.
// Hence the length and verbosity
int test_string(struct keystat *stat){
	// Configure the window
	struct window typing_window;
	init_centre_window(&typing_window);
	print_typing_string(&typing_window);
	print_test_string(&typing_window, stat->test_str);
	wrefresh(typing_window.window_p);
	wmove(typing_window.window_p, typing_window.init_cursor_row, 0);

	// Set numbers for when we need to scroll back
	int scroll_at = get_max_shown_char(&typing_window);
	int scroll_back_at = 0;
	
	// Read the first character and initialize timings
	char char_in = getch();
	stat->char_input[0] = char_in;
	stat->entered_str[0] = char_in;
	redraw_char(&typing_window, *stat->test_str, char_in);

	// Everything to do with timing keypresses
	struct timeval now_tv;
	gettimeofday(&now_tv, 0);
	uint64_t start_time = (now_tv.tv_sec*1000) + (now_tv.tv_usec/1000);
	stat->timings[0] = 0;

	wrefresh(typing_window.window_p);

	unsigned i = 1;
	// Iterator for the full entered string
	unsigned entry_i = 1;

	while((stat->test_str)[i] != '\0'){
		char_in = getch();

		// Within the character input range
		if (char_in >= 32 && char_in <= 126) {
			redraw_char(&typing_window, (stat->test_str)[i], char_in);
						//						(char_in == (stat->test_str)[i]));

			// Get current time (in milliseconds) and record it
			gettimeofday(&now_tv, 0);
			stat->timings[i] = (now_tv.tv_sec*1000 + now_tv.tv_usec/1000)	\
				- start_time;

			// Update the entered string
			stat->char_input[i] = char_in;
			
			i++;
		} else if (is_backspace(char_in) && i > 0) {
			i--;
			backspace_character(&typing_window, (stat->test_str)[i]);
				
			// Don't mark as bs if correct initially
			if(stat->char_input[i] != (stat->test_str)[i]){ 
				stat->bs[i]++; 
			} 
		} else if (is_quit_key(char_in)) {
			return 1;
		}
		
		// Update the entire entered string
		stat->entered_str[entry_i] = char_in;
		entry_i++;

		if (i == scroll_at) {
			scroll_text_down(stat, &typing_window, i);
			scroll_at += get_max_shown_char(&typing_window);
			scroll_back_at += get_max_shown_char(&typing_window) -1 ; 
		} else if(i == scroll_back_at && i > 0) {
			scroll_text_up(stat, &typing_window, i);
			scroll_at -= get_max_shown_char(&typing_window);
			scroll_back_at -= get_max_shown_char(&typing_window) - 1;
		}
		wrefresh(typing_window.window_p);


	}
	return 0;
}

	/* switch(in_correct){ */
	/* case 0: */
	/* 	if(test_char == ' '){ */
	/* 		waddch(op_window->window_p, test_char | WRONG_HL); */
	/* 	}else{ */
	/* 		waddch(op_window->window_p, test_char | WRONG_L); */
	/* 	} */
	/* 	break; */
	/* case 1: */
	/* 	waddch(op_window->window_p, test_char | CORRECT_L); */
	/* 	break; */
	/* case 2: */
	/* 	// Special case when need to backspace */
	/* 	if(test_char == ' '){ */
	/* 		waddch(op_window->window_p, test_char | REWRITE_HL);		 */
	/* 	}else{ */
	/* 		waddch(op_window->window_p, test_char | REWRITE_L); */
	/* 	} */
	/* 	break; */
	/* default: */
	/* 	fprintf(stderr,											\ */
	/* 			"error: incorrect option (%d) passed to redraw_char().\n", \ */
	/* 			in_correct); */
	/* 	exit_application(1); */
	/* 	break; */
	/* } */
