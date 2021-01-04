/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "cc_map.h"

const size_t cc_map_sizeof = sizeof(struct cc_map);

const struct cc_functions cc_map_functions = (struct cc_functions){
  .hasher = cc_map_hasher,
  .copier = cc_map_copier,
  .deleter = cc_map_deleter,
  .equality = cc_map_equality
};

void
_cc_map_node_init(struct cc_map* self,
                     struct cc_map_node* node,
                     const void* key,
                     const void* value)
{
  self->key_functions.deleter(node->key);
  self->value_functions.deleter(node->value);
  self->key_functions.copier(node->key, key, self->key_size);
  self->value_functions.copier(node->value, value, self->value_size);
  node->hash = self->key_functions.hasher(key, self->key_size);
  node->length = 1;
}

void
_cc_map_node_copy(struct cc_map* self,
                     struct cc_map_node* node,
                     const struct cc_map_node* other)
{
  self->key_functions.deleter(node->key);
  self->value_functions.deleter(node->value);
  self->key_functions.copier(node->key, other->key, self->key_size);
  self->value_functions.copier(node->value, other->value, self->value_size);
  node->hash = other->hash;
  node->length = other->length;
}

void
_cc_map_node_free(struct cc_map* self, struct cc_map_node* node)
{
  if (node->length > 0)
  {
    self->key_functions.deleter(node->key);
    self->value_functions.deleter(node->value);
    node->length = 0;
  }
}

struct cc_map_node*
_cc_map_get(const struct cc_map* self, const void* key)
{
  uint64_t hash = self->key_functions.hasher(key, self->key_size);
  size_t pos = ((size_t) hash + self->max_length / 2) % self->capacity;
  struct cc_map_node* node;

  int sign = 1;
  for (size_t n = 1; n <= self->max_length; ++n)
  {
    node = self->nodes + (pos % self->capacity);
    if (hash == node->hash
        && self->key_functions.equality(key, node->key, self->key_size))
    {
      return node;
    }
    sign *= -1;
    pos += sign * n;
  }

  return NULL;
}

size_t
_cc_map_capacity(const struct cc_map* self, size_t count)
{
  size_t size = self->size > count ? self->size : count > 0 ? count : 1;
  size_t exponent = lround(ceil(log2((1.0 / self->max_load_factor) * size)));
  return 1 << (exponent > 4 ? exponent : 4);
}

void
_cc_map_resize(struct cc_map* self, size_t new_capacity)
{
  size_t size;
  size_t length = new_capacity + 2;
  void* buffer = malloc(length * sizeof(struct cc_map_node));
  if (!buffer)
  {
    return;
  }
  struct cc_map_node* nodes = (struct cc_map_node*) buffer;

  size = length * self->key_size;
  void* keys = malloc(size);
  if (!keys)
  {
    free(nodes);
    return;
  }
  memset(keys, 0, size);

  size = length * self->value_size;
  void* values = malloc(size);
  if (!values)
  {
    free(keys);
    free(nodes);
    return;
  }
  memset(values, 0, size);

  void* key = keys;
  void* value = values;
  struct cc_map_node* node = nodes;
  for (size_t n = 0; n < length; ++n, ++node)
  {
    node->key = key;
    node->value = value;
    node->hash = 0;
    node->length = 0;
    key += self->key_size;
    value += self->value_size;
  }

  size_t old_capacity = self->capacity;

  node = nodes;
  nodes = self->nodes;

  self->size = 0;
  self->capacity = new_capacity;
  self->nodes = node;

  if (nodes)
  {
    node = nodes;
    for (size_t n = 0; n < old_capacity; ++n, ++node)
    {
      if (node->length > 0)
      {
        cc_map_insert(self, node->key, node->value);
      }
    }

    free(nodes->key);
    free(nodes->value);
    free(nodes);
  }
}

