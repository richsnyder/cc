/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#ifndef CC_MAP_H
#define CC_MAP_H

#include "cc_memory.h"

#if defined (__cplusplus)
extern "C" {
#endif

struct cc_map_node
{
  void* key;
  void* value;
  uint64_t hash;
  uint16_t length;
};

struct cc_map
{
  size_t size;
  size_t capacity;
  size_t key_size;
  size_t value_size;
  uint16_t max_length;
  double max_load_factor;
  struct cc_functions key_functions;
  struct cc_functions value_functions;
  struct cc_map_node* nodes;
};

struct cc_map_iterator
{
  struct cc_map* map;
  struct cc_map_node* node;
};

struct cc_map_key_value
{
  void* key;
  void* value;
};

typedef struct cc_map* cc_map_t;

typedef struct cc_map_iterator cc_map_iterator_t;

typedef struct cc_map_key_value cc_map_key_value_t;

extern const size_t cc_vector_sizeof;

extern const struct cc_functions cc_map_functions;

uint64_t
cc_map_hasher(const void* buffer, size_t size);

void*
cc_map_copier(void* dest, const void* src, size_t size);

void
cc_map_deleter(void* ptr);

bool
cc_map_equality(const void* left, const void* right, size_t size);

struct cc_map*
cc_map_new(size_t key_size, size_t value_size);

struct cc_map*
cc_map_from_arrays(const void* keys,
                   const void* values,
                   size_t count,
                   size_t key_size,
                   size_t value_size);

struct cc_map*
cc_map_new_f(size_t key_size,
             size_t value_size,
             const struct cc_functions key_functions,
             const struct cc_functions value_functions);

struct cc_map*
cc_map_from_arrays_f(const void* keys,
                     const void* values,
                     size_t count,
                     size_t key_size,
                     size_t value_size,
                     const struct cc_functions key_functions,
                     const struct cc_functions value_functions);

struct cc_map*
cc_map_copy(const struct cc_map* other);

void
cc_map_delete(struct cc_map* self);

struct cc_map_iterator
cc_map_begin(struct cc_map* self);

struct cc_map_iterator
cc_map_end(struct cc_map* self);

bool
cc_map_empty(const struct cc_map* self);

size_t
cc_map_size(const struct cc_map* self);

size_t
cc_map_capacity(const struct cc_map* self);

void
cc_map_clear(struct cc_map* self);

void
cc_map_insert(struct cc_map* self, const void* key, const void* value);

void
cc_map_erase(struct cc_map* self, const void* key);

void
cc_map_swap(struct cc_map* self, struct cc_map* other);

void
cc_map_merge(struct cc_map* self, struct cc_map* other);

void*
cc_map_find(const struct cc_map* self, const void* key);

bool
cc_map_contains(const struct cc_map* self, const void* key);

double
cc_map_load_factor(const struct cc_map* self);

double
cc_map_max_load_factor(const struct cc_map* self);

void
cc_map_set_max_load_factor(struct cc_map* self, double factor);

void
cc_map_reserve(struct cc_map* self, size_t count);

bool
cc_map_eq(const struct cc_map* self, const struct cc_map* other);

bool
cc_map_ne(const struct cc_map* self, const struct cc_map* other);

void
cc_map_iterator_increment(struct cc_map_iterator* self);

void
cc_map_iterator_decrement(struct cc_map_iterator* self);

struct cc_map_key_value
cc_map_iterator_dereference(const struct cc_map_iterator self);

bool
cc_map_iterator_eq(const struct cc_map_iterator self,
                   const struct cc_map_iterator other);

bool
cc_map_iterator_ne(const struct cc_map_iterator self,
                   const struct cc_map_iterator other);

#if defined(__cplusplus)
}
#endif

#endif // CC_MAP_H
