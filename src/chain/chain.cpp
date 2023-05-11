#include "chain_static.h"

//--------------------------------------------------------------------------------------------------------------------------------
// ctor
//--------------------------------------------------------------------------------------------------------------------------------

bool chain_ctor(chain *const lst, void (*el_dump) (const void *const) /* = nullptr */)
{
$i
    log_verify(lst != nullptr, false);

    $el_dump  = el_dump;

    $capacity = DEFAULT_CHAIN_CAPACITY;
    $size     = 0;
    $el_free  = 1;

$   if (!chain_fictional_ctor(lst)) { $o return false; }

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

chain *chain_new(void (*el_dump) (const void *const) /* = nullptr */)
{
$i
$   chain *lst = (chain *) log_calloc(1, sizeof(chain));
    if (lst == nullptr)
    {
$       log_error("log_calloc(1, sizeof(chain) = %lu) returns nullptr\n", sizeof(chain));
$o      return nullptr;
    }

$   if (!chain_ctor(lst, el_dump)) { log_free(lst); $o return nullptr; }

$o  return lst;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool chain_fictional_ctor(chain *const lst)
{
$i
    log_assert(lst != nullptr);

$   $fictional = (chain_node *) aligned_alloc(64, DEFAULT_CHAIN_CAPACITY * sizeof(chain_node));
    if ($fictional == nullptr)
    {
$       log_error("aligned_alloc(64, DEFAULT_CHAIN_CAPACITY = %lu * sizeof(chain_node) = %lu) returns nullptr\n",
                                     DEFAULT_CHAIN_CAPACITY       * sizeof(chain_node));
$o      return false;
    }

    $fictional->prev = 0;
    $fictional->next = 0;

$   chain_free_cycle_ctor(lst);

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void chain_free_cycle_ctor(chain *const lst)
{
$i
    log_assert(lst        != nullptr);
    log_assert($fictional != nullptr);

    const size_t free_node_first_ind = $el_free;
    const size_t free_node_last_ind  = $capacity - 1;

$   for (size_t ind = free_node_first_ind + 1; ind < free_node_last_ind; ++ind)
    {
        chain_free_node_init(lst, ind, ind - 1, ind + 1);
    }

$   chain_free_node_init(lst, free_node_first_ind, free_node_last_ind, size_t_min(free_node_first_ind + 1, free_node_last_ind));
$   chain_free_node_init(lst, free_node_last_ind , size_t_max(free_node_last_ind - 1, free_node_first_ind), free_node_first_ind);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

static void chain_free_node_init(chain *const lst, const size_t ind_cur ,
                                                   const size_t ind_prev,
                                                   const size_t ind_next)
{
$i
    log_assert(lst        != nullptr);
    log_assert($fictional != nullptr);

    log_assert(ind_cur  < $capacity);
    log_assert(ind_prev < $capacity);
    log_assert(ind_next < $capacity);

    $fictional[ind_cur].prev = ind_prev;
    $fictional[ind_cur].next = ind_next;
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool chain_busy_node_new(chain *const lst, const char *const key, const size_t ind_prev,
                                                                         const size_t ind_next)
{
$i
    log_assert(ind_prev < $capacity);
    log_assert(ind_next < $capacity);

    log_verify(key != nullptr, false);

$   if ($size + 1 == $capacity) if (!chain_resize(lst)) { $o return false; }

    const size_t ind_cur = $el_free;

    if ($capacity - $size == 2) $el_free = $capacity;
    else
    {
        size_t free_next = $fictional[$el_free].next;
        size_t free_prev = $fictional[$el_free].prev;

        $fictional[free_next].prev = free_prev;
        $fictional[free_prev].next = free_next;

        $el_free = free_next;
    }

$   bool   ret = chain_busy_node_ctor(lst, key, ind_cur, ind_prev, ind_next);
$o  return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool chain_busy_node_ctor(chain *const lst, const char *const key, const size_t ind_cur,
                                                                          const size_t ind_prev,
                                                                          const size_t ind_next)
{
$i
    log_assert(lst        != nullptr);
    log_assert($fictional != nullptr);
    log_assert(key        != nullptr);

    log_assert(ind_cur != 0);
    log_assert(ind_cur  < $capacity);
    log_assert(ind_prev < $capacity);
    log_assert(ind_next < $capacity);

    chain_node *lst_node = $fictional + ind_cur;

$   memset($keys, 0, 64);
$   chain_try_push_key(lst, key, ind_cur);

    $next = ind_next; $fictional[ind_next].prev = ind_cur;
    $prev = ind_prev; $fictional[ind_prev].next = ind_cur;

    $size++;

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool chain_try_push_key(chain *const lst, const char *const key, const size_t ind_cur)
{
$i
    log_assert(lst != nullptr);
    log_assert(key != nullptr);

    log_assert(ind_cur < $capacity);

    chain_node *lst_node = $fictional + ind_cur;
$   size_t len = size_t_min(strlen(key), 31);

$   if ($keys[0]  == '\0') { memcpy($keys     , key, len); $o return true; }
$   if ($keys[32] == '\0') { memcpy($keys + 32, key, len); $o return true; }

$o  return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool chain_resize(chain *const lst)
{
$i
    log_assert($size + 1 == $capacity);

    size_t      capacity_new = 2 * $capacity;
$   chain_node *fictional_new = (chain_node *) aligned_alloc(64, capacity_new * sizeof(chain_node));
    if (fictional_new == nullptr)
    {
$       log_error("aligned_alloc(64, capacity_new * sizeof(chain_node)) = %lu) returns nullptr\n",
                                     capacity_new * sizeof(chain_node));
$o      return false;
    }

    memcpy(fictional_new, $fictional, sizeof(chain_node) * $capacity);
    free ($fictional);

    $fictional = fictional_new;
    $capacity *=         2;
    $el_free   = $size + 1;

$   chain_free_cycle_ctor(lst);

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------------------------------------------------------------------

void chain_dtor(void *const _lst)
{
$i
    if (_lst == nullptr) { $o return; }

    chain *const lst = (chain *) _lst;

$   log_free($fictional);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

void chain_delete(void *const _lst)
{
$i
$   chain_dtor(_lst);
$   log_free  (_lst);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------
// query
//--------------------------------------------------------------------------------------------------------------------------------

bool chain_push_front(chain *const lst, const char *key)
{
$i
    log_verify(lst != nullptr, false);
    log_verify(key != nullptr, false);

$   if ($size != 0 && chain_try_push_key(lst, key, $fictional->next)) { $o return true; }

    size_t ind_prev = 0;
    size_t ind_next = $fictional->next;

$   chain_busy_node_new(lst, key, ind_prev, ind_next);

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

size_t chain_get_size(const chain *const lst)
{
$i
    log_verify(lst  != nullptr, 0);

    if ($size == 0) { $o return 0; }

    if ($fictional[$fictional->next].keys[32] == '\0') { $o return 2 * $size - 1; }
$o  return 2 * $size;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool chain_find(const chain *const lst, const char *const key, int (*key_cmp)(const char *, const char *))
{
    log_verify(lst     != nullptr, false);
    log_verify(key     != nullptr, false);
    log_verify(key_cmp != nullptr, false);

    chain_node *dup_fict = $fictional;
    chain_node *dup_node = dup_fict + dup_fict->next;

    while (dup_node != dup_fict)
    {
        if (strcmp_asm((dup_node->keys), key) == 0)
        {
            #ifdef FIND_DEBUG
            log_message("[00; 32): %s", dup_node->keys);
            #endif
            return true;
        }
        if (strcmp_asm((dup_node->keys + 32), key) == 0)
        {
            #ifdef FIND_DEBUG
            log_message("[32; 64): %s", dup_node->keys + 32);
            #endif
            return true;
        }

        dup_node = dup_fict + dup_node->next;
    }

    return false;
}

static __always_inline int strcmp_asm(const char *fst, const char *sec)
{
    int result = 0;

    asm(
    ".intel_syntax noprefix\n"

    "vmovdqa ymm0, ymmword ptr [%1]         /* ymm0 <- fst       */\n"
    "vmovdqa ymm1, ymmword ptr [%2]         /* ymm1 <- sec       */\n"

    "vptest  ymm0, ymm1                     /* cf = (fst == sec) */\n"
    "mov %0, 0\n"
    "setnc %b0\n"

    ".att_syntax prefix\n"

    : "=r"(result)
    : "r"(fst), "r"(sec)
    : "ymm0", "ymm1", "cc"
    );

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------
// dump
//--------------------------------------------------------------------------------------------------------------------------------

void chain_dump(const void *const _lst)
{
$i
    const chain *const lst = (const chain *) _lst;
$   chain_static_dump(lst);
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

static void chain_static_dump(const chain *const lst)
{
$i
$   if (!chain_header_dump(lst)) { $o return; }

$   bool are_invalid_public_fields = chain_public_fields_dump(lst);
$   bool are_invalid_static_fields = chain_static_fields_dump(lst);

    bool are_invalid_fields = are_invalid_public_fields | are_invalid_static_fields;

$   chain_fictional_dump(lst, are_invalid_fields);

    LOG_TAB--;
$   log_tab_message("}\n");
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

static __always_inline bool chain_header_dump(const chain *const lst)
{
$i
$   log_tab_service_message("chain (address: %p)\n"
                            "{", "\n",        lst);

$   if (lst == nullptr) { log_tab_service_message("}", "\n"); $o return false; }
    LOG_TAB++;

$o  return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool chain_public_fields_dump(const chain *const lst)
{
$i
    log_assert(lst != nullptr);

    bool is_any_invalid = false;

    if ($el_dump == nullptr) { $ warning_field_dump("el_dump", "%p",   nullptr); }
    else                     { $ usual_field_dump  ("el_dump", "%p",  $el_dump); }

    if ($size > $capacity)   { $ error_field_dump  ("size   ", "%lu", $size); is_any_invalid = true; }
    else                     { $ usual_field_dump  ("size   ", "%lu", $size);    }

$   log_message("\n");

$o  return is_any_invalid;
}

//--------------------------------------------------------------------------------------------------------------------------------

static bool chain_static_fields_dump(const chain *const lst)
{
$i
    log_assert(lst != nullptr);

    bool is_any_invalid = false;

    if ($fictional == nullptr)  { $ error_field_dump ("fictional", "%p",    nullptr); }
    else                        { $ usual_field_dump ("fictional", "%p", $fictional); }

    if ($el_free > $capacity)   { $ error_field_dump ("el_free  ", "%lu", $el_free); is_any_invalid = true; }
    else                        { $ usual_field_dump ("el_free  ", "%lu", $el_free);  }

    if ($capacity < $size)      { $ error_field_dump ("capacity ", "%lu", $capacity); }
    else                        { $ usual_field_dump ("capacity ", "%lu", $capacity); }

$   log_message("\n");

$o  return is_any_invalid;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void chain_fictional_dump(const chain *const lst, const bool is_any_invalid)
{
$i
    log_assert(lst != nullptr);

$   log_tab_service_message("fictional\n"
                            "{", "\n");
    LOG_TAB++;

    if ($fictional == nullptr || is_any_invalid)
    {
$       log_tab_error_message("can't dump it because some of fields are invalid", "\n");
    }
    else { $ chain_fictional_static_dump(lst); }

    LOG_TAB--;
$   log_tab_service_message("}", "\n");
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

static void chain_fictional_static_dump(const chain *const lst)
{
$i
    log_assert(lst        != nullptr);
    log_assert($fictional != nullptr);

    for (size_t ind = 0; ind < $capacity; ++ind)
    {
$       log_tab_service_message("#%lu\n{", "\n", ind); LOG_TAB++;
$       chain_node_static_dump(lst, $fictional + ind);
$       LOG_TAB--; log_tab_service_message("}", "\n");
    }
$o
}

//--------------------------------------------------------------------------------------------------------------------------------

static void chain_node_static_dump(const chain *const lst, const chain_node *const lst_node)
{
$i
    log_assert(lst_node != nullptr);

    if ($prev >= $capacity) { $ error_field_dump  ("prev", "%lu", $prev);  }
    else                    { $ usual_field_dump  ("prev", "%lu", $prev);  }

    if ($next >= $capacity) { $ error_field_dump  ("next", "%lu", $next);  }
    else                    { $ usual_field_dump  ("next", "%lu", $next);  }

    log_tab_message("keys = \"");
    for (int i = 0; i < 64; ++i)
    {
        if ($keys[i] == '\0') log_message("|");
        else                  log_message("%c", $keys[i]);
    }
    log_message("\"\n");
$o
}
