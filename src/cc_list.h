/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#ifndef CC_LIST_H
#define CC_LIST_H

#include "cc_memory.h"

#if defined (__cplusplus)
extern "C" {
#endif

struct cc_list_node
{
  struct cc_list_node* next;
  struct cc_list_node* prev;
  void* data;
};

struct cc_list
{
  size_t size;
  size_t element_size;
  struct cc_functions functions;
  struct cc_list_node* front;
  struct cc_list_node* back;
};

struct cc_list_iterator
{
  const struct cc_list* list;
  struct cc_list_node* node;
};

typedef struct cc_list_node* cc_list_node_t;

typedef struct cc_list* cc_list_t;

typedef struct cc_list_iterator cc_list_iterator_t;

extern const size_t cc_list_sizeof;

extern const struct cc_functions cc_list_functions;

uint64_t
cc_list_hasher(const void* buffer, size_t size);

void*
cc_list_copier(void* dest, const void* src, size_t size);

void
cc_list_deleter(void* ptr);

bool
cc_list_equality(const void* left, const void* right, size_t size);

struct cc_list*
cc_list_new(size_t element_size);

struct cc_list*
cc_list_from_array(const void* data, size_t count, size_t element_size);

struct cc_list*
cc_list_new_f(size_t element_size, const struct cc_functions functions);

struct cc_list*
cc_list_from_array_f(const void* data,
                     size_t count,
                     size_t element_size,
                     const struct cc_functions functions);

struct cc_list*
cc_list_copy(const struct cc_list* other);

void
cc_list_delete(struct cc_list* self);

void
cc_list_assign(struct cc_list* self, size_t count, const void* value);

const void*
cc_list_front(const struct cc_list* self);

const void*
cc_list_back(const struct cc_list* self);

struct cc_list_iterator
cc_list_begin(const struct cc_list* self);

struct cc_list_iterator
cc_list_end(const struct cc_list* self);

bool
cc_list_empty(const struct cc_list* self);

size_t
cc_list_size(const struct cc_list* self);

void
cc_list_clear(struct cc_list* self);

void
cc_list_insert(struct cc_list* self,
               const struct cc_list_iterator pos,
               const void* value);

void
cc_list_erase(struct cc_list* self,
              const struct cc_list_iterator first,
              const struct cc_list_iterator last);

void
cc_list_push_back(struct cc_list* self, const void* value);

void
cc_list_pop_back(struct cc_list* self);

void
cc_list_push_front(struct cc_list* self, const void* value);

void
cc_list_pop_front(struct cc_list* self);

void
cc_list_resize(struct cc_list* self, size_t count, const void* value);

void
cc_list_swap(struct cc_list* self, struct cc_list* other);

void
cc_list_merge(struct cc_list* self,
              struct cc_list* other,
              bool (*comp)(void*, void*));

void
cc_list_splice(struct cc_list* self,
               const struct cc_list_iterator pos,
               struct cc_list* other);

void
cc_list_remove(struct cc_list* self, const void* value);

void
cc_list_remove_if(struct cc_list* self, bool (*pred)(void*));

void
cc_list_reverse(struct cc_list* self);

void
cc_list_unique(struct cc_list* self);

void
cc_list_sort(struct cc_list* self, bool (*comp)(void*, void*));

bool
cc_list_eq(const struct cc_list* self, const struct cc_list* other);

bool
cc_list_ne(const struct cc_list* self, const struct cc_list* other);

void
cc_list_iterator_increment(struct cc_list_iterator* self);

void
cc_list_iterator_decrement(struct cc_list_iterator* self);

void*
cc_list_iterator_dereference(const struct cc_list_iterator self);

bool
cc_list_iterator_eq(const struct cc_list_iterator self,
                    const struct cc_list_iterator other);

bool
cc_list_iterator_ne(const struct cc_list_iterator self,
                    const struct cc_list_iterator other);

#if defined(__cplusplus)
}
#endif

#endif // CC_LIST_H
