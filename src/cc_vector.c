/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "cc_vector.h"

const size_t cc_vector_sizeof = sizeof(struct cc_vector);

const struct cc_functions cc_vector_functions = (struct cc_functions){
  .hasher = cc_vector_hasher,
  .copier = cc_vector_copier,
  .deleter = cc_vector_deleter,
  .equality = cc_vector_equality
};

void
_cc_vector_grow_as_needed(struct cc_vector* self, size_t requested)
{
  if (requested > self->capacity)
  {
    size_t current = self->capacity == 0 ? 1 : self->capacity;
    double fraction = 1.0 * requested / current;
    size_t exponent = lround(ceil(log2(fraction)));
    size_t capacity = current * (1 << exponent);
    cc_vector_reserve(self, capacity);
  }
}

uint64_t
cc_vector_hasher(const void* buffer, size_t size)
{
  const struct cc_vector* self = (const struct cc_vector*) buffer;

  uint64_t hash = 0;
  void* data = self->data;
  cc_hash_fn hasher = self->functions.hasher;
  size_t elem_size = self->element_size;

  for (size_t n = 0; n < self->size; ++n, data += elem_size)
  {
    cc_hash_combine(&hash, hasher(data, elem_size));
  }

  return hash;
}

void*
cc_vector_copier(void* dest, const void* src, size_t size)
{
  if (dest && src)
  {
    struct cc_vector* self = (struct cc_vector*) dest;
    const struct cc_vector* other = (const struct cc_vector*) src;

    void* data = malloc(other->size * other->element_size);
    if (!data)
    {
      return NULL;
    }

    void* a = data;
    void* b = other->data;
    cc_copy_fn copier = other->functions.copier;
    size_t elem_size = other->element_size;
    for (size_t n = 0; n < other->size; ++n, a += elem_size, b += elem_size)
    {
      copier(a, b, elem_size);
    }

    self->size = other->size;
    self->capacity = other->capacity;
    self->element_size = other->element_size;
    self->functions = other->functions;
    self->data = data;

    return self;
  }
  else
  {
    return NULL;
  }
}

void
cc_vector_deleter(void* ptr)
{
  if (ptr)
  {
    struct cc_vector* self = (struct cc_vector*) ptr;
    void* data = self->data;
    cc_delete_fn deleter = self->functions.deleter;
    size_t elem_size = self->element_size;

    for (size_t n = 0; n < self->size; ++n, data += elem_size)
    {
      deleter(data);
    }

    free(self->data);

    self->size = 0;
    self->capacity = 0;
    self->element_size = 0;
    self->functions = cc_default_functions;
    self->data = NULL;
  }
}

bool
cc_vector_equality(const void* left, const void* right, size_t size)
{
  if (left && right)
  {
    const struct cc_vector* self = (const struct cc_vector*) left;
    const struct cc_vector* other = (const struct cc_vector*) right;

    if (self->size != other->size || self->element_size != other->element_size)
    {
      return false;
    }

    void* a = self->data;
    void* b = other->data;
    cc_equal_fn equality = self->functions.equality;
    size_t elem_size = self->element_size;

    for (size_t n = 0; n < self->size; ++n, a += elem_size, b += elem_size)
    {
      if (!equality(a, b, elem_size))
      {
        return false;
      }
    }

    return true;
  }
  else
  {
    return false;
  }
}

struct cc_vector*
cc_vector_new(size_t element_size)
{
  return cc_vector_new_f(element_size, cc_default_functions);
}

struct cc_vector*
cc_vector_from_array(const void* data, size_t count, size_t element_size)
{
  return cc_vector_from_array_f(
      data,
      count,
      element_size,
      cc_default_functions
    );
}

struct cc_vector*
cc_vector_new_f(size_t element_size, const struct cc_functions functions)
{
  void* buffer = malloc(sizeof(struct cc_vector));
  struct cc_vector* self = (struct cc_vector*) buffer;
  if (!self)
  {
    return NULL;
  }

  self->size = 0;
  self->capacity = 0;
  self->element_size = element_size;
  self->functions = functions;
  self->data = NULL;

  return self;
}

struct cc_vector*
cc_vector_from_array_f(const void* data,
                       size_t count,
                       size_t element_size,
                       const struct cc_functions functions)
{
  void* buffer = malloc(sizeof(struct cc_vector));
  struct cc_vector* self = (struct cc_vector*) buffer;
  if (!self)
  {
    return NULL;
  }

  self->data = malloc(count * element_size);
  if (!self->data)
  {
    free(self);
    return NULL;
  }

  cc_copy_fn copier = functions.copier;
  for (size_t n = 0, offset = 0; n < count; ++n, offset += element_size)
  {
    copier(self->data + offset, data + offset, element_size);
  }

  self->size = count;
  self->capacity = count;
  self->element_size = element_size;
  self->functions = functions;

  return self;
}

