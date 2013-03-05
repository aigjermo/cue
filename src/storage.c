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
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "storage.h"

/**
 * @name Internal procedure declarations
 * @{ */
int setStorageDirs (void);

// utility declarations
int getDir          (char **dest, const char *v, char *def);
int mkDir           (const char *path);
int constructPath   (char **dest, const char *f, const char *d);
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
int storage_init (void) {

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
void storage_cleanup (void) {

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

    if (!storageDirs.conf) {
        DEBUGPRINT(0,"EE:: storage_store_token() before storage_init()\n");
        return 1;
    }

    char *filename = NULL;
    if (constructPath(&filename, "token", storageDirs.conf)) {
        return 2;
    }

    FILE *f = fopen(filename, "w");
    free(filename);
    if (!f) {
        DEBUGPRINT(0,"EE:: storage_store_token() "
                "could not open file for writing: %s\n", strerror(errno));
        return 3;
    }

    fprintf(f, "%s", token);
    fclose(f);

    return 0;
}


int storage_retr_token (char **dest) {

    if (!storageDirs.conf) {
        DEBUGPRINT(0,"EE:: storage_retr_token() before storage_init()\n");
        return 1;
    }

    char *filename = NULL;
    if (constructPath(&filename, "token", storageDirs.conf)) {
        return 2;
    }

    FILE *f = fopen(filename, "r");
    free(filename);
    if (!f) {
        DEBUGPRINT(1,"EE:: storage_retr_token() "
                "could not open file for reading: %s\n", strerror(errno));
        return 3;
    }

    if (fseek(f, 0L, SEEK_END)) {
        DEBUGPRINT(0,"EE:: storage_retr_token() "
                "could not seek to end of file: %s\n", strerror(errno));
        fclose(f);
        return 4;
    }

    size_t size = ftell(f);
    fseek(f, 0L, SEEK_SET);
    if (size == 0) {
        DEBUGPRINT(1,"EE:: storage_retr_token() file is empty\n");
        fclose(f);
        return 5;
    }
    
    *dest = malloc(size + 1);
    if (!*dest) {
        DEBUGPRINT(0,"EE: storage_retr_token() malloc failed\n");
        fclose(f);
        return 6;
    }

    size_t cnt;
    if ((cnt = fread(*dest, 1, size, f)) < size) {
        DEBUGPRINT(0,"EE: storage_retr_token() "
                "got too few bytes from fread: %zu\n", cnt);
        fclose(f);
        free(*dest);
        return 7;
    }

    *((*dest)+size) = '\0';
    fclose(f);
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
int setStorageDirs (void) {

    if (getDir(&(storageDirs.conf), "XDG_CONFIG_HOME", "/.config"))
        return 1;

    if (getDir(&(storageDirs.data), "XDG_DATA_HOME", "/.local/share"))
        return 2;

    if (mkDir(storageDirs.conf))
        return 3;

    if (mkDir(storageDirs.data))
        return 4;

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


/**
 * @brief Create a directory if it doesn't exist
 *
 * @param path      path of the directory
 *
 * @return 0 if okay
 */
int mkDir (const char *path) {

    struct stat sb;
    if (!stat(path, &sb) && S_ISDIR(sb.st_mode)) {
        DEBUGPRINT(2,"II:: mkDir(): %s exists\n", path);
        return 0;
    }

    if (mkdir(path, S_IRWXU)) {
        DEBUGPRINT(0,"EE:: mkdir() failed: %s\n", strerror(errno));
        return 1;
    }

    DEBUGPRINT(2,"II:: created directory %s\n", path);
    return 0;
}


/**
 * @brief Create path from directory and filename
 *
 * @param dest      Where to store result
 * @param f         Filename
 * @param d         Directory path
 *
 * @return 0 if okay
 */
int constructPath (char **dest, const char *f, const char *d) {

    if (!f || !d) {
        DEBUGPRINT(0, "EE:: bad path or filename in constructPath\n");
        return 1;
    }

    *dest = malloc(strlen(f) + strlen(d) + 2);
    if (!*dest) {
        DEBUGPRINT(0, "EE:: malloc failed in constructPath()\n");
        return 2;
    }

    sprintf(*dest, "%s/%s", d, f);
    return 0;
}

/**  @} */
