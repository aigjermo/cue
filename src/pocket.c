/**
 * @file pocket.c
 * @brief interface to the pocket service
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-01-27
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pocket.h"
#include "network.h"

/**
 * @name Internal procedure declarations
 * @{ */
char *pocket_get_request_token();           // oauth step 1
int   pocket_user_auth();                   // oauth step 2
/**  @} */


/**
 * @brief oauth keys
 */
struct pocket_keys {

    char *request_token;
    char *access_token;
};

struct pocket_keys keys;

// Exposed procedures

/**
 * @brief Authorize app against pocket service
 *
 * @return 0 if successful
 */
int pocket_authorize() {

    DEBUGPRINT(1, "requesting token from pocket...\n");

    if (!pocket_get_request_token())
        return 1;

    DEBUGPRINT(1, "received token: %s\n", keys.request_token);

    if (pocket_user_auth())
        return 2;

    DEBUGPRINT(1, "successfully redirected user..\n");

    return 0;
}

void pocket_cleanup() {

    if (keys.request_token != NULL)
        free(keys.request_token);

    if (keys.access_token != NULL)
        free(keys.access_token);
}

// Internal procedures

/**
 * @brief Start the pocket authorize procedure by getting request_token
 *
 * @return request token from pocket
 */
char *pocket_get_request_token() {

    char *req = "consumer_key=" POCKET_KEY
                "&redirect_uri=" POCKET_REDIRECT;

    struct network_response *res = 
        network_post(POCKET_URL "v3/oauth/request", req);

    if (res == NULL) {
        return NULL;
    }

    // extract data from response
    char *ptr = strstr(res->string, "code=");
    if (ptr == NULL) {
        DEBUGPRINT(0, "got malformed response from pocket: %s\n", res->string);
        free(res);
        return NULL;
    }

    ptr = ptr+5;
    int len = strlen(ptr) + 1;

    keys.request_token = malloc(len);
    if (keys.request_token == NULL) {
        DEBUGPRINT(0, "malloc failed in pocket_get_request_token()\n");
        return NULL;
    }

    keys.request_token = strncpy(keys.request_token, ptr, len);

    // free memory
    network_response_cleanup(res);

    return keys.request_token;
}

/**
 * @brief Open a browser to the pocket authorize form
 *
 * @return 0
 */
int pocket_user_auth() {

    printf("Trying to open your browser so you can authorize me...\n");

    // try to open browers
    char cmd[150];
    sprintf(cmd, "xdg-open \"" POCKET_URL "auth/authorize?"
            "request_token=%s&redirect_uri=" POCKET_REDIRECT "\" 2>/dev/null",
            keys.request_token);

    system(cmd);

    printf("Back from pocket? press enter to continue..\n");
    getchar(); // we don't really care what the user writes

    return 0;
}

