/**
 * @file pocket.c
 * @brief interface to the pocket service
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-01-27
 */


#include "pocket.h"
#include "network.h"

/**
 * @name Internal procedures
 * @{ */
char* pocket_get_request_token();           // oauth step 1
/**  @} */









/**
 * @name main procedures
 * @{ */

int pocket_authorize() {

    char * req = "consumer_key=" POCKET_KEY 
                 "&redirect_uri=" "~/test.html";

    int ret = network_post(POCKET_URL, req);

    return ret;
}
/**  @} */

