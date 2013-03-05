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

/**
 * @name Internal declarations
 * @{ */
CURL *curl;

size_t network_get_response (void *ptr, size_t size, size_t nmemb, void *out);
/**  @} */


/**
 * @name Exposed procedures
 * @{ */

/**
 * @brief initialize curl
 *
 * @return 0 if successful
 */
int network_init(void) {

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl) return 0;

    DEBUGPRINT(1, "EE:: curl wouldn't initialize");

    curl_global_cleanup();
    return -1;
}

/**
 * @brief close down curl
 */
void network_cleanup (void) {

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

/**
 * @brief Free a network response object
 *
 * @param response object to free
 */
void network_response_cleanup (network_response *res) {

    if (res) {
        if (res->string)
            free(res->string);

        free(res);
    }
}


/**
 * @brief send post request to given address
 *
 * @param url       the address
 * @param data      data to send
 *
 * @return          -1 if failed, http response code otherwise
 */
network_response *network_post(const char *url, const char *data) {

    if (!curl) {
        DEBUGPRINT(0, "EE:: curl has not been initialized\n");
        return NULL;   // init first
    }

    network_response *response = malloc(sizeof(response));
    if (response == NULL) {
        DEBUGPRINT(0, "EE:: malloc failed in network_post()\n");
        return NULL;
    }

    // clear response struct
    response->code = '\0';
    response->string = NULL;

    // setup query
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, network_get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    // execute
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        DEBUGPRINT(0, "EE:: tried connecting, but got error: %s\n",
                curl_easy_strerror(res));
        free(response);
        return NULL;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &(response->code));

    if (response->code != 200) {
        DEBUGPRINT(0, "EE:: pocket responded with: %s\n", response->string);
        free(response);
        return NULL;
    }

    return response;
}


/**
 * @brief URLencode a string
 *
 * @param dest      pointer to a string where the result will be stored
 * @param source    string to encode
 *
 * @return          pointer to the encoded string
 */
char *network_escape(const char *source) {

    if (!curl) {
        DEBUGPRINT(0, "EE:: curl has not been initialized\n");
        return NULL;
    }

    char *buf = curl_easy_escape(curl, source, 0);
    if (!buf) {
        DEBUGPRINT(0, "EE:: curl could not escape string\n");
        return NULL;
    }

    int len = strlen(buf);
    char *dest = malloc(len+1);
    if (!dest) {
        DEBUGPRINT(0, "EE:: network_escape(): Could not malloc\n");
        return NULL;
    }

    strncpy(dest, buf, len+1);
    curl_free(buf);

    return dest;
}
    

/**  @} */

/**
 * @name Utility functions
 * @{ */

/**
 * @brief Callback function for curl to drop it's data
 *
 * @param ptr       pointer to the incoming data
 * @param size      size of the datatype
 * @param nmemb     number of elements
 * @param out       pointer to the outgoing struct
 *
 * @return          number of bytes copied
 */
size_t network_get_response (void *ptr, size_t size, size_t nmemb, void *out) {

    size_t bsize = size * nmemb;
    network_response *resp = out;
    resp->string = malloc(bsize+size);
    if (resp->string == NULL) {
        DEBUGPRINT(0, "EE:: malloc failed in network_get_response()\n");
        return 0;
    }

    resp->string = memcpy(resp->string, ptr, bsize);
    *(resp->string + bsize) = '\0';

    DEBUGPRINT(2,"II:: got %zu bytes from pocket\n", bsize);

    return bsize;
}

/**  @} */
