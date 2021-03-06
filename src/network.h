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


int     network_init(void);                     // init curl
void    network_cleanup(void);                  // shut down and clean up curl

void    network_response_cleanup(
            network_response *res);             // free the response struct;

network_response *network_post(                 // send post request to url
            const char *url,
            const char *data);

char    *network_escape(const char *source);    // urlencode a string

#endif
