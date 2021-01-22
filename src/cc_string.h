/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#ifndef CC_STRING_H
#define CC_STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "cc_memory.h"

#if defined (__cplusplus)
extern "C" {
#endif

struct cc_string
{
  size_t size;
  size_t capacity;
  char* data;
};

struct cc_string_iterator
{
  char* data;
};

typedef struct cc_string* cc_string_t;

typedef struct cc_string_iterator cc_string_iterator_t;

extern const size_t cc_npos;

extern const size_t cc_string_sizeof;

extern const struct cc_functions cc_string_functions;

uint64_t
cc_string_hasher(const void* buffer, size_t size);

void*
cc_string_copier(void* dest, const void* src, size_t size);

void
cc_string_deleter(void* ptr);

bool
cc_string_equality(const void* left, const void* right, size_t size);

struct cc_string*
cc_string_new();

struct cc_string*
cc_string_from_chars(const char* s, size_t count);

struct cc_string*
cc_string_copy(const struct cc_string* other);

void
cc_string_delete(struct cc_string* self);

void
cc_string_assign(struct cc_string* self, size_t count, char ch);

void
cc_string_set(struct cc_string* self, size_t pos, char ch);

char
cc_string_get(const struct cc_string* self, size_t pos);

char
cc_string_front(const struct cc_string* self);

char
cc_string_back(const struct cc_string* self);

const char*
cc_string_data(const struct cc_string* self);

struct cc_string_iterator
cc_string_begin(struct cc_string* self);

struct cc_string_iterator
cc_string_end(struct cc_string* self);

bool
cc_string_empty(const struct cc_string* self);

size_t
cc_string_size(const struct cc_string* self);

void
cc_string_reserve(struct cc_string* self, size_t new_cap);

size_t
cc_string_capacity(const struct cc_string* self);

void
cc_string_shrink_to_fit(struct cc_string* self);

void
cc_string_clear(struct cc_string* self);

void
cc_string_insert(struct cc_string* self,
                  size_t pos,
                  const char* s,
                  size_t count);

void
cc_string_erase(struct cc_string* self, size_t first, size_t last);

void
cc_string_push_back(struct cc_string* self, char ch);

void
cc_string_pop_back(struct cc_string* self);

void
cc_string_append(struct cc_string* self, const char* s, size_t count);

int
cc_string_compare(const struct cc_string* self,
                   const struct cc_string* other);

bool
cc_string_starts_with(const struct cc_string* self, const char* s);

bool
cc_string_ends_with(const struct cc_string* self, const char* s);

void
cc_string_replace(struct cc_string* self,
                   size_t pos,
                   size_t count,
                   const char* s,
                   size_t count2);

struct cc_string*
cc_string_substr(const struct cc_string* self, size_t pos, size_t count);

void
cc_string_resize(struct cc_string* self, size_t count, char ch);

void
cc_string_swap(struct cc_string* self, struct cc_string* other);

size_t
cc_string_find(const struct cc_string* self,
                const char* s,
                size_t pos,
                size_t count);

size_t
cc_string_rfind(const struct cc_string* self,
                 const char* s,
                 size_t pos,
                 size_t count);

size_t
cc_string_find_first_of(const struct cc_string* self,
                         const char* s,
                         size_t pos,
                         size_t count);

size_t
cc_string_find_first_not_of(const struct cc_string* self,
                             const char* s,
                             size_t pos,
                             size_t count);

size_t
cc_string_find_last_of(const struct cc_string* self,
                        const char* s,
                        size_t pos,
                        size_t count);

size_t
cc_string_find_last_not_of(const struct cc_string* self,
                            const char* s,
                            size_t pos,
                            size_t count);

void
cc_string_iterator_increment(struct cc_string_iterator* self);

void
cc_string_iterator_decrement(struct cc_string_iterator* self);

void
cc_string_iterator_advance(struct cc_string_iterator* self, ptrdiff_t n);

ptrdiff_t
cc_string_iterator_distance_to(const struct cc_string_iterator self,
                               const struct cc_string_iterator other);

char*
cc_string_iterator_dereference(const struct cc_string_iterator self);

bool
cc_string_iterator_eq(const struct cc_string_iterator self,
                      const struct cc_string_iterator other);

bool
cc_string_iterator_ne(const struct cc_string_iterator self,
                      const struct cc_string_iterator other);

#if defined (__cplusplus)
}
#endif

#endif // CC_STRING_H
