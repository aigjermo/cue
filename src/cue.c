/**
 * @file cue.c
 * @brief minimal command line interface to the pocket service
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-01-27
 */

#include <stdio.h>

#include "pocket.h"
#include "network.h"

/**
 * @brief print usage info
 */
void cue_print_usage () {

    printf("Todo: helptext\n");
}


/**
 * @prog cue
 * @brief Minimal command line client to pocket
 */
int main (int argc, char* argv[]) {

    cue_print_usage();

    if (network_init()) {
        DEBUGPRINT(0, "network functions could not be initialized");
        return 1;
    }

    pocket_authorize();

    network_cleanup();

    return 0;
}
