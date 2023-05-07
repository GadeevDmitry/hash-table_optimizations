#ifndef HASH_H
#define HASH_H

#include <string.h>
#include "type.h"

//--------------------------------------------------------------------------------------------------------------------------------

hash_val hash_trivial   (hash_key elem);
hash_val hash_first_char(hash_key elem);
hash_val hash_len       (hash_key elem);
hash_val hash_sum_char  (hash_key elem);
hash_val hash_ror       (hash_key elem);
hash_val hash_rol       (hash_key elem);
hash_val hash_crc32     (hash_key elem);

#endif //HASH_H
