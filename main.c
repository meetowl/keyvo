/* Typing practice written by meetowl in C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

/* TODO
   Rewrite every function to be much more unambigious
   Cut down on all the global variables, they are bad practice
   Learn C naming conventions
   Think of Christmas gift for Dad
*/
   


/* Constant arrays to lookup characters from */
const char char_array[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuWwXxYyZz";
const char number_array[] = "1234567890";
const char symbol_array[] = "~`!@#$%^&*(){[}]+=?/_-:;\\|\"'";

/* Windows */
WINDOW *app_border; /* border for the whole application, makes life easier */
WINDOW *welcome_menu; 
WINDOW *typing; /* typing screen */
WINDOW *analysis_screen; /* final analyzation screen */

/* Variables: changed by the program */
static char exam_string[257]; /* the string which has to be typed */
static int result_string[257]; /* binary t/f based on exam and entry strinng */
static int window_height, window_length;
static int lines, columns;
static int rerun; /* has the program been run already? */


/* Variables: changed by the user */ 
static int menu_selection; /* what has the user selected */
static int user_command; /* changed within the typing window */
static char entry_string[257]; /* the string which is typed */
                                                                                
/* Functions: Window / curses related */ 
void start_curses(); // Starts curses config
int menu_select(); // Selection menu
//int listen(); // Listens for input
//int redraw(char cand, int p); 

/* Functions: Logic related */
char * generate_random_string(int gen_num);
void clear_string();

int main(){ 
	//  start_curses();
	printf("%s\n", *generate_random_string(menu_select()));
	exit(0);
}

/* Do everything curses related */
void start_curses(){
	initscr();
	/* Checks whether monitor supports color, 
	   enable color mode if true
	*/
	if (has_colors() == TRUE ){
		start_color();
	}

	/* Defines different colors for different types of input */
	init_pair(1, COLOR_RED, COLOR_BLACK); /* Wrong letter typed */
	init_pair(2, COLOR_RED, COLOR_RED); /* Wrong space / newline */ 
	init_pair(3, COLOR_GREEN, COLOR_BLACK); /* Correct letter typed */

	getmaxyx(stdscr, lines, columns);
	window_height = lines / 2;
	window_length = columns - 2;

  
	// Assigns border characters
	app_border = newwin(window_height+2, window_length+2, ((lines / 2) - \
														   (lines / 4))-1 , 0); 
	wborder(app_border, '|', '|', '_', '_', ' ', ' ', ' ', ' ');

	// Assigns height/length for muliplie windows
	welcome_menu = newwin(window_height, window_length / 2,			\
						  (lines / 2) - (lines / 4),  (window_length / 2) / 4);
	typing = newwin(window_height, window_length, (lines / 2) - (lines / 4) , 1);
  
	noecho(); /* doesn't print what user types */
	cbreak(); /* reads input as soon as key is pressed, allows ctrl-c */
	keypad(stdscr, TRUE); /* allows thing like function keys */
	curs_set(0); /* makes cursor invisible */

	refresh();
	wrefresh(app_border);
}

/* returns number for menu option, letter for action */
int menu_select(){
	wprintw(welcome_menu,
			"hI welcome to chillis, pick one ya dingus\n\
1. Letters + Spaces\n\
2. Letters + Numbers + Spaces\n\
3. Letters + Numbers + Symbols + Spaces\n\
q: Quit application\n\
(default 3):");


	refresh();
	wrefresh(welcome_menu);
  
	wmove(welcome_menu,4,11);
	char user_select = getch();
	switch(user_select){
	case '1':
		return 1;
		break;
	case '2':
		return 2;
		break;
	case '3':
		return 3;
		break;
	case 'q':
		return 9; /* high number to allow expansion */
		break;
	default:
		return 3;
	}
}

							  
/* This clears every string related to typing, can't get rid of it or random 
   doesn't work properly :/ */
void clear_string(){
	for ( int i = 0; i < 255; i++){
		exam_string[i] = 0;
		entry_string[i] = 0;
		result_string[i] = 0;
	}
}

/* Generates random 254 character string of no more than
   8 letters per word and 64 characters per line */
char * generate_random_string(int gen_num){
	clear_string();
	srand(time(0));

	char * random_string[257]; 
	int next_space = 2 + (rand() % 6);
	for(int i = 0; i < 255; i++){
		if( i > 1 ){
			/* newline every 64 characters */
			if( i == 63 || i == 127 || i == 191 || i == 255 ){ 
				strcat(*random_string,"\n");
				continue;
			}
			if( i == next_space){
				strcat(*random_string," ");
				next_space = i + 2 + (rand() % 8);
				/* space before newline is confusing, avoids it. */
				if( next_space == 62 || next_space == 126 ||\
					next_space == 190 || next_space == 254 )\
					next_space += 2 +(rand() % 9); 

				/* makes sure space doesn't fall on newline */
				if( next_space == 63 || next_space == 127 ||\
					next_space == 191 || next_space == 255 )\
					next_space += (rand() % 8); 
				continue;
			}
		}
    
		/* gen_num is the user choice in menu_selection */
		switch(rand() % gen_num){ 
		case 0:
			*random_string[i] = char_array[rand() % 49];
			break;
		case 1:
			*random_string[i] = number_array[rand() % 9];
			break;
		case 2:
			*random_string[i] = symbol_array[rand() % 28];
			break;
		default:
			printf("Something wrong with switch");
			break;
		}
	}
}


int listen(){ 
	const static char type_info[] = "Type the characters, no backspaces," \
		"each line ends with a return character (enter)";
	const static char func_info[] = "F1 to return to menu " \
		"F3 to regenerate and start again";
  
	mvwprintw(typing, 0, (window_length - strlen(type_info)) / 2, \
			  "%s", type_info);
	mvwprintw(typing, 1, (window_length - strlen(func_info)) / 2, \
			  "%s", func_info);

	mvwprintw(typing, 2, 0, "%s", exam_string);
	wmove(typing,2,0);
	wrefresh(typing);

	for ( int it = 0; it < strlen(exam_string); it++ ){
		char user_entry = getch();
		switch(user_entry){
		case (char) KEY_F(1):
			wclear(typing);
			return 0;
			break;
		case (char) KEY_F(3):
			return 2;
			break; 
		default:
			result_string[it] = redraw(user_entry,it);
			wrefresh(typing);
		}
	}
    
}

int redraw(char cand, int p){ /* cand is user entry, p is string pos */
	entry_string[p] = cand; 

	if ( cand == exam_string[p] ){
		result_string[p] = 1;
		waddch(typing, exam_string[p] | GREEN_LETTER);
		return 1;
	}
    if ( exam_string[p] == '\n' && cand != '\n' ) {
		result_string[p] = 0;
		waddch(typing, ' ' | RED_BACK );
		waddch(typing, '\n');
		return 0;
	}
	if ( exam_string[p] == ' ' ){
		result_string[p] = 0;
		waddch(typing, exam_string[p] | RED_BACK );
		return 0;
	}
	result_string[p] = 0;
	waddch(typing, exam_string[p] | RED_LETTER );
	return 0;
}
