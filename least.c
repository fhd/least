#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define ESC 27

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

void handle_escape_sequence(void)
{
    if (getchar() != '[')
        return;

    switch (getchar()) {
    case 'A':
        printf("up\n");
        break;
    case 'B':
        printf("down\n");
        break;
    case 'C':
        printf("left\n");
        break;
    case 'D':
        printf("right\n");
        break;
    }
}

void handle_input(void)
{
    char c;
    while ((c = getchar()) != 'q')
        if (c == ESC)
            handle_escape_sequence();
}

int main(void)
{
    struct termios *oldt = init_terminal();
    clear_screen();
    position_cursor(1, 1);
    handle_input();
    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
    free(oldt);
}
