#include <stdio.h>
#include <stdlib.h>

#include "../../lib/logs/log.h"
#include "type.h"

void hash_key_dump(const void *const _key)
{
    log_tab_message("%p\n"
                    "\"%s\"\n", _key, (hash_key *) _key);
}