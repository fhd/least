#ifndef TERMINAL_H
#define TERMINAL_H

#define ESC 27

void terminal_cursor_previous_line(void);
void terminal_cursor_position(int row, int column);
void terminal_erase_data(void);
void terminal_erase_in_line(void);
void terminal_scroll_up(void);
void terminal_scroll_down();

#endif