uint64_t
cc_map_hasher(const void* buffer, size_t size)
{
  const struct cc_map* self = (const struct cc_map*) buffer;

  uint64_t hash = 0;
  size_t key_size = self->key_size;
  size_t value_size = self->value_size;
  cc_hash_fn key_hasher = self->key_functions.hasher;
  cc_hash_fn value_hasher = self->value_functions.hasher;
  const struct cc_map_node* node = self->nodes;

  for (size_t n = 0; n < self->capacity; ++n, ++node)
  {
    if (node->length > 0)
    {
      cc_hash_combine(&hash, key_hasher(node->key, key_size));
      cc_hash_combine(&hash, value_hasher(node->value, value_size));
    }
  }

  return hash;
}

void*
cc_map_copier(void* dest, const void* src, size_t size)
{
  if (dest && src)
  {
    struct cc_map* self = (struct cc_map*) dest;
    const struct cc_map* other = (const struct cc_map*) src;

    self->size = 0;
    self->capacity = 0;
    self->key_size = other->key_size;
    self->value_size = other->value_size;
    self->max_length = other->max_length;
    self->max_load_factor = other->max_load_factor;
    self->key_functions = other->key_functions;
    self->value_functions = other->value_functions;
    self->nodes = NULL;

    _cc_map_resize(self, other->capacity);

    const struct cc_map_node* node = other->nodes;
    for (size_t n = 0; n < other->capacity; ++n, ++node)
    {
      if (node->length > 0)
      {
        cc_map_insert(self, node->key, node->value);
      }
    }

    return self;
  }
  else
  {
    return NULL;
  }
}

void
cc_map_deleter(void* ptr)
{
 if (ptr)
 {
   struct cc_map* self = (struct cc_map*) ptr;
   struct cc_map_node* node = self->nodes;

   for (size_t n = 0; n < self->capacity; ++n, ++node)
   {
     _cc_map_node_free(self, node);
   }

   free(self->nodes->value);
   free(self->nodes->key);
   free(self->nodes);

   self->size = 0;
   self->capacity = 0;
   self->key_size = 0;
   self->value_size = 0;
   self->max_length = 0;
   self->max_load_factor = 0.0;
   self->key_functions = cc_default_functions;
   self->value_functions = cc_default_functions;
   self->nodes = NULL;
 }
}

bool
cc_map_equality(const void* left, const void* right, size_t size)
{
  if (left && right)
  {
    const struct cc_map* self = (const struct cc_map*) left;
    const struct cc_map* other = (const struct cc_map*) right;

    if (self->size != other->size
        || self->key_size != other->key_size
        || self->value_size != other->value_size)
    {
      return false;
    }

    const struct cc_map_node* a = self->nodes;
    const struct cc_map_node* b;
    cc_equal_fn key_equality = self->key_functions.equality;
    cc_equal_fn value_equality = self->value_functions.equality;
    size_t key_size = self->key_size;
    size_t value_size = self->value_size;

    for (size_t n = 0; n < self->capacity; ++n, ++a)
    {
      if (a->length > 0)
      {
        b = _cc_map_get(other, a->key);
        if (a->hash != b->hash
            || !key_equality(a->key, b->key, key_size)
            || !value_equality(a->value, b->value, value_size))
        {
          return false;
        }
      }
    }

    return true;
  }
  else
  {
    return false;
  }
}

struct cc_map*
cc_map_new(size_t key_size, size_t value_size)
{
  return cc_map_new_f(
      key_size,
      value_size,
      cc_default_functions,
      cc_default_functions
    );
}

struct cc_map*
cc_map_from_arrays(const void* keys,
                   const void* values,
                   size_t count,
                   size_t key_size,
                   size_t value_size)
{
  return cc_map_from_arrays_f(
      keys,
      values,
      count,
      key_size,
      value_size,
      cc_default_functions,
      cc_default_functions
    );
}

struct cc_map*
cc_map_new_f(size_t key_size,
             size_t value_size,
             const struct cc_functions key_functions,
             const struct cc_functions value_functions)
{
  void* buffer = malloc(sizeof(struct cc_map));
  struct cc_map* self = (struct cc_map*) buffer;
  if (!self)
  {
    return NULL;
  }

  self->size = 0;
  self->capacity = 0;
  self->key_size = key_size;
  self->value_size = value_size;
  self->max_length = 0;
  self->max_load_factor = 0.8;
  self->key_functions = key_functions;
  self->value_functions = value_functions;

  self->nodes = NULL;

  _cc_map_resize(self, _cc_map_capacity(self, 0));

  return self;
}

