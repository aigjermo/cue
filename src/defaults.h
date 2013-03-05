#ifndef DEFAULTS
#define DEFAULTS

/** debug printing to stderr that can be switched on
 *  by issuing -DDEBUG=1 (or higher) to gcc. 
 *  This is done by make targets debug and fulldebug
 */
#ifndef DEBUG
    #define DEBUG 0 // default to no debug printing
#endif

#define DEBUGPRINT(level, ...) \
    if (DEBUG >= level) fprintf(stderr, __VA_ARGS__)

#endif
