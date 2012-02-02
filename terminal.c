#include <math.h>
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

int num_digits(int i)
{
    return (i > 0) ? (int) log10((double) i) + 1 : 1;
}

void terminal_cursor_position(int row, int column)
{
    int length = num_digits(row) + num_digits(column) + 2;
    char *seq = malloc(length);
    snprintf(seq, length, "%i;%iH", row, column);
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
