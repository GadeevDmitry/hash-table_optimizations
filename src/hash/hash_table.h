#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "../../lib/list/list.h"
#include "type.h"

//================================================================================================================================
// STRUCT
//================================================================================================================================

struct hash_table
{
    list   *data;
    size_t  size;

    hash_val (*culc)(hash_key elem);
    int      (*cmp )(hash_key first, hash_key second);
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor, dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_ctor(hash_table *const h_tab, const size_t h_size, hash_val (*h_culc)(hash_key elem),
                                                                   int      (*h_cmp )(hash_key fst, hash_key sec));
hash_table *hash_table_new(                   const size_t h_size, hash_val (*h_culc)(hash_key elem),
                                                                   int      (*h_cmp )(hash_key fst, hash_key sec));
void hash_table_dtor(hash_table *const h_tab);
void hash_table_free(hash_table *const h_tab);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_push(      hash_table *const h_tab, hash_key elem);
bool hash_table_find(const hash_table *const h_tab, hash_key elem);

#endif //HASH_TABLE_H