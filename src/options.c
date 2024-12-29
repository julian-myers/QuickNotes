#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include "options.h"


// Menu Options
char *choices[] = {
	"View Note",
	"New Note",
	"Exit",
};

int n_choices = sizeof(choices) / sizeof(char *);


void printMenu(WINDOW *menu_win, int highlight)
{
	int start_x;
	int start_y = (getmaxy(menu_win) -  n_choices) / 2;

	box(menu_win, 0, 0);

	for (int i = 0; i < n_choices; i++) {
		start_x = (getmaxx(menu_win) - strlen(choices[i])) / 2;
		if (highlight == i+1) {
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, start_y+1, start_x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		} else {
			mvwprintw(menu_win, start_y + 1, start_x, "%s", choices[i]);
		}
		start_y++;
	}

	wrefresh(menu_win);
}


void view_notes()
{
	use_default_colors();
	init_pair(1, COLOR_BLUE, -1);
	init_pair(2, COLOR_GREEN, -1);

	const char *directory_path = "./QuickNotes/";
	char **file_names;
	int file_count;
	file_names = read_files(directory_path, &file_count);

	// window for the file explorer.
	int file_w = COLS / 4, file_h = LINES;
	int start = 0;
	WINDOW *file_win = newwin(file_h, file_w, start, start);
	wattron(file_win, COLOR_PAIR(2));
	box(file_win, 0, 0);
	wattroff(file_win, COLOR_PAIR(2));
	wrefresh(file_win);
	mvwprintw(file_win, 1, 1, "NOTES:");
	wrefresh(file_win);

	// window for the notes viewer.
	int view_w = COLS - file_w, view_h = LINES;
	int startx = file_w;
	int starty = 0;
	WINDOW *view_win = newwin(view_h, view_w, starty, startx);
	wattron(view_win, COLOR_PAIR(1));
	box(view_win, 0, 0);
	wattroff(view_win, COLOR_PAIR(1));
	wrefresh(view_win);

	int highlight = 1;
	int c;

	while (true) {
		// move cursor through the list of files
		for (int i = 0; i < file_count; i++) {
			if (highlight == i+1) {
				wattron(file_win, A_REVERSE);
				mvwprintw(file_win, i + 2, 2, "%s", file_names[i]);
				wattroff(file_win, A_REVERSE);
			} else {
				mvwprintw(file_win, i + 2, 2, "%s", file_names[i]);
			}
		}
		wrefresh(file_win);

		// markdown parser
		char file_path[PATH_MAX];
		snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, file_names[highlight - 1]);
		werase(view_win);
		box(view_win, 0, 0);
		render_markdown(view_win, file_path);

		c = wgetch(file_win);
		switch (c) {
			case 'k':
				if (highlight == 1) 
					highlight = file_count;
				else
					highlight --;
				break;

			case 'j':
				if (highlight == file_count)
					highlight = 1;
				else
					highlight++;
				break;

			case 'l': {
				char command[PATH_MAX + 50];
				snprintf(command, sizeof(command), "nvim %s", file_path);
				endwin();
				int ret = system(command);
				if (ret == -1) {
					perror("system");
				} else if (WEXITSTATUS(ret) != 0) {
					fprintf(stderr, "Command failed with exit code %d\n", WEXITSTATUS(ret));
				}
				initscr();
				clear();
				break;
			}
			case 'D': { // Delete the highlighted file
				mvwprintw(file_win, file_h - 2, 1, "Are you sure you want to delete? (y/n)");
				wrefresh(file_win);

				int confirm = wgetch(file_win);
				if (confirm == 'y' || confirm == 'Y') {
					if (remove(file_path) == 0) {
							mvwprintw(file_win, file_h - 2, 1, "File deleted successfully.");
					} else {
							mvwprintw(file_win, file_h - 2, 1, "Failed to delete the file.");
					}
					wrefresh(file_win);
					// Refresh file list
					free_file_names(file_names, file_count);
					file_names = read_files(directory_path, &file_count);
					highlight = 1; // Reset highlight
					werase(file_win);
					box(file_win, 0, 0);
					mvwprintw(file_win, 1, 1, "NOTES:");
					wrefresh(file_win);
				} else {
					mvwprintw(file_win, file_h - 2, 1, "Deletion canceled.");
					wrefresh(file_win);
				}
				break;
			}

			case 'q':
				delwin(file_win);
				delwin(view_win);
				free_file_names(file_names, file_count);
				return;

			default:
				break;
		}

	}
}


