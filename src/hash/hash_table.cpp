#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#define NVERIFY
#define LOG_NTRACE
#define LOG_NLEAK

#include "../../lib/logs/log.h"
#include "hash_table.h"

//--------------------------------------------------------------------------------------------------------------------------------

#define $h_data (h_tab->data)
#define $h_size (h_tab->size)
#define $h_calc (h_tab->hash_calc)
#define $h_cmp  (h_tab->key_cmp )

//--------------------------------------------------------------------------------------------------------------------------------
// ctor, dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_ctor(hash_table *const h_tab, const size_t h_size, hash_val (*h_calc)(hash_key elem),
                                                                   int      (*h_cmp )(hash_key fst, hash_key sec))
{
    log_verify(h_tab  != nullptr, false);
    log_verify(h_calc != nullptr, false);
    log_verify(h_cmp  != nullptr, false);

    $h_data = (chain *) log_calloc(h_size, sizeof(chain));
    if ($h_data == nullptr)
    {
        log_error("Can't allocate memory for hash_table.data\n");
        return false;
    }

    chain *const h_data = $h_data;
    for (size_t i = 0; i < h_size; ++i) chain_ctor(h_data + i, hash_key_dump);

    $h_size = h_size;
    $h_calc = h_calc;
    $h_cmp  = h_cmp ;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_table *hash_table_new(const size_t h_size, hash_val (*h_calc)(hash_key elem),
                                                int      (*h_cmp )(hash_key fst, hash_key sec))
{
    log_verify(h_calc != nullptr, nullptr);
    log_verify(h_cmp  != nullptr, nullptr);

    hash_table *h_tab = (hash_table *) log_calloc(1, sizeof(hash_table));
    if (h_tab == nullptr)
    {
        log_error("Can't allocate memory for hash_table\n");
        return nullptr;
    }

    if (!hash_table_ctor(h_tab, h_size, h_calc, h_cmp)) { log_free(h_tab); return nullptr; }
    return h_tab;
}

//--------------------------------------------------------------------------------------------------------------------------------

void hash_table_dtor(hash_table *const h_tab)
{
    if (h_tab == nullptr) return;

    size_t h_size = $h_size;
    chain *h_data = $h_data;

    for (size_t i = 0; i < h_size; ++i) chain_dtor(h_data + i);

    log_free(h_data);
}

//--------------------------------------------------------------------------------------------------------------------------------

void hash_table_delete(hash_table *const h_tab)
{
    hash_table_dtor(h_tab);
    log_free       (h_tab);
}

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_push(hash_table *const h_tab, hash_key key)
{
    log_verify(h_tab != nullptr, false);
    log_verify(key   != nullptr, false);

    hash_val index = ($h_calc(key)) % $h_size;

    if (chain_find($h_data + index, key, $h_cmp)) return true;

    return chain_push_front($h_data + index, key);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_push_forced(hash_table *const h_tab, hash_key key)
{
    log_verify(h_tab != nullptr, false);
    log_verify(key   != nullptr, false);

    hash_val index = ($h_calc(key)) % $h_size;

    return chain_push_front($h_data + index, key);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_find(const hash_table *const h_tab, hash_key key)
{
    log_verify(h_tab != nullptr, false);
    log_verify(key   != nullptr, false);

    hash_val index = ($h_calc(key)) % $h_size;

    return chain_find($h_data + index, key, $h_cmp);
}

//--------------------------------------------------------------------------------------------------------------------------------

size_t hash_table_chain_size(const hash_table *const h_tab, size_t ind)
{
    log_verify(h_tab != nullptr, 0);

    return chain_get_size($h_data + ind);
}

//--------------------------------------------------------------------------------------------------------------------------------

void hash_table_dump(const hash_table *const h_tab)
{
    log_verify(h_tab != nullptr, false);

    log_service_message("====================", "\n");

    for (size_t i = 0; i < $h_size; ++i)
    {
        log_tab_service_message("chain #%lu:", "\n", i);
        chain_dump($h_data + i);
    }

    log_service_message("====================", "\n");

}