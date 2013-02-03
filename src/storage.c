/**
 * @file storage.c
 * @brief Handle reading and writing of config and data
 * @author Anders Iver Gjermo
 * @version 0.1
 * @date 2013-02-03
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "storage.h"

/**
 * @name Internal procedure declarations
 * @{ */
int setStorageDirs ();

// utility declarations
int getDir (char **dest, const char *v, char *def);
/**  @} */


struct {
    char *conf;
    char *data;
} storageDirs;



/**
 * @name Exposed procedures
 * @{ */

/**
 * @brief init paths and read configuration
 *
 * @return 0 if okay
 */
int storage_init () {

    if(setStorageDirs ()) {
        DEBUGPRINT(0, "EE:: failed to decide data/config directories\n");
        return 1;
    }
    DEBUGPRINT(2, "II:: Storage initialized\n"
            "II:: config dir: %s\n"
            "II:: data dir:   %s\n",
            storageDirs.conf, storageDirs.data);

    return 0;
}

/**
 * @brief Free memory from configuration
 */
void storage_cleanup () {

    if (storageDirs.conf) free(storageDirs.conf);
    if (storageDirs.data) free(storageDirs.data);
}


/**
 * @brief Store an access token
 *
 * @param token
 *
 * @return 0 if everyting worked out
 */
int storage_store_token (const char *token) {


    return 0;
}

/**  @} */

/**
 * @name Internal procedures
 * @{ */

/**
 * @brief figure out where stuff should be located
 *
 * @return 0 if okay
 */
int setStorageDirs () {

    if (getDir(&(storageDirs.conf), "XDG_CONFIG_HOME", "/.config"))
        return 1;

    if (getDir(&(storageDirs.data), "XDG_DATA_HOME", "/.local/share"))
        return 2;

    return 0;
}

/**  @} */
/**
 * @name Utility functions
 * @{ */

/**
 * @brief Get an XDG-path, or create one based on default
 *
 * @param dest      where to write the path
 * @param v         environment variable to use
 * @param def       fall back in case variable is not set
 *
 * @return 0 if okay
 */
int getDir (char **dest, const char *v, char *def) {

    char *defpath = "";
    char *cuepath = "/cue";

    char *base = getenv(v);
    if (!base) {
        base = getenv("HOME");
        if (!base) {
            DEBUGPRINT(0, "EE:: $HOME not set\n");
            return 1;   // this should be very unlikely
        }
        defpath = def; // insert default path instead
    }

    int length = strlen(base) + strlen(defpath) + strlen(cuepath) + 1;
    *dest = malloc(length);
    if (!*dest) {
        DEBUGPRINT(0, "EE:: malloc failed in getDir()\n");
        return 2;
    }

    sprintf(*dest, "%s%s%s", base, defpath, cuepath);
    return 0;
}

/**  @} */
