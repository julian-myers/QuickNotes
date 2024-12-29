#ifndef OPTIONS_H
#define OPTIONS_H

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 250

extern char *choices[];
extern int n_choices;
// function declarations
void init_colors();
void printMenu(WINDOW *menu_win, int highlight);
void view_notes();
void add_note();
char **read_files(const char *dir_path, int *file_count);
void free_file_names(char **file_names, int file_count);
void render_markdown(WINDOW *win, const char *filepath);

#endif // OPTIONS_H
