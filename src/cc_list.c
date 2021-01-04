/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#include <stdlib.h>
#include <string.h>
#include "cc_list.h"

const size_t cc_list_sizeof = sizeof(struct cc_list);

const struct cc_functions cc_list_functions = (struct cc_functions){
  .hasher = cc_list_hasher,
  .copier = cc_list_copier,
  .deleter = cc_list_deleter,
  .equality = cc_list_equality
};

void
_cc_list_emplace_front(struct cc_list* self, struct cc_list_node* node)
{
  if (self && node)
  {
    node->prev = NULL;
    node->next = self->front;

    if (!self->back)
    {
      self->back = node;
    }
    if (self->front)
    {
      self->front->prev = node;
      self->front = node;
    }
    else
    {
      self->front = node;
    }

    ++self->size;
  }
}

struct cc_list_node*
_cc_list_split(struct cc_list_node* head)
{
  struct cc_list_node* fast = head;
  struct cc_list_node* slow = head;
  while (fast->next && fast->next->next)
  {
    fast = fast->next->next;
    slow = slow->next;
  }
  struct cc_list_node* temp = slow->next;
  slow->next = NULL;
  return temp;
}

struct cc_list_node*
_cc_list_merge(struct cc_list_node* first,
               struct cc_list_node* second,
               bool (*comp)(void*, void*))
{
  if (!first)
  {
    return second;
  }

  if (!second)
  {
    return first;
  }

  if (comp(first->data, second->data))
  {
    first->next = _cc_list_merge(first->next, second, comp);
    first->next->prev = first;
    first->prev = NULL;
    return first;
  }
  else
  {
    second->next = _cc_list_merge(first, second->next, comp);
    second->next->prev = second;
    second->prev = NULL;
    return second;
  }
}

struct cc_list_node*
_cc_list_merge_sort(struct cc_list_node* head, bool (*comp)(void*, void*))
{
  if (!head || !head->next)
  {
    return head;
  }

  struct cc_list_node* second = _cc_list_split(head);
  head = _cc_list_merge_sort(head, comp);
  second = _cc_list_merge_sort(second, comp);
  return _cc_list_merge(head, second, comp);
}

uint64_t
cc_list_hasher(const void* buffer, size_t size)
{
  const struct cc_list* self = (const struct cc_list*) buffer;
  const struct cc_list_node* node = self->front;

  uint64_t hash = 0;
  cc_hash_fn hasher = self->functions.hasher;
  size_t elem_size = self->element_size;

  while (node)
  {
    cc_hash_combine(&hash, hasher(node->data, elem_size));
    node = node->next;
  }

  return hash;
}

void*
cc_list_copier(void* dest, const void* src, size_t size)
{
  if (dest && src)
  {
    struct cc_list* self = (struct cc_list*) dest;
    const struct cc_list* other = (const struct cc_list*) src;

    self->size = 0;
    self->element_size = other->element_size;
    self->functions = other->functions;
    self->front = NULL;
    self->back = NULL;

    struct cc_list_node* node = other->front;
    while (node)
    {
      cc_list_push_back(self, node->data);
      node = node->next;
    }

    return self;
  }
  else
  {
    return NULL;
  }
}

void
cc_list_deleter(void* ptr)
{
  if (ptr)
  {
    struct cc_list* self = (struct cc_list*) ptr;
    cc_delete_fn deleter = self->functions.deleter;
    struct cc_list_node* node = self->front;
    struct cc_list_node* next;

    while (node)
    {
      next = node->next;
      deleter(node->data);
      free(node->data);
      free(node);
      node = next;
    }

    self->size = 0;
    self->element_size = 0;
    self->functions = cc_default_functions;
    self->front = NULL;
    self->back = NULL;
  }
}

bool
cc_list_equality(const void* left, const void* right, size_t size)
{
  if (left && right)
  {
    const struct cc_list* self = (const struct cc_list*) left;
    const struct cc_list* other = (const struct cc_list*) right;

    if (self->size != other->size || self->element_size != other->element_size)
    {
      return false;
    }

    const struct cc_list_node* a = self->front;
    const struct cc_list_node* b = other->front;
    cc_equal_fn equality = self->functions.equality;
    size_t elem_size = self->element_size;

    while (a && b)
    {
      if (!equality(a->data, b->data, elem_size))
      {
        return false;
      }

      a = a->next;
      b = b->next;
    }

    return true;
  }
  else
  {
    return false;
  }
}

struct cc_list*
cc_list_new(size_t element_size)
{
  return cc_list_new_f(element_size, cc_default_functions);
}

struct cc_list*
cc_list_from_array(const void* data, size_t count, size_t element_size)
{
  return cc_list_from_array_f(
      data,
      count,
      element_size,
      cc_default_functions
    );
}

