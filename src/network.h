#ifndef NETWORK
#define NETWORK

#include "defaults.h"

/**
 * @brief http response from pocket
 */
typedef struct {
    long code;
    char *string;
} network_response;


int     network_init();                 // init curl
void    network_cleanup();              // shut down and clean up curl
void    network_response_cleanup(
            network_response *res);     // free the response struct;

network_response *network_post(
            const char *url,
            const char *data);          // send post request to url


#endif
