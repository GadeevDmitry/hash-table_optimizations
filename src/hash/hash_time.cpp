#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

//#define NDEBUG
//#define NVERIFY
#define LOG_NTRACE

#include "../../lib/algorithm/algorithm.h"
#include "../../lib/logs/log.h"

#include "type.h"
#include "hash.h"

//--------------------------------------------------------------------------------------------------------------------------------
// SETTINGS
//--------------------------------------------------------------------------------------------------------------------------------

hash_val   (*HASH_FUNC) (hash_key elem) = hash_ror;
const char  *HASH_TABLE_TEXT = "data/dictionary.txt";

const int RUN_CALC_NUM        = 30000;
const int MAX_DICTIONARY_SIZE = 60000;

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARATION
//--------------------------------------------------------------------------------------------------------------------------------

void run_calc();

static double               run_calc (hash_key *lexis_array);
static __always_inline void hash_calc(hash_key *lexis_array);

static hash_key *lexis_array_init (buffer *const dictionary);

//================================================================================================================================
// MAIN
//================================================================================================================================

int main()
{
    run_calc();
}

//================================================================================================================================

void run_calc()
{
    buffer *dictionary    = buffer_new (HASH_TABLE_TEXT);
    hash_key *lexis_array = lexis_array_init(dictionary);

    fprintf(stderr, "calc time: %lf ms\n", 10.0 * run_calc(lexis_array) / (double) RUN_CALC_NUM);

    buffer_free(dictionary);
    log_free  (lexis_array);
}

//--------------------------------------------------------------------------------------------------------------------------------

static double run_calc(hash_key *lexis_array)
{
    log_verify(lexis_array != nullptr, NAN);

    time_t calc_start = clock();
    for (int i = 0; i < RUN_CALC_NUM; ++i)
    {
        hash_calc(lexis_array);
    }
    time_t calc_finish = clock();

    return 1000.0 * (calc_finish - calc_start) / CLOCKS_PER_SEC;
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline void hash_calc(hash_key *lexis_array)
{
    log_verify(lexis_array != nullptr, (void) 0);

    for (int i = 0; lexis_array[i] != nullptr; ++i)
    {
        HASH_FUNC(lexis_array[i]);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

static hash_key *lexis_array_init(buffer *const dictionary)
{
    log_verify(dictionary != nullptr, nullptr);

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