struct cc_list*
cc_list_new_f(size_t element_size, const struct cc_functions functions)
{
  void* buffer = malloc(sizeof(struct cc_list));
  struct cc_list* self = (struct cc_list*) buffer;
  if (!self)
  {
    return NULL;
  }

  self->size = 0;
  self->element_size = element_size;
  self->functions = functions;
  self->front = NULL;
  self->back = NULL;
  return self;
}

struct cc_list*
cc_list_from_array_f(const void* data,
                     size_t count,
                     size_t element_size,
                     const struct cc_functions functions)
{
  void* buffer = malloc(sizeof(struct cc_list));
  struct cc_list* self = (struct cc_list*) buffer;
  if (!self)
  {
    return NULL;
  }

  self->size = 0;
  self->element_size = element_size;
  self->functions = functions;
  self->front = NULL;
  self->back = NULL;

  const void* p = data;
  for (size_t n = 0; n < count; ++n, p += element_size)
  {
    cc_list_push_back(self, p);
  }

  return self;
}

struct cc_list*
cc_list_copy(const struct cc_list* other)
{
  if (other)
  {
    void* buffer = malloc(sizeof(struct cc_list));
    struct cc_list* self = (struct cc_list*) buffer;
    if (!self)
    {
      return NULL;
    }

    if (!cc_list_copier(self, other, sizeof(struct cc_list)))
    {
      free(self);
      return NULL;
    }

    return self;
  }
  else
  {
    return NULL;
  }
}

void
cc_list_delete(struct cc_list* self)
{
  cc_list_clear(self);
  free(self);
}

void
cc_list_assign(struct cc_list* self, size_t count, const void* value)
{
  cc_list_clear(self);
  for (size_t n = 0; n < count; ++n)
  {
    cc_list_push_back(self, value);
  }
}

const void*
cc_list_front(const struct cc_list* self)
{
  if (self && self->front)
  {
    return self->front->data;
  }
  else
  {
    return NULL;
  }
}

struct cc_list_iterator
cc_list_begin(const struct cc_list* self)
{
  return (struct cc_list_iterator){
    .list = self,
    .node = self->front
  };
}

struct cc_list_iterator
cc_list_end(const struct cc_list* self)
{
  return (struct cc_list_iterator){
    .list = self,
    .node = NULL
  };
}

const void*
cc_list_back(const struct cc_list* self)
{
  if (self && self->back)
  {
    return self->back->data;
  }
  else
  {
    return NULL;
  }
}

bool
cc_list_empty(const struct cc_list* self)
{
  if (self)
  {
    return self->size == 0;
  }
  else
  {
    return true;
  }
}

size_t
cc_list_size(const struct cc_list* self)
{
  if (self)
  {
    return self->size;
  }
  else
  {
    return 0;
  }
}

void
cc_list_clear(struct cc_list* self)
{
  if (self)
  {
    cc_delete_fn deleter = self->functions.deleter;
    struct cc_list_node* node = self->front;
    struct cc_list_node* next;

    while (node)
    {
      next = node->next;
      deleter(node->data);
      free(node->data);
      free(node);
      node = next;
    }

    self->size = 0;
    self->front = NULL;
    self->back = NULL;
  }
}

void
cc_list_insert(struct cc_list* self,
               const struct cc_list_iterator pos,
               const void* value)
{
  if (self && value)
  {
    void* buffer = malloc(sizeof(struct cc_list_node));
    struct cc_list_node* node = (struct cc_list_node*) buffer;
    if (!node)
    {
      return;
    }

    void* data = malloc(self->element_size);
    if (!data)
    {
      free(node);
      return;
    }
    self->functions.copier(data, value, self->element_size);

    struct cc_list_node* before = pos.node ? pos.node->prev : self->back;
    struct cc_list_node* after = pos.node;

    node->prev = before;
    node->next = after;
    node->data = data;

    if (before)
    {
      before->next = node;
    }
    if (after)
    {
      after->prev = node;
    }

    if (after == self->front)
    {
      self->front = node;
    }
    if (before == self->back)
    {
      self->back = node;
    }

    ++self->size;
  }
}

void
cc_list_erase(struct cc_list* self,
              const struct cc_list_iterator first,
              const struct cc_list_iterator last)
{
  if (self && first.node)
  {
    struct cc_list_node* before = first.node->prev;
    struct cc_list_node* after = last.node ? last.node : NULL;

    if (before)
    {
      before->next = after;
    }
    else
    {
      self->front = after;
    }
    if (after)
    {
      after->prev = before;
    }
    else
    {
      self->back = before;
    }

    size_t erased = 0;
    struct cc_list_node* next;
    struct cc_list_node* node = first.node;
    cc_delete_fn deleter = self->functions.deleter;
    while (node != last.node)
    {
      next = node->next;
      deleter(node->data);
      free(node->data);
      free(node);
      ++erased;
      node = next;
    }

    self->size -= erased;
  }
}

