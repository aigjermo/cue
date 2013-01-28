/**
 * @file network.c
 * @brief network functions for interfacing with pocket
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-01-27
 */

#include <curl/curl.h>
#include <stdlib.h>     //malloc
#include <string.h>     //memcpy

#include "network.h"

CURL *curl;

/**
 * @brief initialize curl
 *
 * @return 0 if successful
 */
int network_init() {

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl) return 0;

    DEBUGPRINT(1, "failed to initialize curl");

    curl_global_cleanup();
    return -1;
}

/**
 * @brief close down curl
 */
void network_cleanup() {

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}


size_t network_get_response(void *ptr, size_t size, size_t nmemb, void *out) {

    size_t bsize = size * nmemb;
    struct network_response *resp = out;
    resp->string = malloc(bsize+size);
    if (resp->string == NULL) {
        DEBUGPRINT(0, "realloc failed\n");
        return 0;
    }

    resp->string = memcpy(resp->string, ptr, bsize);
    *(resp->string + bsize) = '\0';

    DEBUGPRINT(1,"got %zu bytes from pocket\n", bsize);

    return bsize;
}

/**
 * @brief send post request to given address
 *
 * @param url       the address
 * @param data      data to send
 *
 * @return          -1 if failed, http response code otherwise
 */
struct network_response *network_post(const char *url, const char *data) {

    if (!curl) return NULL;   // init first

    struct network_response *response = malloc(sizeof(response));
    if (response == NULL) {
        DEBUGPRINT(1, "malloc failed\n");
        return NULL;
    }

    // setup query
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, network_get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    // execute
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        DEBUGPRINT(0, "post request failed with error: %s\n",
                curl_easy_strerror(res));
    }

    response->code = res;

    return response;
}
