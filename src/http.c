#include "http.h"

typedef struct memory memory;

size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp) {
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

CURLcode http_request( const char *url, struct memory *mem, curl_write_callback write_callback ) {
    CURL *handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, mem);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    
    CURLcode success = curl_easy_perform(handle);
    
    curl_easy_cleanup(handle);
    
    return success;
}