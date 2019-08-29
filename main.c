/* Typing practice written by meetowl in C */
#include "stat.h"
#include "input.h"

int main(){
	start_curses();

	
	
	struct window welcome_menu;
	configure_window(&welcome_menu);

	// Start of what the user interacts with 
	char option_slct = 0;
	char prev_slct = 0;
	while(option_slct != 'q' - '0'){
		// Show window, take input
		print_option_menu(&welcome_menu, prev_slct + '0');
		show_window(&welcome_menu);
		option_slct = getch() - '0';

		// If 'previous' was selected
		if(option_slct == 'p' - '0'){
			option_slct = prev_slct;
		}

		// This is the structure that most operations are performed on
		struct keystat curstat;


		// If first [1-3] selected
		if(option_slct >= 1 && option_slct <= 3){
			curstat.test_str = generate_random_string(option_slct);
			curstat.test_str_size = TEST_STR_SIZE;
			werase(welcome_menu.window_p);
			int fincode = test_string(&curstat);

			if(fincode == 0){
				display_results(&curstat);
			}

			prev_slct = option_slct;
		}

	}
	exit_application(0);
}
