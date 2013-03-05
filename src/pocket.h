#ifndef POCKET
#define POCKET

#include "defaults.h"


#define POCKET_URL      "https://getpocket.com/"
#define POCKET_REDIRECT "~/test.html"
#define POCKET_KEY      "11706-e0ddf4a044fc1629613f1abc"


int     pocket_authorize (void);                    // authorize with pocket
int     pocket_add_url   (char *url, char *title);  // add page to pocket
void    pocket_cleanup   (void);

#endif
