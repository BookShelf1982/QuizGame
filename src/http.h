#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <memory.h>
#include <curl/curl.h>

struct memory {
    char *response;
    size_t size;
};

typedef struct memory memory;

size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp);

CURLcode http_request( const char *url, struct memory *mem, curl_write_callback write_callback );

#endif // HTTP_H