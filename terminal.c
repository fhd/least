#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"

void print_escape_sequence(const char *seq)
{
    printf("%c[%s", ESC, seq);
}

void terminal_cursor_previous_line(void)
{
    print_escape_sequence("F");
}

void terminal_cursor_position(int row, int column)
{
    char *seq;
    asprintf(&seq, "%i;%iH", row, column);
    print_escape_sequence(seq);
    free(seq);
}

void terminal_erase_data(void)
{
    print_escape_sequence("2J");
}

void terminal_erase_in_line(void)
{
    print_escape_sequence("2K");
}

void terminal_scroll_up(void)
{
    print_escape_sequence("S");
}

void terminal_scroll_down()
{
    print_escape_sequence("T");
}