struct cc_map*
cc_map_from_arrays_f(const void* keys,
                     const void* values,
                     size_t count,
                     size_t key_size,
                     size_t value_size,
                     const struct cc_functions key_functions,
                     const struct cc_functions value_functions)
{
  struct cc_map* self = cc_map_new_f(
      key_size,
      value_size,
      key_functions,
      value_functions
    );

  const void* key = keys;
  const void* value = values;
  for (size_t n = 0; n < count; ++n, key += key_size, value += value_size)
  {
    cc_map_insert(self, key, value);
  }

  return self;
}

struct cc_map*
cc_map_copy(const struct cc_map* other)
{
  void* buffer = malloc(sizeof(struct cc_map));
  struct cc_map* self = (struct cc_map*) buffer;
  if (!self)
  {
    return NULL;
  }

  if (!cc_map_copier(self, other, sizeof(struct cc_map)))
  {
    free(self);
    return NULL;
  }

  return self;
}

void
cc_map_delete(struct cc_map* self)
{
  cc_map_deleter(self);
  free(self);
}

struct cc_map_iterator
cc_map_begin(struct cc_map* self)
{
  struct cc_map_node* node = self->nodes;
  struct cc_map_node* last = self->nodes + self->capacity;
  while (node->length == 0 && node != last)
  {
    ++node;
  }

  return (struct cc_map_iterator){
    .map = self,
    .node = node
  };
}

struct cc_map_iterator
cc_map_end(struct cc_map* self)
{
  return (struct cc_map_iterator){
    .map = self,
    .node = self->nodes + self->capacity
  };
}

bool
cc_map_empty(const struct cc_map* self)
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
cc_map_size(const struct cc_map* self)
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

size_t
cc_map_capacity(const struct cc_map* self)
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
cc_map_clear(struct cc_map* self)
{
  if (self)
  {
    struct cc_map_node* node = self->nodes;
    for (size_t n = 0; n < self->capacity; ++n, ++node)
    {
      _cc_map_node_free(self, node);
    }
    self->size = 0;
  }
}

void
cc_map_insert(struct cc_map* self, const void* key, const void* value)
{
  if (self && key && value)
  {
    struct cc_map_node* current = self->nodes + self->capacity;
    struct cc_map_node* swap = current + 1;

    _cc_map_node_init(self, current, key, value);

    size_t pos = ((size_t) current->hash) % self->capacity;
    struct cc_map_node* existing = self->nodes + pos;
    cc_copy_fn copier = self->value_functions.copier;

    while (true)
    {
      if (existing->length == 0)
      {
        _cc_map_node_copy(self, existing, current);
        if (current->length > self->max_length)
        {
          self->max_length = current->length;
        }

        ++self->size;

        double load_factor = (double) self->size / (double) self->capacity;
        if (load_factor > self->max_load_factor)
        {
          _cc_map_resize(self, _cc_map_capacity(self, self->size));
        }

        break;
      }
      if (current->hash == existing->hash)
      {
        copier(existing->value, current->value, self->value_size);
        break;
      }
      if (current->length > existing->length)
      {
        _cc_map_node_copy(self, swap, existing);
        _cc_map_node_copy(self, existing, current);
        _cc_map_node_copy(self, current, swap);
      }

      ++current->length;
      pos = (pos + 1) % self->capacity;
      existing = self->nodes + pos;
    }
  }
}

void
cc_map_erase(struct cc_map* self, const void* key)
{
  if (self && key)
  {
    struct cc_map_node* node = _cc_map_get(self, key);
    _cc_map_node_free(self, node);

    size_t pos = (node - self->nodes + 1) % self->capacity;
    struct cc_map_node* next = self->nodes + pos;
    while (next->length > 1)
    {
      next->length -= 1;
      _cc_map_node_copy(self, node, next);
      _cc_map_node_free(self, next);

      node = next;
      pos = (pos + 1) % self->capacity;
      next = self->nodes + pos;
    }

    --(self->size);
  }
}

