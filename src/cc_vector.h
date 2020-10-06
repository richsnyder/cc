/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#ifndef CC_VECTOR_H
#define CC_VECTOR_H

#include "cc_memory.h"

#if defined (__cplusplus)
extern "C" {
#endif

struct cc_vector
{
  size_t size;
  size_t capacity;
  size_t element_size;
  struct cc_functions functions;
  void* data;
};

struct cc_vector_iterator
{
  size_t element_size;
  void* data;
};

typedef struct cc_vector* cc_vector_t;

typedef struct cc_vector_iterator cc_vector_iterator_t;

extern const size_t cc_vector_sizeof;

extern const struct cc_functions cc_vector_functions;

uint64_t
cc_vector_hasher(const void* buffer, size_t size);

void*
cc_vector_copier(void* dest, const void* src, size_t size);

void
cc_vector_deleter(void* ptr);

bool
cc_vector_equality(const void* left, const void* right, size_t size);

struct cc_vector*
cc_vector_new(size_t element_size);

struct cc_vector*
cc_vector_from_array(const void* data, size_t count, size_t element_size);

struct cc_vector*
cc_vector_new_f(size_t element_size, const struct cc_functions functions);

struct cc_vector*
cc_vector_from_array_f(const void* data,
                       size_t count,
                       size_t element_size,
                       const struct cc_functions functions);

struct cc_vector*
cc_vector_copy(const struct cc_vector* other);

void
cc_vector_delete(struct cc_vector* self);

void
cc_vector_assign(struct cc_vector* self, size_t count, const void* value);

void
cc_vector_set(struct cc_vector* self, size_t pos, const void* value);

const void*
cc_vector_get(const struct cc_vector* self, size_t pos);

const void*
cc_vector_front(const struct cc_vector* self);

const void*
cc_vector_back(const struct cc_vector* self);

const void*
cc_vector_data(const struct cc_vector* self);

struct cc_vector_iterator
cc_vector_begin(struct cc_vector* self);

struct cc_vector_iterator
cc_vector_end(struct cc_vector* self);

bool
cc_vector_empty(const struct cc_vector* self);

size_t
cc_vector_size(const struct cc_vector* self);

void
cc_vector_reserve(struct cc_vector* self, size_t new_cap);

size_t
cc_vector_capacity(const struct cc_vector* self);

void
cc_vector_shrink_to_fit(struct cc_vector* self);

void
cc_vector_clear(struct cc_vector* self);

void
cc_vector_insert(struct cc_vector* self, size_t pos, const void* value);

void
cc_vector_erase(struct cc_vector* self, size_t first, size_t last);

void
cc_vector_push_back(struct cc_vector* self, const void* value);

void
cc_vector_pop_back(struct cc_vector* self);

void
cc_vector_resize(struct cc_vector* self, size_t count, const void* value);

void
cc_vector_swap(struct cc_vector* self, struct cc_vector* other);

bool
cc_vector_eq(const struct cc_vector* self, const struct cc_vector* other);

bool
cc_vector_ne(const struct cc_vector* self, const struct cc_vector* other);

void
cc_vector_iterator_increment(struct cc_vector_iterator* self);

void
cc_vector_iterator_decrement(struct cc_vector_iterator* self);

void
cc_vector_iterator_advance(struct cc_vector_iterator* self, ptrdiff_t n);

ptrdiff_t
cc_vector_iterator_distance_to(const struct cc_vector_iterator self,
                               const struct cc_vector_iterator other);

void*
cc_vector_iterator_dereference(const struct cc_vector_iterator self);

bool
cc_vector_iterator_eq(const struct cc_vector_iterator self,
                      const struct cc_vector_iterator other);

bool
cc_vector_iterator_ne(const struct cc_vector_iterator self,
                      const struct cc_vector_iterator other);

#if defined(__cplusplus)
}
#endif

#endif // CC_VECTOR_H
