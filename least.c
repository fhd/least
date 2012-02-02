#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "terminal.h"

char **buffer;
int terminal_rows, terminal_cols, display_rows, first_line, num_lines;

void read_terminal_properties(void)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    terminal_rows = w.ws_row;
    terminal_cols = w.ws_col;
    display_rows = terminal_rows - 1;
}

char *read_line(FILE *fp)
{
    char *line = malloc(terminal_cols);
    if (!fgets(line, terminal_cols, fp)) {
        free(line);
        return NULL;
    }
    int last = strlen(line) - 1;
    if (line[last] == '\n')
        line[last] = '\0';
    buffer = realloc(buffer, (num_lines + 1) * sizeof(char *));
    buffer[num_lines++] = line;
    return line;
}

void read_content(const char *file)
{
    buffer = NULL;
    num_lines = 0;
    FILE *fp = fopen(file, "r");
    if (!fp) {
        printf("Unable to open file %s.\n", file);
        exit(1);
    }
    while(read_line(fp));
}

struct termios *init_terminal(void)
{
    struct termios *oldt = malloc(sizeof(struct termios));
    tcgetattr(STDIN_FILENO, oldt);
    struct termios newt = *oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    return oldt;
}

void scroll_up(void)
{
    if (first_line <= 0)
        return;

    terminal_scroll_down();
    terminal_erase_in_line();
    terminal_cursor_position(1, 1);
    puts(buffer[--first_line]);
    terminal_cursor_position(terminal_rows, 1);
}

void scroll_down(void)
{
    if (first_line + display_rows >= num_lines)
        return;

    terminal_scroll_up();
    terminal_cursor_previous_line();
    puts(buffer[++first_line + display_rows - 1]);
}

void scroll_page_up(void)
{
    int i;
    for (i = 0; i < display_rows; i++)
        scroll_up();
}

void scroll_page_down(void)
{
    int i;
    for (i = 0; i < display_rows; i++)
        scroll_down();
}

void scroll_to_line(int line)
{
    first_line = line;
    terminal_erase_data();
    terminal_cursor_position(1, 1);
    int i;
    for (i = 0; i < display_rows; i++)
        puts(buffer[line + i]);
}

void scroll_to_top(void)
{
    scroll_to_line(0);
}

void scroll_to_bottom(void)
{
    scroll_to_line(num_lines - display_rows);
}

void handle_escape_sequence(void)
{
    char c = getchar();
    if (c == 'O') {
        c = getchar();
        if (c == 'H')
            scroll_to_top();
        else if (c == 'F')
            scroll_to_bottom();
    } else if (c == '[') {
        c = getchar();
        if (c == 'A')
            scroll_up();
        else if (c == 'B')
            scroll_down();
        else if (c == '5' && getchar() == '~')
            scroll_page_up();
        else if (c == '6' && getchar() == '~')
            scroll_page_down();
    }
}

void handle_input(void)
{
    char c;
    while ((c = getchar()) != 'q')
        if (c == ESC)
            handle_escape_sequence();
}

void free_buffer()
{
    int i;
    for (i = 0; i < num_lines; i++)
        free(buffer[i]);
    free(buffer);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s FILE\n", basename(argv[0]));
        return 1;
    }

    read_terminal_properties();
    read_content(argv[1]);

    struct termios *oldt = init_terminal();

    scroll_to_line(0);
    handle_input();

    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
    free(oldt);
    free_buffer();
    return 0;
}