void
cc_list_push_back(struct cc_list* self, const void* value)
{
  if (self && value)
  {
    void* buffer = malloc(sizeof(struct cc_list_node));
    struct cc_list_node* node = (struct cc_list_node*) buffer;
    if (!node)
    {
      return;
    }

    void* data = malloc(self->element_size);
    if (!data)
    {
      free(node);
      return;
    }
    self->functions.copier(data, value, self->element_size);

    node->prev = self->back;
    node->next = NULL;
    node->data = data;

    if (!self->front)
    {
      self->front = node;
    }

    if (self->back)
    {
      self->back->next = node;
      self->back = node;
    }
    else
    {
      self->back = node;
    }

    ++self->size;
  }
}

void
cc_list_pop_back(struct cc_list* self)
{
  if (self && self->back)
  {
    struct cc_list_node* previous = self->back->prev;
    self->functions.deleter(self->back->data);
    free(self->back->data);
    free(self->back);

    if (previous)
    {
      self->back = previous;
      previous->next = NULL;
    }
    else
    {
      self->back = NULL;
      self->front = NULL;
    }

    --(self->size);
  }
}

void
cc_list_push_front(struct cc_list* self, const void* value)
{
  if (self && value)
  {
    void* buffer = malloc(sizeof(struct cc_list_node));
    struct cc_list_node* node = (struct cc_list_node*) buffer;
    if (!node)
    {
      return;
    }

    void* data = malloc(self->element_size);
    if (!data)
    {
      free(node);
      return;
    }
    self->functions.copier(data, value, self->element_size);

    node->prev = NULL;
    node->next = self->front;
    node->data = data;

    if (!self->back)
    {
      self->back = node;
    }

    if (self->front)
    {
      self->front->prev = node;
      self->front = node;
    }
    else
    {
      self->front = node;
    }

    ++self->size;
  }
}

void
cc_list_pop_front(struct cc_list* self)
{
  if (self && self->front)
  {
    struct cc_list_node* next = self->front->next;
    self->functions.deleter(self->front->data);
    free(self->front->data);
    free(self->front);

    if (next)
    {
      self->front = next;
      next->prev = NULL;
    }
    else
    {
      self->front = NULL;
      self->back = NULL;
    }

    --(self->size);
  }
}

void
cc_list_resize(struct cc_list* self, size_t count, const void* value)
{
  if (self)
  {
    if (count < self->size)
    {
      size_t count_ = self->size - count;
      for (size_t n = 0; n < count_; ++n)
      {
        cc_list_pop_back(self);
      }
    }
    else if (count > self->size && value)
    {
      size_t count_ = count - self->size;
      for (size_t n = 0; n < count_; ++n)
      {
        cc_list_push_back(self, value);
      }
    }
  }
}

void
cc_list_swap(struct cc_list* self, struct cc_list* other)
{
  if (self && other)
  {
    size_t size = self->size;
    size_t element_size = self->element_size;
    struct cc_functions functions = self->functions;
    struct cc_list_node* front = self->front;
    struct cc_list_node* back = self->back;

    self->size = other->size;
    self->element_size = other->element_size;
    self->functions = other->functions;
    self->front = other->front;
    self->back = other->back;

    other->size = size;
    other->element_size = element_size;
    other->functions = functions;
    other->front = front;
    other->back = back;
  }
}

void
cc_list_merge(struct cc_list* self,
              struct cc_list* other,
              bool (*comp)(void*, void*))
{
  if (self && other && comp)
  {
    struct cc_list_node* a = self->front;
    struct cc_list_node* b = other->front;
    struct cc_list_node* c;

    while (a && b)
    {
      if (comp(b->data, a->data))
      {
        c = b->next;
        if (a->prev)
        {
          a->prev->next = b;
        }
        else
        {
          self->front = b;
        }
        b->prev = a->prev;
        b->next = a;
        a->prev = b;
        b = c;
      }
      else
      {
        a = a->next;
      }
    }

    if (b)
    {
      self->back->next = b;
      b->prev = self->back;
      self->back = other->back;
    }

    self->size += other->size;

    other->size = 0;
    other->front = NULL;
    other->back = NULL;
  }
}

