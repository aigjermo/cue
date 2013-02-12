#ifndef STORAGE
#define STORAGE

#include "defaults.h"


int     storage_init (void);
void    storage_cleanup (void);
int     storage_store_token (const char *token);

#endif
