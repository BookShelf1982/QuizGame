#include "ui.h"

static const char *main_menu_opts[3] = {
    "Start New Game",
    "Credits",
    "Quit"
};

static const char *credits_str[] = { 
    "A game by Marcus Steinke.",
    "Powered by OpenTriviaDB.",
    "",
    "Thank you for playing!",
    "",
    " Go back (ENTER) "
};

const char *opts_test[] = {
    "exit to main menu",
    "A: A Butterfly",
    "A: A Snowbug",
    "A: A Tulip",
    "A: A Bum",
};

const char *yesno[] = {
    "Yes",
    "No"
};

int init_ui() {
    char *locale_str = setlocale( LC_ALL, "en_US.UTF-8" );
    if ( locale_str == NULL ) {
        perror( "setlocale failed" );
        return 1;
    }

    initscr();
    noecho();
    cbreak();
    halfdelay( 2 );
    curs_set( 0 );

    return 0;
}

void draw_answerbox( WINDOW *win, uint32_t selidx, bool confirm, const char *questions[], uint32_t size ) {
    wborder( win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_LTEE, ACS_RTEE, ACS_LLCORNER, ACS_LRCORNER );
    if ( !confirm ) {
        for ( int i = 0; i < size; i++ ) {
            if (  i ==  selidx ) {
                wattron( win, A_REVERSE );
                mvwprintw( win, i + 1, 1, " > %s",  questions[ i ] );
                wattroff( win, A_REVERSE );
            } else {
                mvwprintw( win, i + 1, 1, "%s   ", questions[ i ] );
            }
        }
    } else {
        for ( int i = 0; i < 2; i++ ) {
            if (  i ==  selidx ) {
                wattron( win, A_REVERSE );
                mvwprintw( win, i + 1, 1, " > %s",  yesno[ i ] );
                wattroff( win, A_REVERSE );
            } else {
                mvwprintw( win, i + 1, 1, "%s   ", yesno[ i ] );
            }
        }
    }
}

void draw_questionbox( WINDOW *win, const char *question ) {
    wborder( win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LTEE, ACS_RTEE );
    mvwprintw( win, 1, 1, "%s", question );
    wrefresh( win );
}

void draw_quizbox( WINDOW *win ) {
    int mx = getmaxx( win );

    box( win, ACS_VLINE, ACS_HLINE );
    mvwhline( win, 4, 0, ACS_HLINE, mx );
    mvwaddch( win, 4, 0, ACS_LTEE );
    mvwaddch( win, 4, mx - 1, ACS_RTEE );
    mvwprintw( win, 0, 1, " Quiz Box " );
}

void draw_questions( WINDOW *win, uint32_t selidx, const char *questions[], uint32_t size ) {
    for ( int i = 0; i < size; i++ ) {
        if (  i ==  selidx ) {
            wattron( win, A_REVERSE );
            mvwprintw( win, i, 1, " > %s",  questions[ i ] );
            wattroff( win, A_REVERSE );
        } else {
            mvwprintw( win, i, 1, "%s   ", questions[ i ] );
        }
    }
}

void draw_confirm( WINDOW *win, uint32_t selidx ) {
    mvwprintw( win, 5, 1, " Is answer %d your final answer?", selidx );
}

void draw_credits( WINDOW *win ) {
    box( win, ACS_VLINE, ACS_HLINE );
    mvwprintw( win, 0, 1, " Credits " );
    for ( int i = 0; i < 6; i++ ) {
        mvwprintw( win, 1 + i, 1, "%s", credits_str[ i ] );
    }
}

void draw_main_menu( WINDOW *win, uint32_t selidx ) {
    box( win, ACS_VLINE, ACS_HLINE );
    mvwprintw (win, 0, 1, " The Quiz Game " );

    for ( int i = 0; i < 3; i++ ) {
        if (  i ==  selidx ) {
            wattron( win, A_REVERSE );
            mvwprintw( win, i + 1, 1, " > %s", main_menu_opts[ i ] );
            wattroff( win, A_REVERSE );
        } else {
            mvwprintw( win, i + 1, 1, "%s   ", main_menu_opts[ i ] );
        }
    }
}