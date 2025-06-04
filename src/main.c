#include <stdio.h>
#include "http.h"
#include "ui.h"

static bool shouldClose = false;

extern const char *opts_test[];
static const char *question = "[TEST QUESSTION.]";

#define WIN_MAINMENU 0
#define WIN_CREDITS 1
#define WIN_ANSWERBOX 2
#define WIN_QUESTIONBOX 3

WINDOW *windows[4] = { 0 };

void list_think( int c, uint32_t *selidx, int max ) {
    switch ( c ) {
        case KEY_UP:
            *selidx = ( *selidx == 0 ) ? *selidx : *selidx - 1;
            break;
        case KEY_DOWN:
            *selidx = ( ( *selidx + 1 ) == max ) ? *selidx : *selidx + 1;
            break;
    }
}

int main() {
    char *locale_str = setlocale( LC_ALL, "en_US.UTF-8" );
    if ( locale_str == NULL ) {
        perror( "setlocale failed" );
        return 1;
    }
    
    if ( init_ui() > 0 ) {
        perror( "init_ui failed" );
        return 1;
    }

    int sy = 0, sx = 0;
    getmaxyx( stdscr, sy, sx );

    for ( int i = 0; i < 2; i++ ) {
        windows[ i ] = newwin( sy, sx, 0, 0 );
        keypad( windows[ i ], true );
    }

    windows[ WIN_ANSWERBOX ] = newwin( sy - 5, sx, 5, 0 );
    keypad( windows[ WIN_ANSWERBOX ], true );

    windows[ WIN_QUESTIONBOX ] = newwin( 6, sx, 0, 0 );
    keypad( windows[ WIN_QUESTIONBOX ], true );
    
    // game state //
    uint32_t winidx = WIN_MAINMENU;
    uint32_t selidx = 0;
    uint32_t cselidx = 0;
    bool confirm = false;

    while ( !shouldClose ) {        
        int c = wgetch( windows[ winidx ] );

        switch ( winidx ) {
            case WIN_MAINMENU:
                list_think( c, &selidx, 3 );
                if ( c == 10 ) {
                    switch ( selidx ) {
                        case 2:
                            shouldClose = true;
                            break;
                        case 1:
                            winidx = WIN_CREDITS;
                            break;
                        case 0:
                            winidx = WIN_QUESTIONBOX;
                            break;
                    }
                    break;
                }
                draw_main_menu( windows[ winidx ], selidx );
                break;
            case WIN_CREDITS:
                if ( c == 10 ) {
                    winidx = WIN_MAINMENU;
                    break;
                }
                draw_credits( windows[ winidx ] );
                break;
            case WIN_ANSWERBOX:
                if ( confirm ) {
                    list_think( c, &cselidx, 2 );
                } else {
                    list_think( c, &selidx, 5 );
                }
                if ( c == 10 ) {
                    if ( !confirm ) {
                        confirm = true;
                        wclear( windows[ winidx ] );
                        wrefresh( windows[ winidx ] );
                    } else {
                        if ( cselidx == 0 ) {
                            confirm = false;
                            if ( selidx == 0 ) {
                                winidx = WIN_MAINMENU;
                                break;
                            }
                        } else {
                            cselidx = 0;
                            confirm = !confirm;
                        }
                    }
                }
                if (confirm) {
                    draw_answerbox( windows[ WIN_ANSWERBOX ], cselidx, confirm, opts_test, 5 );
                } else {
                    draw_answerbox( windows[ WIN_ANSWERBOX ], selidx, confirm, opts_test, 5 );
                }
                break;
            case WIN_QUESTIONBOX:
                draw_questionbox( windows[ WIN_QUESTIONBOX ], question );
                winidx = WIN_ANSWERBOX;
                break;
        }
    }
    
    endwin();
    return 0;
}