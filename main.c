/* Typing practice written by meetowl in C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/time.h>


#define WRONG_L COLOR_PAIR(1)
#define WRONG_HL COLOR_PAIR(2)
#define CORRECT_L COLOR_PAIR(3)
#define REWRITE_L COLOR_PAIR(4)
#define REWRITE_HL COLOR_PAIR(5)

/* Test string size */ 
#define TEST_STR_SIZE 5

/* Windows to be used */
struct window{
	WINDOW *window_p;
	WINDOW *border_p;
	int height;
	int	length;
	int start_curs_y;
	int start_curs_x;
};

struct keystat{
	char *test_str;
	int test_str_size;
	char entered_str[TEST_STR_SIZE*4];
	char char_input[TEST_STR_SIZE];
	int timings[TEST_STR_SIZE];
	int bs[TEST_STR_SIZE];
};

void exit_application(int stat){
	endwin();
	exit(stat);
}

int * get_std_height_length(){
	int * return_size = malloc(3*sizeof(int));
	getmaxyx(stdscr, *return_size, *(return_size+1));
	*(return_size+2) = '\0';
	return return_size;
}

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


// TODO: Finish text printing and determine optimal values for scaling
int scaled_height(int scr_height){
	if(scr_height < 18){
		return -1;
	}
	//	if(scr_height > 36){
	//	return scr_height / 2;
	//}
	return 9;
}

int scaled_length(int scr_length){
	if(scr_length < 82){
		return -1;
	}
	//	if(scr_length > 164){
	//	return scr_length / 2;
	//}
	return 79;
}

void print_option_menu(struct window *op_window){
	const char line_0[] = "Welcome to Keyvo!";
	const char line_1[] = "by meetowl";
	const char line_2[] = "Pick an option:";
	const char line_3[] = "1. Letters + Spaces";
	const char line_4[] = "2. Letters + Numbers + Spaces";
	const char line_5[] = "3. Letters + Numbers + Symbols + Spaces";
	const char line_6[] = "q: Quit application";
	const char line_7[] = "(default 3):";

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
	const char line_2[] = "F1 to return to menu | F3 to regenerate string";

	WINDOW *win = op_window->window_p;
	int win_l=op_window->length;
	op_window->start_curs_y = 4;
	op_window->start_curs_x = 0;

	mvwprintw(win, 0, (win_l - strlen(line_0))/2, "%s", line_0);
	mvwprintw(win, 1, (win_l - strlen(line_1))/2, "%s", line_1);
	mvwprintw(win, 2, (win_l - strlen(line_2))/2, "%s", line_2);
}



void configure_window(struct window *op_window){
	int *std_dim = get_std_height_length();
	int wdw_h = scaled_height(*std_dim),		\
		wdw_l = scaled_length(*(std_dim+1));

	// Makes sure screen is sufficient size
	if(wdw_h == -1 || wdw_l == -1){
		// TODO: write minimum x and y to resize to
		printf("Please resize your terminal!\n");
		exit_application(1);
	}

	// Set the struct's parameters
	op_window ->length = wdw_l;
	op_window->height = wdw_h;
	create_w(op_window);
	create_w_brdr(op_window);
}

char * generate_random_string(int gen_num){
	const char char_array[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQq"	\
		"RrSsTtUuWwXxYyZz";
	const char number_array[] = "1234567890";
	const char symbol_array[] = "~`!@#$%^&*(){[}]+=?/_-:;\\|\"'";

	struct timeval rand_tv;
	gettimeofday(&rand_tv, 0);
	srand(rand_tv.tv_sec+rand_tv.tv_usec);
	
	char *rdm_str = malloc(TEST_STR_SIZE*sizeof(char)+1);
	int next_space = 3+(rand()%6);
	for(int i = 0; i < TEST_STR_SIZE; i++){
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
	*(rdm_str+TEST_STR_SIZE) = '\0';
	return rdm_str;
}

void print_test_string(struct window *op_window, char * str){
	mvwaddch(op_window->window_p, 4, 0, *(str));
	int i = 1;
	while(*(str+i) != '\0'){
		waddch(op_window->window_p, *(str+i));
		i++;
	}
}

void show_window(struct window *op_window){
	refresh();
	wrefresh(op_window->border_p);
	wrefresh(op_window->window_p);
	refresh();
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


void test_string(struct keystat *instat){
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
	instat->char_input[0] = instat->entered_str[0] = char_in;
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
		
		// Backspace (emacs keeps changing which char it wants to use)
		if(char_in == 263 || char_in == 127 || char_in == 8 || char_in == 7){
			i--;
			backspace_character(&typing_window, *(instat->test_str+i));

			// Don't mark as bs if correct initially
			if(instat->char_input[i] != *(instat->test_str+i)){ 
				instat->bs[i]++; 
			} 
		}else{
			redraw_char(&typing_window, *(instat->test_str+i),	\
						(char_in == *(instat->test_str+i)));

			// Get current time and record it
			gettimeofday(&now_tv, 0);
			instat->timings[i] = (now_tv.tv_sec*1000 + now_tv.tv_usec/1000) - st_t;
			i++;
		}
		// Character entry book keeping
		instat->char_input[i] = char_in;
		instat->entered_str[entr_i] = char_in;
		entr_i++; 

		wrefresh(typing_window.window_p);
	}
}
int getavgms(struct keystat *stats){
	uint64_t sum_t = 0;
	for(int i = 1; i < stats->test_str_size; i++){
		sum_t += stats->timings[i] - stats->timings[i-1];
	}
	return sum_t/stats->test_str_size;
}

void
print_results(struct window *op_window, int avg_t, int wpm, int acc, \
			  int mstks, int mstks_fx){
	const char line0[] = "Test finished!";

	WINDOW *win = op_window->window_p;
	int win_l = op_window->length;

	mvwprintw(win, 0, (win_l - strlen(line0))/2, "%s", line0);
	mvwprintw(win, 1, 0, "Average keypress: %10dms", avg_t);
	mvwprintw(win, 2, 0, "Words per minute: %10d", wpm);
	mvwprintw(win, 3, 0, "Accuracy:         %10d\%", acc);
	mvwprintw(win, 4, 0, "Mistakes total:   %10d", mstks);
	mvwprintw(win, 5, 0, "Mistakes fixed:   %10d", mstks_fx);
	
	show_window(op_window);
}
void display_results(struct keystat *stats){
	struct window results_window;
	configure_window(&results_window);

	int avg_t = getavgms(stats);
	// TODO
	int wpm = 0,
		acc = 0,
		mstks = 0,
		mstks_fxd = 0;

	print_results(&results_window,				\
				  avg_t, wpm, acc, mstks, mstks_fxd);
	



	getchar();
}
int main(){
	start_curses();

	struct window welcome_menu;
	configure_window(&welcome_menu);

	// Start of what the user interacts with 
	int option_slct = 0;
	while(option_slct != 'q'){
		// Show window, take input
		print_option_menu(&welcome_menu);
		show_window(&welcome_menu);
		option_slct = getch() - '0';

		// This is the structure that most operations are performed on
		struct keystat curstat;

		// If first [1-3] selected
		if(option_slct > 0 && option_slct < 4){
			curstat.test_str = generate_random_string(option_slct);
			curstat.test_str_size = TEST_STR_SIZE;
			werase(welcome_menu.window_p);
			test_string(&curstat);
			display_results(&curstat);
		}
	}
	exit_application(0);
}
