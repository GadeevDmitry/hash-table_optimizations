#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "../chain/chain.h"
#include "type.h"

//================================================================================================================================
// STRUCT
//================================================================================================================================

struct hash_table
{
    chain  *data;
    size_t  size;

    hash_val (*hash_calc)(hash_key elem);
    int      (*key_cmp )(hash_key first, hash_key second);
};

//--------------------------------------------------------------------------------------------------------------------------------
// ctor, dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_ctor(hash_table *const h_tab, const size_t h_size, hash_val (*h_calc)(hash_key elem),
                                                                   int      (*h_cmp )(hash_key fst, hash_key sec));
hash_table *hash_table_new(                   const size_t h_size, hash_val (*h_calc)(hash_key elem),
                                                                   int      (*h_cmp )(hash_key fst, hash_key sec));
void hash_table_dtor  (hash_table *const h_tab);
void hash_table_delete(hash_table *const h_tab);

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_push        (      hash_table *const h_tab, hash_key elem);
bool hash_table_push_forced (      hash_table *const h_tab, hash_key elem);
bool hash_table_find        (const hash_table *const h_tab, hash_key elem);
void hash_table_dump        (const hash_table *const h_tab);
size_t hash_table_chain_size(const hash_table *const h_tab, size_t    ind);

#endif //HASH_TABLE_H