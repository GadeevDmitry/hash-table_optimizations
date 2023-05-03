/** @file */
#ifndef chain_H
#define chain_H

#include "../../lib/logs/log.h"
#include "../../lib/algorithm/algorithm.h"

//================================================================================================================================
// STRUCT
//================================================================================================================================

struct chain_node
{
    char keys[64];

    size_t prev;
    size_t next;
};

struct chain
{
    chain_node *fictional;

    size_t el_free;
    size_t    size;
    size_t capacity;

    void (*el_dump) (const void *const);
};

//================================================================================================================================
// FUNCTION DECLARATION
//================================================================================================================================

bool   chain_ctor(chain *const lst, void (*el_dump) (const void *const) = nullptr);
chain *chain_new (                  void (*el_dump) (const void *const) = nullptr);

void chain_dtor(void *const _lst);
void chain_free(void *const _lst);

bool   chain_push_front(      chain *const lst, const char *const key);
size_t chain_get_size  (const chain *const lst);

void chain_dump(const void  *const _lst);

#endif // chain_H
