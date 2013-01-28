#ifndef NETWORK
#define NETWORK

#include "defaults.h"

int     network_init();                 // init curl
void    network_cleanup();              // shut down and clean up curl

struct network_response *network_post(
            const char *url,
            const char *data);  // send post request to url


/**
 * @brief response from pocket
 */
struct network_response {
    int  code;
    char *string;
};


#endif