void
cc_map_swap(struct cc_map* self, struct cc_map* other)
{
  if (self && other)
  {
    size_t size = self->size;
    size_t capacity = self->capacity;
    size_t key_size = self->key_size;
    size_t value_size = self->value_size;
    uint16_t max_length = self->max_length;
    double max_load_factor = self->max_load_factor;
    struct cc_functions key_functions = self->key_functions;
    struct cc_functions value_functions = self->value_functions;
    struct cc_map_node* nodes = self->nodes;

    self->size = other->size;
    self->capacity = other->capacity;
    self->key_size = other->key_size;
    self->value_size = other->value_size;
    self->max_length = other->max_length;
    self->max_load_factor = other->max_load_factor;
    self->key_functions = other->key_functions;
    self->value_functions = other->value_functions;
    self->nodes = other->nodes;

    other->size = size;
    other->capacity = capacity;
    other->key_size = key_size;
    other->value_size = value_size;
    other->max_length = max_length;
    other->max_load_factor = max_load_factor;
    other->key_functions = key_functions;
    other->value_functions = value_functions;
    other->nodes = nodes;
  }
}

void
cc_map_merge(struct cc_map* self, struct cc_map* other)
{
  if (self && other)
  {
    struct cc_map_node* node = other->nodes;
    for (size_t n = 0; n < other->capacity; ++n, ++node)
    {
      if (node->length > 0)
      {
        if (!_cc_map_get(self, node->key))
        {
          cc_map_insert(self, node->key, node->value);
          cc_map_erase(other, node->key);
        }
      }
    }
  }
}

void*
cc_map_find(const struct cc_map* self, const void* key)
{
  if (self && key)
  {
    struct cc_map_node* node = _cc_map_get(self, key);
    if (node)
    {
      return node->value;
    }
  }

  return NULL;
}

bool
cc_map_contains(const struct cc_map* self, const void* key)
{
  return cc_map_find(self, key);
}

double
cc_map_load_factor(const struct cc_map* self)
{
  if (self)
  {
    return (double) self->size / (double) self->capacity;
  }
  else
  {
    return 0.0;
  }
}

double
cc_map_max_load_factor(const struct cc_map* self)
{
  if (self)
  {
    return self->max_load_factor;
  }
  else
  {
    return 0.0;
  }
}

void
cc_map_set_max_load_factor(struct cc_map* self, double factor)
{
  if (self)
  {
    self->max_load_factor = factor;
  }
}

void
cc_map_reserve(struct cc_map* self, size_t count)
{
  if (self)
  {
    _cc_map_resize(self, _cc_map_capacity(self, count));
  }
}

bool
cc_map_eq(const struct cc_map* self, const struct cc_map* other)
{
  return cc_map_equality(self, other, sizeof(struct cc_map));
}

bool
cc_map_ne(const struct cc_map* self, const struct cc_map* other)
{
  return !cc_map_eq(self, other);
}

void
cc_map_iterator_increment(struct cc_map_iterator* self)
{
  ++self->node;
  struct cc_map_node* last = self->map->nodes + self->map->capacity;
  while (self->node->length == 0 && self->node != last)
  {
    ++self->node;
  }
}

void
cc_map_iterator_decrement(struct cc_map_iterator* self)
{
  --self->node;
  while (self->node->length == 0 && self->node != self->map->nodes)
  {
    --self->node;
  }
}

struct cc_map_key_value
cc_map_iterator_dereference(const struct cc_map_iterator self)
{
  return (struct cc_map_key_value){
    .key = self.node->key,
    .value = self.node->value
  };
}

bool
cc_map_iterator_eq(const struct cc_map_iterator self,
                   const struct cc_map_iterator other)
{
  return self.map == other.map && self.node == other.node;
}

bool
cc_map_iterator_ne(const struct cc_map_iterator self,
                   const struct cc_map_iterator other)
{
  return self.map != other.map || self.node != other.node;
}
