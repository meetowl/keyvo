#include <sys/time.h>

#include "stat.h"
#include "text.h"

void backspace_character(struct window *op_window, char rechar){
	uint8_t y, x;
	// Macro that determines the position of cursor
	getyx(op_window->window_p, y, x);
	if(x == 0){
		if(y != op_window->start_curs_y){
			wmove(op_window->window_p, y-1, op_window->length-1);
			redraw_char(op_window, rechar, 2);
			wmove(op_window->window_p, y-1, op_window->length-1);
		}
	}else{
		wmove(op_window->window_p, y, x-1);
		redraw_char(op_window, rechar, 2);
		wmove(op_window->window_p, y, x-1);
	}
}

int isbackspace(char in){
	// Most backspace character codes
	return (in == 263 || in == 127 || in == 8 || in == 7);
}

int isregenkey(char in){
	// Ctrl-R code
	return (in == 18);
}

int isquitkey(char in){
	// Ctrl-Q keycode
	return (in == 24);
}

int test_string(struct keystat *instat){
	// Set all backspace to 0
	for(int i = 0; i < sizeof(instat->bs); i++){
		instat->bs[i] = 0;
	}
	
	// Configure the window
	struct window typing_window;
	configure_window(&typing_window);
	print_typing_string(&typing_window);
	print_test_string(&typing_window, instat->test_str);
	show_window(&typing_window);
	wmove(typing_window.window_p, 4, 0);

	// Read the first character and initialize timings
	char char_in = getch();
	instat->char_input[0] = char_in;
	instat->entered_str[0] = char_in;
	redraw_char(&typing_window, *instat->test_str, (char_in == *instat->test_str));

	// Everything to do with timing keypresses
	struct timeval now_tv;
	gettimeofday(&now_tv, 0);
	uint64_t st_t = (now_tv.tv_sec*1000) + (now_tv.tv_usec/1000);
	instat->timings[0] = 0;

	wrefresh(typing_window.window_p);

	// Start testing keystrokes until we have reached
	// the string's terminator char
	uint16_t i = 1,
		entr_i = 1;

	while(*(instat->test_str+i) != '\0'){
		char_in = getch();

		// Within the character input range
		if (char_in >= 32 && char_in <= 126) {
			redraw_char(&typing_window, *(instat->test_str+i),	\
						(char_in == *(instat->test_str+i)));

			// Get current time and record it
			gettimeofday(&now_tv, 0);
			instat->timings[i] = (now_tv.tv_sec*1000 + now_tv.tv_usec/1000)	\
				- st_t;

			// Update the entered string
			instat->char_input[i] = char_in;
			
			i++;
		} else if (isbackspace(char_in) && i > 0) {
			i--;
			backspace_character(&typing_window, *(instat->test_str+i));
				
			// Don't mark as bs if correct initially
			if(instat->char_input[i] != *(instat->test_str+i)){ 
				instat->bs[i]++; 
			}
		/* } else if (isregenkey(char_in)) { */
		/* 	printw("regenerating. "); */
		} else if (isquitkey(char_in)) {
			return 1;
		}
		

		// Update the entire entered string
		instat->entered_str[entr_i] = char_in;
		entr_i++;
		

		wrefresh(typing_window.window_p);


	}
	return 0;

}
