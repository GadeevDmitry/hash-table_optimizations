#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#define NVERIFY
#define LOG_NTRACE

#include "../lib/algorithm/algorithm.h"
#include "../lib/logs/log.h"

#include "hash/type.h"
#include "hash/hash.h"
#include "hash/hash_table.h"

//--------------------------------------------------------------------------------------------------------------------------------
// SETTINGS
//--------------------------------------------------------------------------------------------------------------------------------

const size_t HASH_TABLE_SIZE = 1907;
const char  *HASH_TABLE_TEXT = "data/dictionary.txt";
const char  *HASH_TABLE_CSV  = "data/division.csv";

#define CSV_SEP ";"

//--------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARATION
//--------------------------------------------------------------------------------------------------------------------------------

void csv_hash (FILE *const csv_stream, hash_val (*h_culc)(hash_key elem), const char *const h_name);
void csv_index(FILE *const csv_stream);

static void        division_convert(const buffer *const dictionary, hash_val (*h_culc)(hash_key elem), FILE *const csv_stream);
static hash_table *dictionary_parse(const buffer *const dictionary, hash_val (*h_culc)(hash_key elem));

//================================================================================================================================
// MAIN
//================================================================================================================================

int main()
{
    FILE   *csv_stream = fopen(HASH_TABLE_CSV, "w");

    csv_hash(csv_stream, hash_trivial   , "trivial");
    csv_hash(csv_stream, hash_first_char, "first char");
    csv_hash(csv_stream, hash_len       , "length");
    csv_hash(csv_stream, hash_sum_char  , "sum char");
    csv_hash(csv_stream, hash_rol       , "ROL");
    csv_hash(csv_stream, hash_ror       , "ROR");
    csv_hash(csv_stream, hash_crc32     , "CRC32");

    csv_index(csv_stream);
    fclose   (csv_stream);
}

//================================================================================================================================

void csv_index(FILE *const csv_stream)
{
    fprintf(csv_stream, "%-20s" CSV_SEP " ", "index");

    for (size_t index = 0; index < HASH_TABLE_SIZE; ++index)
    {
        fprintf(csv_stream, "%6lu " CSV_SEP " ", index);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

void csv_hash(FILE *const csv_stream, hash_val (*h_culc)(hash_key elem), const char *const h_name)
{
    log_verify(csv_stream != nullptr, (void) 0);
    log_verify(h_culc     != nullptr, (void) 0);
    log_verify(h_name     != nullptr, (void) 0);

    buffer *dictionary = buffer_new(HASH_TABLE_TEXT);

    fprintf(csv_stream, "%-20s" CSV_SEP " ", h_name); division_convert(dictionary, h_culc, csv_stream);

    buffer_free(dictionary);
}

//--------------------------------------------------------------------------------------------------------------------------------

static void division_convert(const buffer *const dictionary, hash_val (*h_culc)(hash_key elem), FILE *const csv_stream)
{
    log_verify(dictionary != nullptr, (void) 0);
    log_verify(h_culc     != nullptr, (void) 0);
    log_verify(csv_stream != nullptr, (void) 0);

    hash_table *store = dictionary_parse(dictionary, h_culc);
    log_verify(store != nullptr, (void) 0);

    size_t h_size = store->size;
    list  *h_data = store->data;

    for (size_t index = 0; index < h_size; ++index)
    {
        fprintf(csv_stream, "%6lu " CSV_SEP " ", h_data[index].size);
    }
    putc('\n', csv_stream);

    hash_table_free(store);
}

//--------------------------------------------------------------------------------------------------------------------------------

static hash_table *dictionary_parse(const buffer *const dictionary, hash_val (*h_culc)(hash_key elem))
{
    log_assert(dictionary != nullptr);
    log_assert(h_culc     != nullptr);

    hash_table *store = hash_table_new(HASH_TABLE_SIZE, h_culc, strcmp);
    log_verify(store != nullptr, nullptr);

    hash_key lexis = strtok(dictionary->buff_beg, "\n");
    while (lexis != nullptr)
    {
        hash_table_push_forced(store, lexis); // работает корректно, если буфер состоит из уникальных слов, иначе статистика заполнения будет неверная,
        lexis = strtok(nullptr, "\n");        // так как функция hash_table_push_forced() не проверяет на наличие копий в списке!
    }

    return store;
}
