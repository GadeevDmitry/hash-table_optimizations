#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NDEBUG
#define NVERIFY

#include "../lib/algorithm/algorithm.h"
#include "../lib/logs/log.h"

#include "hash/type.h"
#include "hash/hash.h"
#include "hash/hash_table.h"

#include <valgrind/callgrind.h>

//--------------------------------------------------------------------------------------------------------------------------------
// SETTINGS
//--------------------------------------------------------------------------------------------------------------------------------

const size_t HASH_TABLE_SIZE = 1907;
const char  *HASH_TABLE_TEXT = "data/dictionary.txt";

int        (*HASH_TABLE_CMP ) (hash_key fst, hash_key sec) = strcmp;
hash_val   (*HASH_TABLE_CALC) (hash_key elem)              = hash_crc32;

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARATION
//--------------------------------------------------------------------------------------------------------------------------------

int         hash_table_search(const hash_table *const store, const buffer *const query);
hash_table *hash_table_init  (                               const buffer *const dictionary);
buffer     *dictionary_parse ();

//================================================================================================================================
// MAIN
//================================================================================================================================

int main()
{
    buffer *dictionary = dictionary_parse();
    buffer *query      = dictionary_parse();

    hash_table *store = hash_table_init(dictionary);

    time_t search_start = clock();
    CALLGRIND_ZERO_STATS;
    int lexicon = hash_table_search(store, query);
    CALLGRIND_DUMP_STATS;
    time_t search_finish = clock();

    buffer_free(dictionary);
    buffer_free(query);

    fprintf(stderr, "lexicon:        %d words\n"
                    "searching time: %lg ms  \n", lexicon, (search_finish - search_start) * 1000.0 / CLOCKS_PER_SEC);
}

//================================================================================================================================

int hash_table_search(const hash_table *const store, const buffer *const query)
{
    log_verify(store != nullptr, -1);
    log_verify(query != nullptr, -1);

    int result = 0;
    hash_key lexis = strtok(query->buff_beg, "\n");
    while (lexis != nullptr)
    {
        if (hash_table_find(store, lexis)) result++;
        lexis = strtok(nullptr, "\n");
    }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_table *hash_table_init(const buffer *const dictionary)
{

    log_verify(dictionary != nullptr, nullptr);

    hash_table *store = hash_table_new(HASH_TABLE_SIZE, HASH_TABLE_CALC, HASH_TABLE_CMP);
    log_verify(store != nullptr, nullptr);

    hash_key lexis = strtok(dictionary->buff_beg, "\n");
    while (lexis != nullptr)
    {
        hash_table_push_forced(store, lexis);
        lexis = strtok(nullptr, "\n");
    }

    return store;
}

//--------------------------------------------------------------------------------------------------------------------------------

buffer *dictionary_parse()
{
    buffer *dictionary = buffer_new(HASH_TABLE_TEXT);
    log_verify(dictionary != nullptr, nullptr);

    return dictionary;
}