struct cc_vector*
cc_vector_copy(const struct cc_vector* other)
{
  if (other)
  {
    void* buffer = malloc(sizeof(struct cc_vector));
    struct cc_vector* self = (struct cc_vector*) buffer;
    if (!self)
    {
      return NULL;
    }

    if (!cc_vector_copier(self, other, sizeof(struct cc_vector)))
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
cc_vector_delete(struct cc_vector* self)
{
  cc_vector_deleter(self);
  free(self);
}

void
cc_vector_assign(struct cc_vector* self, size_t count, const void* value)
{
  if (self)
  {
    _cc_vector_grow_as_needed(self, count);

    size_t elem_size = self->element_size;
    cc_copy_fn copier = self->functions.copier;
    cc_delete_fn deleter = self->functions.deleter;
    for (size_t n = 0, offset = 0; n < self->size; ++n, offset += elem_size)
    {
      deleter(self->data + offset);
    }
    for (size_t n = 0, offset = 0; n < count; ++n, offset += elem_size)
    {
      copier(self->data + offset, value, elem_size);
    }

    self->size = count;
  }
}

void
cc_vector_set(struct cc_vector* self, size_t pos, const void* value)
{
  if (self && self->data && pos < self->size)
  {
    size_t offset = pos * self->element_size;
    self->functions.deleter(self->data + offset);
    self->functions.copier(self->data + offset, value, self->element_size);
  }
}

const void*
cc_vector_get(const struct cc_vector* self, size_t pos)
{
  if (self && self->data && pos < self->size)
  {
    size_t offset = pos * self->element_size;
    return self->data + offset;
  }
  else
  {
    return NULL;
  }
}

const void*
cc_vector_front(const struct cc_vector* self)
{
  if (self && self->data && self->size > 0)
  {
    return self->data;
  }
  else
  {
    return NULL;
  }
}

const void*
cc_vector_back(const struct cc_vector* self)
{
  if (self && self->data && self->size > 0)
  {
    size_t offset = (self->size - 1) * self->element_size;
    return self->data + offset;
  }
  else
  {
    return NULL;
  }
}

const void*
cc_vector_data(const struct cc_vector* self)
{
  if (self)
  {
    return self->data;
  }
  else
  {
    return NULL;
  }
}

struct cc_vector_iterator
cc_vector_begin(struct cc_vector* self)
{
  return (struct cc_vector_iterator){
    .element_size = self->element_size,
    .data = self->data
  };
}

struct cc_vector_iterator
cc_vector_end(struct cc_vector* self)
{
  return (struct cc_vector_iterator){
    .element_size = self->element_size,
    .data = self->data + self->size * self->element_size
  };
}

bool
cc_vector_empty(const struct cc_vector* self)
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
cc_vector_size(const struct cc_vector* self)
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
cc_vector_reserve(struct cc_vector* self, size_t new_cap)
{
  if (self && new_cap > self->capacity)
  {
    void* data = malloc(new_cap * self->element_size);
    if (!data)
    {
      return;
    }

    size_t elem_size = self->element_size;
    cc_copy_fn copier = self->functions.copier;
    cc_delete_fn deleter = self->functions.deleter;
    for (size_t n = 0, offset = 0; n < self->size; ++n, offset += elem_size)
    {
      copier(data + offset, self->data + offset, elem_size);
      deleter(self->data + offset);
    }
    free(self->data);

    self->capacity = new_cap;
    self->data = data;
  }
}

size_t
cc_vector_capacity(const struct cc_vector* self)
{
  if (self)
  {
    return self->capacity;
  }
  else
  {
    return 0;
  }
}

void
cc_vector_shrink_to_fit(struct cc_vector* self)
{
  if (self && self->capacity > self->size)
  {
    void* data = malloc(self->size * self->element_size);
    if (!data)
    {
      return;
    }

    size_t elem_size = self->element_size;
    cc_copy_fn copier = self->functions.copier;
    cc_delete_fn deleter = self->functions.deleter;
    for (size_t n = 0, offset = 0; n < self->size; ++n, offset += elem_size)
    {
      copier(data + offset, self->data + offset, elem_size);
      deleter(self->data + offset);
    }
    free(self->data);

    self->capacity = self->size;
    self->data = data;
  }
}

void
cc_vector_clear(struct cc_vector* self)
{
  if (self)
  {
    self->size = 0;
  }
}

void
cc_vector_insert(struct cc_vector* self, size_t pos, const void* value)
{
  if (self && pos <= self->size)
  {
    _cc_vector_grow_as_needed(self, self->size + 1);

    size_t elem_size = self->element_size;
    cc_copy_fn copier = self->functions.copier;
    cc_delete_fn deleter = self->functions.deleter;
    void* dest = self->data + self->size * self->element_size;
    void* src = self->data + (self->size - 1) * self->element_size;
    for (size_t n = pos; n < self->size; ++n)
    {
      copier(dest, src, elem_size);
      deleter(src);
      dest -= elem_size;
      src -= elem_size;
    }
    copier(self->data + pos * elem_size, value, elem_size);

    self->size += 1;
  }
}

void
cc_vector_erase(struct cc_vector* self, size_t first, size_t last)
{
  if (self && self->data)
  {
    first = first < self->size ? first : self->size;
    last = last < self->size ? last : self->size;

    if (first < last)
    {
      size_t n = first;
      size_t offset = first * self->element_size;
      size_t elem_size = self->element_size;
      cc_delete_fn deleter = self->functions.deleter;
      for (; n < last; ++n, offset += elem_size)
      {
        deleter(self->data + offset);
      }

      void* dest = self->data + first * elem_size;
      const void* src = self->data + last * elem_size;
      cc_copy_fn copier = self->functions.copier;
      for (n = last, offset = 0; n < self->size; ++n, offset += elem_size)
      {
        copier(dest + offset, src + offset, elem_size);
      }

      self->size -= last - first;
    }
  }
}

void
cc_vector_push_back(struct cc_vector* self, const void* value)
{
  if (self)
  {
    size_t size = self->size + 1;
    size_t offset = self->size * self->element_size;

    _cc_vector_grow_as_needed(self, size);

    self->functions.copier(self->data + offset, value, self->element_size);

    self->size = size;
  }
}

void
cc_vector_pop_back(struct cc_vector* self)
{
  if (self && self->size > 0)
  {
    self->size -= 1;
  }
}

void
cc_vector_resize(struct cc_vector* self, size_t count, const void* value)
{
  if (self)
  {
    if (count < self->size)
    {
      self->size = count;
    }
    else if (count > self->size)
    {
      _cc_vector_grow_as_needed(self, count);

      size_t n = self->size;
      size_t offset = self->size * self->element_size;
      size_t elem_size = self->element_size;
      cc_copy_fn copier = self->functions.copier;
      for (; n < count; ++n, offset += elem_size)
      {
        copier(self->data + offset, value, elem_size);
      }

      self->size = count;
    }
  }
}

void
cc_vector_swap(struct cc_vector* self, struct cc_vector* other)
{
  if (self && other)
  {
    size_t size = self->size;
    size_t capacity = self->capacity;
    size_t element_size = self->element_size;
    struct cc_functions functions = self->functions;
    void* data = self->data;

    self->size = other->size;
    self->capacity = other->capacity;
    self->element_size = other->element_size;
    self->functions = other->functions;
    self->data = other->data;

    other->size = size;
    other->capacity = capacity;
    other->element_size = element_size;
    other->functions = functions;
    other->data = data;
  }
}

bool
cc_vector_eq(const struct cc_vector* self, const struct cc_vector* other)
{
  return cc_vector_equality(self, other, sizeof(struct cc_vector));
}

bool
cc_vector_ne(const struct cc_vector* self, const struct cc_vector* other)
{
  return !cc_vector_eq(self, other);
}

void
cc_vector_iterator_increment(struct cc_vector_iterator* self)
{
  self->data = (char*) self->data + self->element_size;
}

void
cc_vector_iterator_decrement(struct cc_vector_iterator* self)
{
  self->data = (char*) self->data - self->element_size;
}

void
cc_vector_iterator_advance(struct cc_vector_iterator* self, ptrdiff_t n)
{
  self->data = (char*) self->data + n * self->element_size;
}

ptrdiff_t
cc_vector_iterator_distance_to(const struct cc_vector_iterator self,
                               const struct cc_vector_iterator other)
{
  return ((char*) other.data - (char*) self.data) / self.element_size;
}

void*
cc_vector_iterator_dereference(const struct cc_vector_iterator self)
{
  return self.data;
}

bool
cc_vector_iterator_eq(const struct cc_vector_iterator self,
                      const struct cc_vector_iterator other)
{
  return self.data == other.data;
}

bool
cc_vector_iterator_ne(const struct cc_vector_iterator self,
                      const struct cc_vector_iterator other)
{
  return self.data != other.data;
}
