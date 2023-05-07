#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#define NVERIFY
#define LOG_NTRACE
#define LOG_NLEAK

#include "../../lib/logs/log.h"
#include "hash.h"

static __always_inline hash_val ror(hash_val val);
static __always_inline hash_val rol(hash_val val);

//--------------------------------------------------------------------------------------------------------------------------------
// hash
//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

hash_val hash_trivial(hash_key elem)
{
    log_verify(elem != nullptr, 0);

    return 1;
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------

hash_val hash_first_char(hash_key elem)
{
    log_verify(elem != nullptr, 0);

    return (hash_val) elem[0];
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_val hash_len(hash_key elem)
{
    log_verify(elem != nullptr, 0);

    return strlen(elem);
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_val hash_sum_char(hash_key elem)
{
    log_verify(elem != nullptr, 0);

    hash_val result = (hash_val) elem[0];
    size_t len      = strlen(elem);

    for (size_t i = 1; i < len; ++i) { result += (hash_val) elem[i]; }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_val hash_ror(hash_key elem)
{
    log_verify(elem != nullptr, 0);

    hash_val result = (hash_val) elem[0];
    size_t len      = strlen(elem);

    for (size_t i = 1; i < len; ++i) { result = ror(result) ^ (hash_val) elem[i]; }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_val hash_rol(hash_key elem)
{
    log_verify(elem != nullptr, 0);

    hash_val result = (hash_val) elem[0];
    size_t len = strlen(elem);

    for (size_t i = 1; i < len; ++i) { result = rol(result) ^ (hash_val) elem[i]; }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------

hash_val hash_crc32(hash_key elem)
{
    const unsigned polynom = 0x04C11DB7;
    hash_val        result = -1U;

    for (size_t cnt = 0; elem[cnt] != '\0'; ++cnt)
    {
        result = result ^ (hash_val) elem[cnt];

        for (size_t bit = 7; bit != 0; --bit)
        {
            unsigned mask = -(result & 1);
            result = (result >> 1) ^ (polynom & mask);
        }
    }

    return ~result;
}

//--------------------------------------------------------------------------------------------------------------------------------
// ROR, ROL
//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline hash_val ror(hash_val val)
{
    const unsigned shift = (sizeof(hash_val) << 3) - 1;

    return (val >> 1) | ((val & 1UL) << shift);
}

static __always_inline hash_val rol(hash_val val)
{
    const unsigned shift = (sizeof(hash_val) << 3) - 1;

    return (val << 1) | ((val & (1UL << shift)) >> shift);
}
