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

#include "http.h"

static size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp) {
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)clientp;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(!ptr)
      return 0;  // out of memory

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

CURLcode http_request( const char *url, struct memory *mem ) {
    CURL *handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, mem);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    
    CURLcode success = curl_easy_perform(handle);
    
    curl_easy_cleanup(handle);
    
    return success;
}