void
cc_list_splice(struct cc_list* self,
               const struct cc_list_iterator pos,
               struct cc_list* other)
{
  if (self && other)
  {
    struct cc_list_node* before = pos.node ? pos.node->prev : self->back;
    struct cc_list_node* after = pos.node;

    if (before)
    {
      other->front->prev = before;
      before->next = other->front;
    }
    else
    {
      self->front = other->front;
    }

    if (after)
    {
      other->back->next = after;
      after->prev = other->back;
    }
    else
    {
      self->back = other->back;
    }

    self->size += other->size;
    other->size = 0;
    other->front = NULL;
    other->back = NULL;
  }
}

void
cc_list_remove(struct cc_list* self, const void* value)
{
  if (self && value)
  {
    size_t erased = 0;

    struct cc_list_node* next;
    struct cc_list_node* node = self->front;
    cc_delete_fn deleter = self->functions.deleter;
    cc_equal_fn equality = self->functions.equality;
    size_t elem_size = self->element_size;

    while (node)
    {
      next = node->next;

      if (equality(node->data, value, elem_size))
      {
        if (node == self->front)
        {
          self->front = node->next;
        }
        if (node == self->back)
        {
          self->back = node->prev;
        }

        if (node->prev)
        {
          node->prev->next = node->next;
        }
        if (node->next)
        {
          node->next->prev = node->prev;
        }

        deleter(node->data);
        free(node->data);
        free(node);
        ++erased;
      }

      node = next;
    }

    self->size -= erased;
  }
}

void
cc_list_remove_if(struct cc_list* self, bool (*pred)(void*))
{
  if (self && pred)
  {
    size_t erased = 0;
    struct cc_list_node* next;
    struct cc_list_node* node = self->front;
    cc_delete_fn deleter = self->functions.deleter;

    while (node)
    {
      next = node->next;

      if (pred(node->data))
      {
        if (node == self->front)
        {
          self->front = node->next;
        }
        if (node == self->back)
        {
          self->back = node->prev;
        }

        if (node->prev)
        {
          node->prev->next = node->next;
        }
        if (node->next)
        {
          node->next->prev = node->prev;
        }

        deleter(node->data);
        free(node->data);
        free(node);
        ++erased;
      }

      node = next;
    }

    self->size -= erased;
  }
}

void
cc_list_reverse(struct cc_list* self)
{
  if (self)
  {
    struct cc_list_node* node = self->front;
    struct cc_list_node* next;
    self->front = self->back;
    self->back = node;
    while (node)
    {
      next = node->next;
      node->next = node->prev;
      node->prev = node->next;
      node = next;
    }
  }
}

void
cc_list_unique(struct cc_list* self)
{
  if (self && self->front)
  {
    size_t erased = 0;
    size_t elem_size = self->element_size;
    struct cc_list_node* node = self->front;
    struct cc_list_node* next = self->front->next;
    cc_delete_fn deleter = self->functions.deleter;
    cc_equal_fn equal = self->functions.equality;

    while (next)
    {
      if (equal(node->data, next->data, elem_size))
      {
        node->next = next->next;
        if (next->next)
        {
          next->next->prev = node;
        }
        else
        {
          self->back = node;
        }

        deleter(next->data);
        free(next->data);
        free(next);
        ++erased;

        next = node->next;
      }
      else
      {
        node = next;
        next = node->next;
      }
    }

    self->size -= erased;
  }
}

void
cc_list_sort(struct cc_list* self, bool (*comp)(void*, void*))
{
  if (self && comp)
  {
    self->front = _cc_list_merge_sort(self->front, comp);
    struct cc_list_node* node = self->front;
    while (1)
    {
      if (!node->next)
      {
        break;
      }
      node = node->next;
    }
    self->back = node;
  }
}

bool
cc_list_eq(const struct cc_list* self, const struct cc_list* other)
{
  return cc_list_equality(self, other, sizeof(struct cc_list));
}

bool
cc_list_ne(const struct cc_list* self, const struct cc_list* other)
{
  return !cc_list_eq(self, other);
}

void
cc_list_iterator_increment(struct cc_list_iterator* self)
{
  if (self && self->node)
  {
    self->node = self->node->next;
  }
}

void
cc_list_iterator_decrement(struct cc_list_iterator* self)
{
  if (self)
  {
    if (self->node)
    {
      self->node = self->node->prev;
    }
    else
    {
      self->node = self->list->back;
    }
  }
}

void*
cc_list_iterator_dereference(const struct cc_list_iterator self)
{
  if (self.node)
  {
    return self.node->data;
  }
  else
  {
    return NULL;
  }
}

bool
cc_list_iterator_eq(const struct cc_list_iterator self,
                      const struct cc_list_iterator other)
{
  return self.list == other.list && self.node == other.node;
}

bool
cc_list_iterator_ne(const struct cc_list_iterator self,
                      const struct cc_list_iterator other)
{
  return self.list != other.list || self.node != other.node;
}