// add note func
void add_note()
{
	// prompt window dimensions
	int width = 40, height = 3;
	int startx = (COLS - width) / 2; 
	int starty = (LINES - height) / 2; 

	// creating the prompt window.
	WINDOW *prompt_win = newwin(height, width, starty, startx);
	box(prompt_win, 0, 0);
	mvwprintw(prompt_win, 1, 1, "Title: "); 
	wrefresh(prompt_win);

	echo();
	char filename[256];
	mvwgetstr(prompt_win, 1, strlen("Title:  "), filename);
	noecho();

	delwin(prompt_win);

	// open neovim
	// TODO: use EDITOR environment variable.
	char command[300];
	snprintf(command, sizeof(command), "nvim QuickNotes/%s.md", filename);
	refresh();

	endwin();

	int ret = system(command);
	if (ret == -1) {
		perror("system");
	} else if (WEXITSTATUS(ret) != 0) {
		fprintf(stderr, "Command failed with exit code %d\n", WEXITSTATUS(ret));
	}

	initscr();
	clear();
	mvprintw(1, 1, "editing complete. press any key to return to the main menu.");
	refresh();
	getch();
}


// function for reading the names of files in a given directory,
// for this case, the QuickNotes directory.
char **read_files(const char *dir_path, int *file_count)
{
    DIR *dir;
    struct dirent *entry;
    char **file_names = NULL;
    int count = 0;

    // Open the directory
    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return NULL;
    }

    // Iterate through the directory entries
    while ((entry = readdir(dir)) != NULL) {
        // ignore "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Reallocate memory to store the new file name
        file_names = realloc(file_names, sizeof(char *) * (count + 1));
        if (file_names == NULL) {
            perror("realloc");
            closedir(dir);
            return NULL;
        }

        // Allocate memory for the file name and copy it
        file_names[count] = strdup(entry->d_name);
        if (file_names[count] == NULL) {
            perror("strdup");
            closedir(dir);
            return NULL;
        }

        count++;
    }

    // Close the directory
    closedir(dir);

    // Store the number of files in the output parameter
    *file_count = count;

    return file_names;
}

// this just frees the allocated memory
void free_file_names(char **file_names, int file_count)
{
    for (int i = 0; i < file_count; i++) {
        free(file_names[i]);
    }
    free(file_names);
}


// this parses the markdown file.
// TODO: image rendering
void render_markdown(WINDOW *win, const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        mvwprintw(win, 1, 1, "Failed to open file: %s", filepath);
        wrefresh(win);
        return;
    }

    char line[1024];
    int row = 1; // Starting row for text
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

				const char *start = line;

        if (strncmp(line, "# ", 2) == 0) {
            // H1 Header: Bold and red
            wattron(win, A_BOLD | COLOR_PAIR(1));
            mvwprintw(win, row++, 1, "%s", line + 2); // Skip "# "
            wattroff(win, A_BOLD | COLOR_PAIR(1));
						continue;
        } else if (strncmp(line, "## ", 3) == 0) {
            // H2 Header: Bold only
            wattron(win, A_BOLD);
            mvwprintw(win, row++, 1, "%s", line + 3); // Skip "## "
            wattroff(win, A_BOLD);
						continue;
        }

				if (strncmp(line, "- ", 2) == 0 || strncmp(line, "* ", 2) == 0) {
            // List Item: Green bullet
            wattron(win, COLOR_PAIR(2));
            mvwprintw(win, row++, 2, "• %s", line + 2); // Indent bullet
            wattroff(win, COLOR_PAIR(2));
						continue;
        }

				if (strncmp(line, "> ", 2) == 0) {
            // Blockquote: Blue and italic (dim in `ncurses`)
            wattron(win, COLOR_PAIR(3) | A_DIM);
            mvwprintw(win, row++, 3, "%s", line + 2); // Indent blockquote
            wattroff(win, COLOR_PAIR(3) | A_DIM);
						continue;
        }

				if (strncmp(line, "```", 3) == 0) {
            // Code Block: Yellow and reversed
            wattron(win, COLOR_PAIR(4) | A_REVERSE);
            mvwprintw(win, row++, 1, "%s", line);
            wattroff(win, COLOR_PAIR(4) | A_REVERSE);
						continue;
        } 
				
				while (*start) {
					if (strncmp(start, "**", 2) == 0) {
						wattron(win, A_BOLD);
						start += 2;
						const char *end = strstr(start, "**");
						if (end) {
							while (start < end) {
								waddch(win, *start++);
							}
							wattroff(win, A_BOLD);
							start += 2;
						} else {
							waddch(win, *start++);
						}
					} else if ((*start == '*' || *start == '_') && (start == line || *(start - 1) == ' ')) {
							// Handle italic text
							char delimiter = *start;
							start++; // Skip '*' or '_'
							const char *end = strchr(start, delimiter);
							if (end) {
								wattron(win, A_ITALIC);
								// Print italic text
								while (start < end) {
										waddch(win, *start++);
								}
								wattroff(win, A_ITALIC);
								start++; // Skip closing '*' or '_'
							} else {
								// No closing delimiter, print remaining text as is
								waddch(win, *start++);
							}
            } else {
               // Print plain text
               waddch(win, *start++);
            }
				}


				row++;
        if (row >= getmaxy(win) - 1) {
            break; // Prevent overflow
        }
				wmove(win, row, 1);
    }

    fclose(file);
    wrefresh(win);
}
