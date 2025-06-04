#ifndef UI_H
#define UI_H

#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>

int init_ui();

void focus_on(WINDOW **focus, WINDOW *win);

void draw_answerbox( WINDOW *win, uint32_t selidx, bool confirm, const char *questions[], uint32_t size );
void draw_questionbox( WINDOW *win, const char *question );
void draw_credits( WINDOW *win );
void draw_main_menu( WINDOW *win, uint32_t selidx );

#endif // UI_H