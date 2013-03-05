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
#include "storage.h"
#include "network.h"

/**
 * @name Internal procedure declarations
 * @{ */
int pocket_get_request_token    (char **token);         // oauth step 1
int pocket_user_auth            (const char *token);    // oauth step 2
int pocket_get_access_token     (const char *token);    // oauth step 3
/**  @} */


/**
 * @name Exposed procedures
 * @{ */

/**
 * @brief Authorize app against pocket service
 *
 * @return 0 if successful
 */
int pocket_authorize(void) {

    DEBUGPRINT(1, "II:: requesting token from pocket...\n");

    char *token = NULL;

    if (pocket_get_request_token(&token))
        return 1;

    DEBUGPRINT(1, "II:: received request token: %s\n", token);

    if (pocket_user_auth(token))
        return 2;

    DEBUGPRINT(1, "II:: successfully redirected user..\n");

    if (pocket_get_access_token(token)) {
        return 3;
    }

    free(token);

    return 0;
}


/**
 * @brief add a url to pocket for reading later
 *
 * @param url       url to add
 * @param title     optional title of the page
 *
 * @return 0 if successful
 */
int pocket_add_url(char *url, char *title) {

    DEBUGPRINT(1, "II:: Adding url to pocket...\n");

    char *token = NULL;

    if (storage_retr_token(&token)) {
        DEBUGPRINT(0,"II:: No stored token, "
                "please authorize against pocket first\n");
        return 1;
    }

    char *u = network_escape(url);
    char *t = network_escape(title);
    if (!u || !t) {
        DEBUGPRINT(0,"EE:: pocket_add_url(): could not encode strings\n");
        return 2;
    }

    // length of the request string (including null byte)
    int len = strlen(POCKET_KEY) + strlen(token) + strlen(u) + strlen(t) + 40;
    char *data = malloc(len);

    sprintf(data, "url=%s&title=%s&consumer_key=%s&access_token=%s",
            u, t, POCKET_KEY, token);

    network_response *res = network_post(POCKET_URL "v3/add", data);
    if (!res) {
        return 3;
    }

    printf("%s\n", res->string);
    
    //free stuff
    network_response_cleanup(res);
    free (data);
    free (token);
    free (u);
    free (t);
    return 0;
}

/**  @} */

/**
 * @name Internal procedures
 * @{ */

/**
 * @brief Start the pocket authorize procedure by getting request_token
 *
 * @return request token from pocket
 */
int pocket_get_request_token(char **token) {

    char *req = "consumer_key=" POCKET_KEY
                "&redirect_uri=" POCKET_REDIRECT;

    network_response *res = network_post(POCKET_URL "v3/oauth/request", req); 
    if (!res) {
        return 1;
    }

    // extract data from response
    char *ptr = strstr(res->string, "code=");
    if (!ptr) {
        DEBUGPRINT(0, "EE:: got malformed response from pocket:\nEE:: -> %s\n", 
                res->string);
        network_response_cleanup(res);
        return 2;
    }

    ptr = ptr+5;
    int len = strlen(ptr) + 1;

    *token = malloc(len);
    if (!*token) {
        DEBUGPRINT(0, "EE:: malloc failed in pocket_get_request_token()\n");
        network_response_cleanup(res);
        return 3;
    }

    *token = strncpy(*token, ptr, len);

    // free memory
    network_response_cleanup(res);

    return 0;
}

/**
 * @brief Open a browser to the pocket authorize form
 *
 * @return 0
 */
int pocket_user_auth(const char *token) {

    printf("Press enter to open a browser and authorize cue");
    getchar(); // wait for user to confirm

    // try to open browers
    char cmd[200];
    sprintf(cmd, "xdg-open \"" POCKET_URL "auth/authorize?"
            "request_token=%s&redirect_uri=" POCKET_REDIRECT "\" 2>/dev/null",
            token);

    if (system(cmd)) {
        printf("could not open browser, please open the follong url and "
                "authorize cue, then come back here and press enter\n"
                "url: " POCKET_URL "auth/authorize?request_token=%s&"
                "redirect_uri=" POCKET_REDIRECT "\n", token);
    }
    else { 
        printf("Back from pocket? press enter to continue..\n");
    }
    getchar(); // again wait for user

    return 0;
}

/**
 * @brief Request an access token from pocket
 *
 * @return 0 if successful
 */
int pocket_get_access_token(const char *token) {

    if (!token) {
        DEBUGPRINT(0, "EE:: tried authorizing without a request token\n");
        return 1;
    }

    char req[100];
    sprintf(req, "consumer_key=" POCKET_KEY "&code=%s", token);
    
    network_response *res =
        network_post(POCKET_URL "v3/oauth/authorize", req);

    if (!res) {
        return 2;
    }

    // parse response
    char *rtoken = strstr(res->string, "access_token=");
    char *user = strstr(res->string, "username=");
    
    if (!rtoken || !user) {
        DEBUGPRINT(0, "EE:: got malformed response from pocket: %s\n",
                res->string);
        network_response_cleanup(res);
        return 3;
    }

    rtoken   += 13;
    user    += 9;

    char *ptr;
    while ((ptr = strrchr(res->string, '&')))
        *ptr = '\0';

    if (storage_store_token(rtoken)) {
        DEBUGPRINT(0, "EE:: failed to store token \n");
        network_response_cleanup(res);
        return 4;
    }

    // free memory
    network_response_cleanup(res);

    return 0;
}

/**  @} */
