#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define ESC 27
/*
 * TODO: Don't hard code buffer size.
 */
#define BUFFER_LINES 1024
#define BUFFER_COLS 80

char buffer[BUFFER_LINES][BUFFER_COLS];
int terminal_rows, first_line, num_lines;

struct termios *init_terminal(void)
{
    struct termios *oldt = malloc(sizeof(struct termios));
    tcgetattr(STDIN_FILENO, oldt);
    struct termios newt = *oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    return oldt;
}

void print_escape_sequence(const char *seq)
{
    printf("%c[%s", ESC, seq);
}

void clear_screen(void)
{
    print_escape_sequence("2J");
}

void position_cursor(int row, int column)
{
    char *seq;
    asprintf(&seq, "%i;%iH", row, column);
    print_escape_sequence(seq);
    free(seq);
}

void scroll_up(void)
{
    if (first_line <= 0)
        return;

    print_escape_sequence("T");
    print_escape_sequence("2K");
    position_cursor(1, 1);
    puts(buffer[--first_line]);
    position_cursor(terminal_rows, 1);
}

void scroll_down(void)
{
    if (first_line + terminal_rows >= num_lines)
        return;

    print_escape_sequence("S");
    print_escape_sequence("F");
    puts(buffer[++first_line + terminal_rows]);
}

void handle_escape_sequence(void)
{
    if (getchar() != '[')
        return;

    switch (getchar()) {
    case 'A':
        scroll_up();
        break;
    case 'B':
        scroll_down();
        break;
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
        if (i <= terminal_rows)
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

    struct termios *oldt = init_terminal();
    clear_screen();
    position_cursor(1, 1);
    read_content(argv[1]);
    handle_input();
    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
    free(oldt);
    return 0;
}
