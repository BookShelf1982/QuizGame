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

#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <memory.h>
#include <curl/curl.h>

struct memory {
    char *response;
    size_t size;
};

// size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp);

CURLcode http_request( const char *url, struct memory *mem );

#endif // HTTP_H