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

const int RUN_SEARCH_NUM      = 15000;
const int MAX_DICTIONARY_SIZE = 60000;

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARATION
//--------------------------------------------------------------------------------------------------------------------------------

extern "C" hash_val crc32_asm(hash_key elem);
void run_search();

static double               search_test      (const hash_table *const store, hash_key *lexis_array);
static __always_inline void hash_table_search(const hash_table *const store, hash_key *lexis_array);
static hash_table          *hash_table_init   (                              hash_key *lexis_array);

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

    fprintf(stderr, "search time: %lf ms\n", search_test(store, lexis_array));

    buffer_free(dictionary);
    hash_table_delete(store);

        free((void *) *lexis_array);
    log_free(          lexis_array);
}

//--------------------------------------------------------------------------------------------------------------------------------

static double search_test(const hash_table *const store, hash_key *lexis_array)
{
    log_verify(store       != nullptr, NAN);
    log_verify(lexis_array != nullptr, NAN);

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
    log_verify(store       != nullptr, (void) 0);
    log_verify(lexis_array != nullptr, (void) 0);

    for (int i = 0; lexis_array[i] != nullptr; ++i)
    {
        hash_table_find(store, lexis_array[i]);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static hash_table *hash_table_init(hash_key *lexis_array)
{
    log_verify(lexis_array != nullptr, nullptr);

    hash_table *store = hash_table_new(HASH_TABLE_SIZE, crc32_asm, strcmp /* strcmp_asm */);
    log_verify(store != nullptr, nullptr);

    for (int i = 0; lexis_array[i] != nullptr; ++i)
    {
        hash_table_push_forced(store, lexis_array[i]);  // работает корректно, если буфер состоит из уникальных слов, иначе статистика заполнения будет неверная,
                                                        // так как функция hash_table_push_forced() не проверяет на наличие копий в списке!
    }

    return store;
}

//--------------------------------------------------------------------------------------------------------------------------------

static hash_key *lexis_array_init(buffer *const dictionary)
{
    log_verify(dictionary != nullptr, nullptr);

    hash_key *lexis_array = (hash_key *) log_calloc   (    MAX_DICTIONARY_SIZE, sizeof(char *));
    char     *lexis_store = (char     *) aligned_alloc(32, MAX_DICTIONARY_SIZE * 32);
    hash_key  lexis = strtok(dictionary->buff_beg, "\n");

    int array_ind = 0;

    while (lexis != nullptr)
    {
        memcpy(lexis_store + (array_ind << 5), lexis, size_t_min(31, strlen(lexis)));

        lexis_array[array_ind] = lexis_store + (array_ind << 5);
        lexis = strtok(nullptr, "\n");

        array_ind++;
    }
    lexis_array[array_ind] = nullptr;

    return lexis_array;
}
