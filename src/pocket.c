/**
 * @file pocket.c
 * @brief interface to the pocket service
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-01-27
 */

#include <stdio.h>

#include "pocket.h"
#include "network.h"

/**
 * @name Internal procedure declarations
 * @{ */
char *pocket_get_request_token();           // oauth step 1
/**  @} */


/**
 * @brief oauth keys
 */
struct pocket_keys {

    char *request_token;
    char *access_token;
};






/**
 * @name main procedures
 * @{ */

int pocket_authorize() {

    printf("received: %s\n", pocket_get_request_token());

    return 0;
}
/**  @} */


/**
 * @name internal procedures
 * @{ */


/**
 * @brief step 1 of the oauth process
 *
 * @return request token from pocket
 */
char *pocket_get_request_token() {

    char * req = "consumer_key=" POCKET_KEY 
                 "&redirect_uri=" "~/test.html";

    struct network_response *res = network_post(POCKET_URL, req);

    if (res == NULL) {
        DEBUGPRINT(0,"network request failed\n"); 
        return "";
    }

    return res->string;
}
/**  @} */
