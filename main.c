/* Typing practice written by meetowl in C */
#include "stat.h"
#include "input.h"
#include <sys/types.h>
#include <unistd.h>

int main(){
	start_curses();
	refresh();
	struct window welcome_menu;
	init_centre_window(&welcome_menu);
	draw_box_around_win(&welcome_menu);
	
	// Start of what the user interacts with 
	char option_slct = 0;
	char prev_slct = 0;
	while(option_slct != 'q'){
		// Show window, take input
		print_option_menu(&welcome_menu, prev_slct);
		wrefresh(welcome_menu.window_p);

		option_slct = getch();

		// If 'previous' was selected
		if(option_slct == 'p' && prev_slct != 0){
			option_slct = prev_slct;
		}
                                                                                		
		if (option_slct == 1) {
			print_gen_menu(&welcome_menu);
		}

		struct keystat current_stats;
		init_keystat(&current_stats);
		current_stats.test_str = generate_random_string(option_slct);
		current_stats.test_str_size = TEST_STR_SIZE;
		werase(welcome_menu.window_p);
		int fin_code = test_string(&current_stats);

		if(fin_code == 0){
			display_results(&current_stats);
		}

		prev_slct = option_slct;
	}
	exit_application(0);
}
