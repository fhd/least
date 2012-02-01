#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "terminal.h"

/*
 * TODO: Don't hard code buffer size.
 */
#define BUFFER_LINES 1024
#define BUFFER_COLS 80

char buffer[BUFFER_LINES][BUFFER_COLS];
int terminal_rows, display_rows, first_line, num_lines;

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

void read_content(const char *file)
{
    FILE *fp = fopen(file, "r");
    int i;
    for (i = 0; i < BUFFER_LINES; i++) {
        char *line = (char *) &buffer[i];
        fgets(line, BUFFER_COLS, fp);
        int len = strlen(line);
        if (len == 0)
            break;
        int last = len - 1;
        if (line[last] == '\n')
            line[last] = '\0';
        if (i < display_rows)
            puts(line);
    }
    first_line = 0;
    num_lines = i;
}

void handle_input(void)
{
    char c;
    while ((c = getchar()) != 'q')
        if (c == ESC)
            handle_escape_sequence();
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s FILE\n", basename(argv[0]));
        return 1;
    }

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    terminal_rows = w.ws_row;
    display_rows = terminal_rows - 1;

    struct termios *oldt = init_terminal();
    terminal_erase_data();
    terminal_cursor_position(1, 1);
    read_content(argv[1]);
    handle_input();
    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
    free(oldt);
    return 0;
}
