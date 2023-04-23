#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#define NVERIFY
#define LOG_NTRACE
#define LOG_NLEAK

#include "hash_table.h"

#define $h_data (h_tab->data)
#define $h_size (h_tab->size)
#define $h_culc (h_tab->culc)
#define $h_cmp  (h_tab->cmp )

//--------------------------------------------------------------------------------------------------------------------------------
// ctor, dtor
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_ctor(hash_table *const h_tab, const size_t h_size, hash_val (*h_culc)(hash_key elem),
                                                                   bool     (*h_cmp )(hash_key fst, hash_key sec))
{
    log_verify(h_tab  != nullptr, false);
    log_verify(h_culc != nullptr, false);
    log_verify(h_cmp  != nullptr, false);

    $h_data = (list *) log_calloc(h_size, sizeof(list));
    if ($h_data == nullptr)
    {
        log_error("Can't allocate memory for hash_table.data\n");
        return false;
    }

    list *const h_data = $h_data;
    for (size_t i = 0; i < h_size; ++i) list_ctor(h_data + i, sizeof(hash_key));

    $h_size = h_size;
    $h_culc = h_culc;
    $h_cmp  = h_cmp ;

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_table *hash_table_new(const size_t h_size, hash_val (*h_culc)(hash_key elem),
                                                bool     (*h_cmp )(hash_key fst, hash_key sec))
{
    log_verify(h_culc != nullptr, nullptr);
    log_verify(h_cmp  != nullptr, nullptr);

    hash_table *h_tab = (hash_table *) log_calloc(1, sizeof(hash_table));
    if (h_tab == nullptr)
    {
        log_error("Can't allocate memory for hash_table\n");
        return nullptr;
    }

    if (!hash_table_ctor(h_tab, h_size, h_culc, h_cmp)) { log_free(h_tab); return nullptr; }
    return h_tab;
}

//--------------------------------------------------------------------------------------------------------------------------------

void hash_table_dtor(hash_table *const h_tab)
{
    if (h_tab == nullptr) return;

    size_t h_size = $h_size;
    list  *h_data = $h_data;
    for (int i = 0; i < h_size; ++i) list_dtor(h_data + i);

    log_free(h_data);
}

//--------------------------------------------------------------------------------------------------------------------------------

void hash_table_free(hash_table *const h_tab)
{
    hash_table_dtor(h_tab);
    log_free       (h_tab);
}

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_push(hash_table *const h_tab, hash_key elem)
{
    log_verify(h_tab != nullptr, false);

    hash_val index = ((*$h_culc)(elem)) % $h_size;

    return list_push_front($h_data + index, &elem);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_find(hash_table *const h_tab, hash_key elem)
{
    log_verify(h_tab != nullptr, false);

    hash_val index = ((*$h_culc)(elem)) % $h_size;
    bool (*h_cmp)(hash_key fst, hash_key sec) = $h_cmp;

    list  *dup_list = $h_data + index;
    size_t dup_num  = dup_list-> size;

    for (size_t i = 0; i < dup_num; ++i)
    {
        hash_t   dup_key = {};
        list_get(dup_list, i, &dup_key);

        if (h_cmp(elem, (hash_key) dup_key)) return true;
    }

    return false;
}