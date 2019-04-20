/* Typing practice written by meetowl in C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

#define WRONG_L COLOR_PAIR(1)
#define WRONG_HL COLOR_PAIR(2)
#define CORRECT_L COLOR_PAIR(3)

/* Test string size */ 
#define TEST_SIZE_CHAR 321

/* Windows to be used */
struct window{
	WINDOW *window_p;
	WINDOW *border_p;
	int height, length;
};

void exit_application(int stat){
	endwin();
	exit(stat);
}

void set_fonts(int theme){ // TODO: Allow different themes
	switch(theme){
	case 0: // Monochrome monitor
		init_pair(1, COLOR_BLACK, COLOR_WHITE); /* Wrong letter typed */
		init_pair(2, COLOR_BLACK, COLOR_WHITE); /* Wrong space / newline */ 
		init_pair(3, COLOR_WHITE, COLOR_BLACK); /* Correct letter typed */
		break;
	case 1:
		init_pair(1, COLOR_RED, COLOR_BLACK); /* Wrong letter typed */
		init_pair(2, COLOR_RED, COLOR_RED); /* Wrong space / newline */ 
		init_pair(3, COLOR_GREEN, COLOR_BLACK); /* Correct letter typed */
		break;
	}
}
/* Creates the window in the given window structure pointer */
void create_w(struct window *op_window, struct window *std_window){
	
	int win_start_h = (std_window->height/2) - (op_window->height/2);
	int win_start_l = (std_window->length/2) - (op_window->length/2);
	op_window->window_p = newwin(op_window->height, op_window->length,	\
								 win_start_h, win_start_l);
}
/* Creates the window border in the given window structure pointer */
void create_w_brdr(struct window *op_window, struct window *std_window){
	int border_h = op_window->height+3;
	int border_l = op_window->length+3;
	int border_start_h = ((std_window->height/2) - (op_window->height/2)) - 1;
	int border_start_l = ((std_window->length/2) - (op_window->length/2)) - 1;

	op_window->border_p = newwin(border_h, border_l, border_start_h, border_start_l);
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
	const char line_1[] = "Type the characters, no backspaces.";
	const char line_2[] = "F1 to return to menu | F3 to regenerate string";

	WINDOW * win = op_window->window_p;
	int win_l=op_window->length;

	mvwprintw(win, 0, (win_l - strlen(line_0))/2, "%s", line_0);
	mvwprintw(win, 1, (win_l - strlen(line_1))/2, "%s", line_1);
	mvwprintw(win, 2, (win_l - strlen(line_2))/2, "%s", line_2);

		
		
}
void configure_window(struct window *op_window, 			\
					  struct window *std_window,			\
					  int wdw_h, int wdw_l){				

	op_window->length = wdw_l;
	op_window->height = wdw_h;
	create_w(op_window, std_window);
	create_w_brdr(op_window, std_window);
			 
}
char * generate_random_string(int gen_num){
	const char char_array[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQq"	\
		"RrSsTtUuWwXxYyZz";
	const char number_array[] = "1234567890";
	const char symbol_array[] = "~`!@#$%^&*(){[}]+=?/_-:;\\|\"'";

	srand(time(0));
	
	char *rdm_str = malloc(TEST_SIZE_CHAR*sizeof(char));
	int next_space = 3+(rand()%6);
	for(int i = 0; i < TEST_SIZE_CHAR; i++){
		switch(rand() % gen_num){
		case 0:
			// -1 the size, or '\0' will have a chance of appearing.
			*(rdm_str+i) = char_array[rand() % \
									  (sizeof(char_array)-1)];
			break;
		case 1:
			*(rdm_str+i) = number_array[rand() %					\
										(sizeof(number_array)-1)];
			/*			if(*(rdm_str+i) == 0){
				printw("it equals to b0");
				}*/
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
	*(rdm_str+TEST_SIZE_CHAR) = '\0';
	return rdm_str;
}

void print_test_string(struct window *op_window, char * str){
	// BUG: cursor doesn't change with `waddch` after `wmove`
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


void redraw_char(struct window *op_window, const char * test_char,	\
				 const char in_correct){
				 
	if(in_correct){
		waddch(op_window->window_p, *test_char | CORRECT_L);
	}else if(*test_char == ' '){
		waddch(op_window->window_p, *test_char | WRONG_HL);
	}else{
		waddch(op_window->window_p, *test_char | WRONG_L);
	}
		

	wrefresh(op_window->window_p);
}
int is_char_correct(const char * test_char, char in_char){
	if(in_char == *test_char){
		return 1;
	}
	return 0;
}

int main(){
	start_curses();

	struct window std_window;
	getmaxyx(stdscr, std_window.height, std_window.length);
	int wdw_h = scaled_height(std_window.height);
	int wdw_l = scaled_length(std_window.length);
	if( wdw_h == -1 || wdw_l == -1){
		printf("Please resize your terminal!\n");
		exit_application(1);
	}
	
	struct window welcome_menu;
	configure_window(&welcome_menu, &std_window, wdw_h, wdw_l);
	//	print_option_menu(&welcome_menu);
	struct window type_window;
	configure_window(&type_window, &std_window, wdw_h, wdw_l);
	//	print_typing_string(&type_window);
	struct window result_window;
	configure_window(&result_window, &std_window, wdw_h, wdw_l);




	
	int option_slct = 0;
	/* Start of what the typist interacts with 
	   Quits when 'q' is pressed (char 65)
	 */
	do{
		werase(type_window.window_p);
		print_option_menu(&welcome_menu);
		show_window(&welcome_menu);
		option_slct = getch() - '0';
		static char * test_str;
		if(option_slct == 1 || option_slct == 2 ||	\
		   option_slct == 3){
			test_str = generate_random_string(option_slct);
			werase(welcome_menu.window_p);
			print_typing_string(&type_window);
			print_test_string(&type_window, test_str);
			show_window(&type_window);
		}

		if(option_slct != 65){
			wmove(type_window.window_p, 4, 0);
			int i = 0;
			int time_new;
			int *time_last = &time_new;
			int *time_array = malloc(TEST_SIZE_CHAR*sizeof(int));
			while(*(test_str+i) != '\0'){
				int in_correct = is_char_correct(test_str+i, getch());
				time_new = time(0);
				*(time_array+i) = time_new - *time_last -1;
				//				printw("%d, %d, %d",*(time_array+i), time_new, *time_last);
				int temp = time_new;
				*time_last =  &temp;
				redraw_char(&type_window, test_str+i, in_correct);
				i++;
			}
			int time_end = time(0);
		}
	}while(option_slct != 65);
	exit_application(0);
}