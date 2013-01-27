/**
 * @file network.c
 * @brief network functions for interfacing with pocket
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-01-27
 */

#include <curl/curl.h>

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


/**
 * @brief send post request to given address
 *
 * @param url       the address
 *
 * @return          -1 if failed, http response code otherwise
 */
int network_post(const char *url, const char *data) {

    if (!curl) return -1;   // init first

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);

    return res;
}
