/*
    Copyright (C) 2025 Marcus Steinke.

    This file is part of QuizGame.

    QuizGame is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QuizGame is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License 
    along with QuizGame. If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include "http.h"
#include "cJSON.h"

enum MainMenuState {
    MAIN_MENU_STATE_MENU,
    MAIN_MENU_STATE_CREDITS
};

struct MainMenu {
    uint32_t selidx;
    enum MainMenuState state;
};

#define QUESTION_COUNT 10

struct QnAData {
    char *question;
    char **answers;
    uint32_t answer_count;
    uint32_t correct;
};

struct QuizBox {
    uint32_t selidx;
    struct QnAData qna;
} quiz_box;

struct Game {
    uint32_t score;
} game;

int cy = 0, cx = 0;

// https://stackoverflow.com/questions/42321370/fisher-yates-shuffling-algorithm-in-c
void shuffle_answers( char **answers, uint32_t count, uint32_t *correct ) {
    int i, j, c = 0;
    char *tmp;
    
    for ( i = count - 1; i > 0; i-- ) {
        j = rand() % ( i + 1 );
        if ( c == j )
            c = i;

        tmp = answers[ j ];
        answers[ j ] = answers[ i ];
        answers[ i ] = tmp;
    }

    *correct = c;
}

int get_questions( uint32_t count, struct QnAData **data ) {
    char url[40] = "https://opentdb.com/api.php?amount="; // 4 extra cahracters for answer count
    snprintf( &url[ 34 ], 4, "=%d", count );
    struct QnAData *result = NULL;

    struct memory m = { 0 };
    CURLcode success = http_request( url, &m );
    if ( success > 0 ) {
        return 1;
    }

    // parse json
    cJSON *json = cJSON_Parse( m.response );
    if ( json == NULL ) {
        return 8;
    }

    if ( !cJSON_IsObject( json ) )
        return 2;
    
    // get result code
    cJSON *response_code = cJSON_GetObjectItem( json, "response_code" );
    if ( cJSON_IsNumber( response_code ) ) {
        double response_num =  cJSON_GetNumberValue( response_code ); 
        if ( response_num > 0.0 ) {
            fprintf( stderr, "response code: %f\n", response_num );
            return 3;
        }
    }

    // get all qna data in the results array
    cJSON *results_array = cJSON_GetObjectItem( json, "results" );
    if ( cJSON_IsArray( results_array ) ) {
        int size = cJSON_GetArraySize( results_array );
        result = ( struct QnAData * ) malloc( sizeof( struct QnAData ) * size );
        for ( int i = 0; i < size; i++ ) {
            cJSON *object = cJSON_GetArrayItem( results_array, i );
            if ( !cJSON_IsObject( object ) ) {
                free( result );
                return 4;
            }

            cJSON *question = cJSON_GetObjectItem( object, "question" );
            if ( !cJSON_IsString( question ) ) {
                free( result );
                return 5;
            }
            
            cJSON *correct_answer = cJSON_GetObjectItem( object, "correct_answer" );
            if ( !cJSON_IsString( correct_answer ) ) {
                free( result );
                return 6;
            }

            cJSON *incorrect_answers = cJSON_GetObjectItem( object, "incorrect_answers" );
            if ( !cJSON_IsArray( incorrect_answers ) ) {
                free( result );
                return 7;
            }

            int answer_count = cJSON_GetArraySize( incorrect_answers ) + 1;
            result[ i ].answer_count = answer_count;
            result[ i ].answers = ( char ** ) malloc( sizeof( char ** ) * answer_count );

            result[ i ].answers[ 0 ] = strdup( cJSON_GetStringValue( correct_answer ) );

            int incorrect_count = answer_count - 1;
            for ( int j = 0; j < incorrect_count; j++ ) {
                result[ i ].answers[ j + 1 ] = strdup( cJSON_GetStringValue( cJSON_GetArrayItem( incorrect_answers, j ) ) );
            }

            if ( answer_count > 2 )
                shuffle_answers( result[ i ].answers, result[ i ].answer_count, &result[ i ].correct );


            result[ i ].question = strdup( cJSON_GetStringValue( question ) );
        }
    }

    cJSON_Delete( json );
    free( m.response );

    *data = result;

    return 0;
}

int list_think( int c, uint32_t *selidx, int max ) {
    switch ( c ) {
        case KEY_UP:
            *selidx = ( *selidx == 0 ) ? *selidx : *selidx - 1;
            return 1;
        case KEY_DOWN:
            *selidx = ( ( *selidx + 1 ) == max ) ? *selidx : *selidx + 1;
            return 1;
    }
    
    return 0;
}

void draw_answerbox( WINDOW *win, uint32_t selidx, const char *answers[], uint32_t count ) {
    move( getcury( stdscr ) + 1, 1 );
    for ( uint32_t i = 0; i < count; i++ ) {
        if ( selidx == i) {
            attron(A_REVERSE);
            wprintw( win, ">  %s", answers[ i ] );
            attroff(A_REVERSE);
        } else {
            wprintw( win, "%s   ", answers[ i ] );
        }
        move( getcury( stdscr ) + 1, 1 );
    }
}

void draw_questionbox( WINDOW *win, const char *question, int *y, int *x ) {
    move( 1, 1 );
    wprintw( win, "%s", question );
    *y = getcury( win );
    *x = getcurx( win );
}

void run( struct QnAData *data, uint32_t *idx ) {
    while ( 1 ) {
        int c = getch();

        if ( *idx > QUESTION_COUNT - 1 )
            return;

        if ( list_think( c, &quiz_box.selidx, quiz_box.qna.answer_count ) > 0 ) {
            move( cy, cx );
            draw_answerbox( stdscr, quiz_box.selidx, ( ( const char ** ) quiz_box.qna.answers ), quiz_box.qna.answer_count );
            continue;
        } else if ( c == KEY_ENTER || c == '\r' || c == '\n' ) {
            ( *idx )++;
            clear();
            if ( quiz_box.selidx == quiz_box.qna.correct ) {
                game.score++;
                mvwprintw( stdscr, 0, 1, "Score: %d",  game.score );
                move( 1, 1 );
                printw( "You are " );
                attron( COLOR_PAIR( 1 ) );
                printw( "correct" );
                attroff( COLOR_PAIR( 1 ) );
                printw( "!" );

                while ( 1 ) {
                    int c2 = getch();
                    if ( c2 == KEY_ENTER || c2 == '\r' || c2 == '\n' )
                        break;
                }
            } else {
                mvwprintw( stdscr, 0, 1, "Score: %d",  game.score );
                move( 1, 1 );
                printw( "You are " );
                attron( COLOR_PAIR( 2 ) );
                printw( "incorrect" );
                attroff( COLOR_PAIR( 2 ) );
                printw( "." );
                
                while ( 1 ) {
                    int c2 = getch();
                    if ( c2 == KEY_ENTER || c2 == '\r' || c2 == '\n' )
                        break;
                }
            }
            if ( *idx > QUESTION_COUNT - 1  )
                return;

            quiz_box.qna = data[ *idx ];
            quiz_box.selidx = 0;

            box( stdscr, ACS_VLINE, ACS_HLINE );
            draw_questionbox( stdscr, quiz_box.qna.question, &cy, &cx );
            draw_answerbox( stdscr, quiz_box.selidx, ( ( const char ** ) quiz_box.qna.answers ), quiz_box.qna.answer_count );
            mvwprintw( stdscr, 0, 1, "Score: %d",  game.score );
            mvwprintw( stdscr, 0, 35, "Questions Answered: %d / %d",  *idx, QUESTION_COUNT );
        }
    }
}

void free_qnadata( struct QnAData *data ) {
    for ( int j = 0; j < data->answer_count; j++ ) {
        free( data->answers[ j ] );
    }
    free( data->question );
    free( data->answers );
}

int main() {
    srand( time( NULL ) );
    initscr();
    keypad( stdscr, true );
    cbreak();
    noecho();
    curs_set( 0 );

    if ( has_colors() == FALSE ) {
        endwin();
        perror( "Your terminal doesn't support colors." );
        return 1;
    }

    start_color();
    init_pair( 1, COLOR_GREEN, COLOR_BLACK);
    init_pair( 2, COLOR_RED, COLOR_BLACK);
    
    uint32_t qidx = 0;
    struct QnAData *data;

    int result = get_questions( QUESTION_COUNT, &data );
    if ( result > 0 ) {
        endwin();
        fprintf( stderr, "Failed to get questions and answers. Err: %d\n", result );
        return 1;
    }
    quiz_box.qna = data[ qidx ];

    box( stdscr, ACS_VLINE, ACS_HLINE );
    draw_questionbox( stdscr, quiz_box.qna.question, &cy, &cx );
    draw_answerbox( stdscr, quiz_box.selidx, ( ( const char ** ) quiz_box.qna.answers ), quiz_box.qna.answer_count );
    mvwprintw( stdscr, 0, 1, "Score: %d",  game.score );
    mvwprintw( stdscr, 0, 35, "Question: %d / %d",  qidx, QUESTION_COUNT );
    run( data, &qidx );

    clear();
    printw( "You got %d out of %d correct!\nPress enter to close game...", game.score, QUESTION_COUNT );
    while ( 1 ) {
        int c = getch();
        if ( c != KEY_ENTER || c != '\r' || c != '\n' )
            break;
    }

    for ( int i = 0; i < QUESTION_COUNT; i++ ) {
        free_qnadata( &data[ i ] );
    }

    endwin();
    return 0;
}