#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "options.h"


void init_colors()
{
	if (!has_colors() || start_color() == ERR) {
		endwin();
		fprintf(stderr, "colors not supported");
		exit(1);
	}

	use_default_colors();

	init_pair(1, COLOR_RED, -1);
	init_pair(2, COLOR_GREEN, -1);
	init_pair(3, COLOR_BLUE, -1);
	init_pair(4, COLOR_YELLOW, -1); 
	init_pair(5, COLOR_CYAN, -1); 
}


int main()
{
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	init_colors();

	// menu dimensions
	int startx = 0, starty = 0;
	int width = COLS, height = LINES;
	WINDOW *menu_win = newwin(height, width, starty, startx);
	keypad(menu_win, TRUE); // Enable keyboard input for the menu window

	// Menu logic
	int highlight = 1;
	int choice = 0;
	int c;

	while (1) {

		printMenu(menu_win, highlight); // Print the menu
		c = wgetch(menu_win); // Capture user input

		switch (c) {
				case 'k':
						if (highlight == 1)
								highlight = n_choices;
						else
								highlight--;
						break;

				case 'j':
						if (highlight == n_choices)
								highlight = 1;
						else
								highlight++;
						break;

				case 10: // Enter key
						choice = highlight;
						break;

				case 'q': // q for quit 
						endwin();
						return 0;

				default:
						break;
		}

		if (choice) { // Execute the selected option
				if (choice == 1) {
						view_notes();
				} else if (choice == 2) {
						add_note();
				} else if (choice == 3) {
						break;
				} else if (choice == 4) {
						break; // Exit the loop
				}

				choice = 0; // Reset choice after executing
		}
	}

	// Clean up and exit
	clrtoeol();
	refresh();
	endwin();
	return 0;
}
