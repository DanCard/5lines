/*  Limit output to last 5 lines.

    Copyright (C) 2015  Daniel Cardenas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Todo:  Would like default name of log file to base on program name executed prior to pipe, but
 *        don't know how to retrieve that.
 * 
 * Compile:
 *   gcc -o 5lines 5lines.c -lncurses
 */
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_BUFFER_SIZE 8192

int main(int argc, char * const * argv) {
    int n_screen_log_lines = 5;
    char *cvalue = NULL;
    int index;
    int option;

    opterr = 0;
    while ((option = getopt (argc, argv, "0123456789n:")) != -1)
      switch (option) {
          case '0': n_screen_log_lines = 0; break;
          case '1': n_screen_log_lines = 1; break;
          case '2': n_screen_log_lines = 2; break;
          case '3': n_screen_log_lines = 3; break;
          case '4': n_screen_log_lines = 4; break;
          case '5': n_screen_log_lines = 5; break;
          case '6': n_screen_log_lines = 6; break;
          case '7': n_screen_log_lines = 7; break;
          case '8': n_screen_log_lines = 8; break;
          case '9': n_screen_log_lines = 9; break;
          case 'n':
            sscanf(optarg, "%d", &n_screen_log_lines);
            break;
          case '?':
            if (optopt == 'n')
              fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
              fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
              fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
          default:
            abort ();
      }
    //printf ("num log lines = %d\n", n_screen_log_lines);

    int buf_index = 0;
    FILE *log_file;
    int num_buffers = n_screen_log_lines ? : 2;
    char buffer[num_buffers][LINE_BUFFER_SIZE];

    // Retrieve first line of input.  Default is to use it for determining log file name.
    char * fgets_return = fgets(buffer[buf_index], LINE_BUFFER_SIZE, stdin);
    if (fgets_return == NULL) return 0;

    // Open log file.
    char * filename;
    if (optind < argc) {
        filename = (char *)argv[optind];
    } else {
        char filename_buf[128];
        filename = filename_buf;
        // Use first word as file name.
        // Scan for space.
        char c;
        int space_loc = 0;
        while ((c = buffer[buf_index][space_loc]) != 0) {
            if (c == ' ') {
                break;
            }
            space_loc++;
        }
        int last_char_loc = space_loc;
        if (last_char_loc > 123 ) last_char_loc = 123;
        strncpy(filename, buffer[buf_index], last_char_loc);
        strcpy(&filename[last_char_loc], ".log");
        printf("\t\t\t\tLog file: %s\n", filename );
    }
    log_file = fopen(filename, "w");
    
    // Set up for ncurses
    const char* term_type = getenv("TERM");
    if (term_type == NULL || *term_type == '\0') {
        term_type = "unknown";
    }
    FILE* term_in = fopen("/dev/tty", "r");
    if (term_in == NULL) {
        perror("fopen(/dev/tty)");
        return 1;
    }
    SCREEN* main_screen = newterm(term_type, stdout, term_in);
    set_term(main_screen);
    scrollok(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    raw();
    // Done setting up ncurses

    // Read input and write output loop
    while(fgets_return != NULL) {
        printw("%s\r", buffer[buf_index]);
        refresh();      // Ask ncurses to update the screen.
        fputs(buffer[buf_index], log_file); 
        buf_index++;
        if (buf_index >= num_buffers) buf_index = 0;
        fgets_return = fgets(buffer[buf_index], LINE_BUFFER_SIZE, stdin);
    }
    endwin();   // ncurses mode exit.

    // Log the last few lines to stdout.
    int j;
    for (j = 0; j < n_screen_log_lines; j++) {
        fputs(buffer[buf_index], stdout);
        buf_index += 1;
        if (buf_index >= n_screen_log_lines) buf_index = 0;
    }

    return 0;
}
