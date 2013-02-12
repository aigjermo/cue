#ifndef STORAGE
#define STORAGE

#include "defaults.h"


int     storage_init ();
void    storage_cleanup ();
int     storage_store_token (const char *token);
int     storage_retr_token  (char **dest);

#endif
