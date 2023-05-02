#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define NDEBUG
#define NVERIFY
#define LOG_NTRACE
#define LOG_NLEAK
//#define CALLGRIND_MODE

#include "../lib/algorithm/algorithm.h"
#include "../lib/logs/log.h"

#include "hash/type.h"
#include "hash/hash.h"
#include "hash/hash_table.h"

#ifdef CALLGRIND_MODE
#include <valgrind/callgrind.h>
#endif

//--------------------------------------------------------------------------------------------------------------------------------
// SETTINGS
//--------------------------------------------------------------------------------------------------------------------------------

const size_t HASH_TABLE_SIZE = 1907;
const char  *HASH_TABLE_TEXT = "data/dictionary.txt";

int        (*HASH_TABLE_CMP ) (hash_key fst, hash_key sec) = strcmp;
hash_val   (*HASH_TABLE_CALC) (hash_key elem)              = hash_crc32;

const int RUN_SEARCH_NUM      = 5000;
const int MAX_DICTIONARY_SIZE = 60000;

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARATION
//--------------------------------------------------------------------------------------------------------------------------------

void run_search();

static double               run_search       (const hash_table *const store, hash_key *lexis_array);
static __always_inline void hash_table_search(const hash_table *const store, hash_key *lexis_array);
static hash_table          *hash_table_init  (                               hash_key *lexis_array);

static hash_key hash_table_rebuild(hash_table *const store, const size_t total_key_size);
static void     hash_list_rebuild (hash_table *const store, const size_t chain, char **const next_addr_key);

static hash_key *lexis_array_init(buffer *const dictionary);

//================================================================================================================================
// MAIN
//================================================================================================================================

int main()
{
    run_search();
}

//================================================================================================================================

void run_search()
{
    buffer *dictionary    = buffer_new (HASH_TABLE_TEXT);
    hash_key *lexis_array = lexis_array_init(dictionary);
    hash_table *store     = hash_table_init(lexis_array);

    hash_key store_key_new = hash_table_rebuild(store, dictionary->buff_size);
    buffer_free(dictionary);

    fprintf(stderr, "search time: %lf ms\n", run_search(store, lexis_array));

    log_free  ((void *) store_key_new);
    log_free  (lexis_array);
    hash_table_free (store);
}

//--------------------------------------------------------------------------------------------------------------------------------

static double run_search(const hash_table *const store, hash_key *lexis_array)
{
    log_assert(store       != nullptr);
    log_assert(lexis_array != nullptr);

    clock_t search_start = clock();

    #ifdef CALLGRIND_MODE
    CALLGRIND_ZERO_STATS;
    #endif

    for (int i = 0; i < RUN_SEARCH_NUM; ++i)
    {
        hash_table_search(store, lexis_array);
    }

    #ifdef CALLGRIND_MODE
    CALLGRIND_DUMP_STATS;
    #endif

    clock_t search_finish = clock();

    return 1000.0 * (search_finish - search_start) / (CLOCKS_PER_SEC * (double) RUN_SEARCH_NUM);
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void hash_table_search(const hash_table *const store, hash_key *lexis_array)
{
    log_assert(store       != nullptr);
    log_assert(lexis_array != nullptr);

    for (int i = 0; lexis_array[i] != nullptr; ++i)
    {
        hash_table_find(store, lexis_array[i]);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static hash_table *hash_table_init(hash_key *lexis_array)
{
    log_assert(lexis_array != nullptr);

    hash_table *store = hash_table_new(HASH_TABLE_SIZE, HASH_TABLE_CALC, HASH_TABLE_CMP);
    log_assert(store != nullptr);

    for (int i = 0; lexis_array[i] != nullptr; ++i)
    {
        hash_table_push_forced(store, lexis_array[i]);
    }

    return store;
}

//--------------------------------------------------------------------------------------------------------------------------------

static hash_key hash_table_rebuild(hash_table *const store, const size_t total_key_size)
{
    log_assert(store != nullptr);

    char *key_store_new = (char *) log_calloc(sizeof(char), total_key_size + 1);
    log_assert(key_store_new != nullptr);

    char *next_key_addr = key_store_new;
    for (size_t chain = 0; chain < HASH_TABLE_SIZE; ++chain)
    {
        hash_list_rebuild(store, chain, &next_key_addr);
    }

    return key_store_new;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void hash_list_rebuild(hash_table *const store, const size_t chain, char **const next_addr_key)
{
    log_assert(store != nullptr);
    log_assert(chain < HASH_TABLE_SIZE);

    log_assert( next_addr_key != nullptr);
    log_assert(*next_addr_key != nullptr);

    list_node *dup_fict = store->data[chain].fictional;
    list_node *dup_cur  = dup_fict + dup_fict->next;

    while (dup_cur != dup_fict)
    {
        size_t key_len = strlen((const char *) dup_cur->data) + 1; //null-character

        memcpy(*next_addr_key, dup_cur->data, key_len);

        dup_cur->data = next_addr_key;
        dup_cur = dup_fict + dup_cur->next;

        *next_addr_key += key_len;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static hash_key *lexis_array_init(buffer *const dictionary)
{
    log_assert(dictionary != nullptr);

    hash_key *lexis_array = (hash_key *) log_calloc(MAX_DICTIONARY_SIZE, sizeof(char *));
    hash_key  lexis = strtok(dictionary->buff_beg, "\n");

    int array_ind = 0;
    while (lexis != nullptr)
    {
        lexis_array[array_ind] = lexis;
        lexis = strtok(nullptr, "\n");

        array_ind ++;
    }
    lexis_array[array_ind] = nullptr;

    return lexis_array;
}
