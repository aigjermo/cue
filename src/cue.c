/**
 * @file cue.c
 * @brief minimal command line interface to the pocket service
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-01-27
 */

#include <stdio.h>
#include <string.h>

#include "pocket.h"
#include "network.h"
#include "storage.h"

/**
 * @name Internal declarations
 * @{ */

typedef int (*action)(int, char**);

action  parse_args          (int argc, char **argv);

int     cue_authorize       (int argc, char **argv);
int     cue_add_url         (int argc, char **argv);
void    cue_print_usage     (void);

/**  @} */


/**
 * @prog cue
 * @brief Minimal command line client to pocket
 */
int main (int argc, char **argv) {


    action todo = parse_args(argc, argv);
    if (!todo) {
        cue_print_usage();
        return 0;
    }

    if (storage_init()) {
        return 1;
    }
    if (network_init()) {
        DEBUGPRINT(0, "network functions could not be initialized");
        return 2;
    }

    int ret = (*todo)(argc, argv);

    network_cleanup();
    storage_cleanup();

    return ret;
}


/**
 * @name cue actions
 * @{ */

/**
 * @brief print usage info
 */
void cue_print_usage (void) {

    printf("Todo: helptext\n");
}


/**
 * @brief Authorize cue with pocket service
 *
 * @return 0 if successful
 */
int cue_authorize (int argc, char **argv) {

    return pocket_authorize();
}


/**
 * @brief Add a new url to pocket
 *
 * @return 0 if successful
 */
int cue_add_url (int argc, char **argv) {

    if (argc < 3) {
        cue_print_usage();
        return -1;
    }

    return pocket_add_url(argv[2], (argc<4) ? "" : argv[3]);
}

/**  @} */

/**
 * @name Utility functions
 * @{ */

/**
 * @brief look for a valid command for cue
 *
 * @param argc
 * @param argv
 *
 * @return pointer to the corresponding function or NULL
 */
action parse_args (int argc, char **argv) {

    if (argc <= 1)
        return NULL;

    char *comp = "";

    comp = "add";
    if (strstr(comp, argv[1]) == comp)
        return &cue_add_url;

    comp = "authorize";
    if (strstr(comp, argv[1]) == comp)
        return &cue_authorize;

    return NULL;
}

/**  @} */
