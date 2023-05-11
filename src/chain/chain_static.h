/** @file */
#ifndef CACHE_chain_STATIC_H
#define CACHE_chain_STATIC_H
//#define FIND_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "chain_settings.h"
#include "chain.h"

//================================================================================================================================
// DSL
//================================================================================================================================

#define $keys       (lst_node->keys)
#define $prev       (lst_node->prev)
#define $next       (lst_node->next)

#define $fictional  (lst->fictional)
#define $el_free    (lst->el_free)
#define $size       (lst->size)
#define $capacity   (lst->capacity)
#define $el_dump    (lst->el_dump)

//================================================================================================================================
// FUNCTION DECLARATION
//================================================================================================================================

static bool chain_fictional_ctor (chain *const lst);
static void chain_free_cycle_ctor(chain *const lst);

static void chain_free_node_init(chain *const lst,                        const size_t ind_cur, const size_t ind_prev, const size_t ind_next);
static bool chain_busy_node_new (chain *const lst, const char *const key,                       const size_t ind_prev, const size_t ind_next);
static bool chain_busy_node_ctor(chain *const lst, const char *const key, const size_t ind_cur, const size_t ind_prev, const size_t ind_next);
static bool chain_try_push_key  (chain *const lst, const char *const key, const size_t ind_cur);

static bool chain_resize(chain *const lst);

static void                 chain_static_dump           (const chain *const lst);
static __always_inline bool chain_header_dump           (const chain *const lst);
static bool                 chain_public_fields_dump    (const chain *const lst);
static bool                 chain_static_fields_dump    (const chain *const lst);
static void                 chain_fictional_dump        (const chain *const lst, const bool is_any_invalid);
static void                 chain_fictional_static_dump (const chain *const lst);
static void                 chain_node_static_dump      (const chain *const lst, const chain_node *const lst_node);

#endif //CACHE_chain_STATIC_H
