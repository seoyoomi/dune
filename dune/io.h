/*
* raw(?) I/O
*/
#ifndef _IO_H_
#define _IO_H_

#include "common.h"

KEY get_key(void);
void printc(POSITION pos, char ch, int color);
void gotoxy(POSITION pos);
void set_color(int color);
void set_cursor_position(int, int);
void clear_line_from_cursor();

#endif

