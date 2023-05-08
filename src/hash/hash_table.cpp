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

static bool hash_list_find(const hash_table *const h_tab, const size_t index, hash_key elem);

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

    $h_data = (list *) log_calloc(h_size, sizeof(list));
    if ($h_data == nullptr)
    {
        log_error("Can't allocate memory for hash_table.data\n");
        return false;
    }

    list *const h_data = $h_data;
    for (size_t i = 0; i < h_size; ++i) cache_list_ctor(h_data + i, hash_key_dump);

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
    list  *h_data = $h_data;

    for (size_t i = 0; i < h_size; ++i) cache_list_dtor(h_data + i);

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

buffer *hash_table_rebuild(hash_table *const h_tab, const size_t dictionary_size)
{
    log_verify(h_tab       != nullptr, nullptr);
    log_verify(lexis_array != nullptr, nullptr);

    buffer *new_dictionary = buffer_new(dictionary_size);
    log_verify(new_dictionary != nullptr, nullptr);

    size_t table_size = h_tab->size;

    for (size_t chain = 0; chain < table_size; ++chain)
    {
        list_node *coll_fict = h_tab->data[chain].fictional;
        list_node *coll_node = coll_fict + coll_fict->next;

        while (coll_node != coll_fict)
        {
            void *cur_buff_pos = new_dictionary->buff_pos;
            buffer_write(new_dictionary, coll_node->data, strlen((const char *) coll_node->data) + 1);

            coll_node->data = cur_buff_pos;
            coll_node       = coll_fict + coll_node->next;
        }
    }

    return new_dictionary;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_push(hash_table *const h_tab, hash_key elem)
{
    log_verify(h_tab != nullptr, false);
    log_verify(elem  != nullptr, false);

    hash_val index = ($h_calc(elem)) % $h_size;

    if (hash_list_find(h_tab, index, elem)) return true;

    return cache_list_push_front($h_data + index, elem);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_push_forced(hash_table *const h_tab, hash_key elem)
{
    log_verify(h_tab != nullptr, false);
    log_verify(elem  != nullptr, false);

    hash_val index = ($h_calc(elem)) % $h_size;

    return cache_list_push_front($h_data + index, elem);
}

//--------------------------------------------------------------------------------------------------------------------------------

bool hash_table_find(const hash_table *const h_tab, hash_key elem)
{
    log_verify(h_tab != nullptr, false);
    log_verify(elem  != nullptr, false);

    hash_val index = ($h_calc(elem)) % $h_size;

    return hash_list_find(h_tab, index, elem);
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool hash_list_find(const hash_table *const h_tab, const size_t index, hash_key elem)
{
    log_verify(h_tab != nullptr, false);
    log_verify(index <  $h_size, false);

    int (*h_cmp)(hash_key fst, hash_key sec) = $h_cmp;

    list_node *dup_fict = $h_data[index].fictional;
    list_node *dup_node = dup_fict + dup_fict->next;

    while (dup_node != dup_fict)
    {
        if (h_cmp((hash_key)(dup_node->data), elem) == 0) return true;
        dup_node = dup_fict + dup_node->next;
    }

    return false;
}