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
#include "cc_string.h"

const size_t cc_npos = SIZE_MAX;

const size_t cc_string_sizeof = sizeof(struct cc_string);

const struct cc_functions cc_string_functions = (struct cc_functions){
  .hasher = cc_string_hasher,
  .copier = cc_string_copier,
  .deleter = cc_string_deleter,
  .equality = cc_string_equality
};

size_t
_cc_string_capacity(size_t requested)
{
  double fraction = 1.0 * (requested < 16 ? 16 : requested + 1);
  size_t exponent = lround(ceil(log2(fraction)));
  return (1 << exponent) - 1;
}

char*
_cc_strrchr(const char* str, int ch, size_t count)
{
  const char* p = str + count - 1;
  for (size_t n = 0; n < count; ++n)
  {
    if (*p == ch)
    {
      return (char*) p;
    }
    --p;
  }
  return NULL;
}

uint64_t
cc_string_hasher(const void* buffer, size_t size)
{
  const struct cc_string* self = (const struct cc_string*) buffer;
  return cc_default_hasher(self->data, self->size);
}

void*
cc_string_copier(void* dest, const void* src, size_t size)
{
  if (dest && src)
  {
    struct cc_string* self = (struct cc_string*) dest;
    const struct cc_string* other = (const struct cc_string*) src;

    size_t capacity = _cc_string_capacity(other->size);
    char* data = (char*) malloc(capacity + 1);
    if (!data)
    {
      return NULL;
    }

    memcpy(data, other->data, other->size);
    memset(data + other->size, 0, capacity - other->size + 1);

    self->size = other->size;
    self->capacity = other->capacity;
    self->data = data;

    return self;
  }
  else
  {
    return NULL;
  }
}

void
cc_string_deleter(void* ptr)
{
  if (ptr)
  {
    struct cc_string* self = (struct cc_string*) ptr;

    free(self->data);

    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
  }
}

bool
cc_string_equality(const void* left, const void* right, size_t size)
{
  const struct cc_string* self = (const struct cc_string*) left;
  const struct cc_string* other = (const struct cc_string*) right;

  return self->size == other->size
      && strncmp(self->data, other->data, self->size) == 0;
}

struct cc_string*
cc_string_new()
{
  void* buffer = malloc(sizeof(struct cc_string));
  struct cc_string* self = (struct cc_string*) buffer;
  if (!self)
  {
    return NULL;
  }

  size_t capacity = _cc_string_capacity(0);
  void* data = malloc(capacity + 1);
  if (!data)
  {
    free(self);
    return NULL;
  }
  memset(data, 0, capacity + 1);

  self->size = 0;
  self->capacity = capacity;
  self->data = data;
  return self;
}

struct cc_string*
cc_string_from_chars(const char* s, size_t count)
{
  void* buffer = malloc(sizeof(struct cc_string));
  struct cc_string* self = (struct cc_string*) buffer;
  if (!self)
  {
    return NULL;
  }

  size_t capacity = _cc_string_capacity(count);
  char* data = (char*) malloc(capacity + 1);
  if (!data)
  {
    free(self);
    return NULL;
  }
  memcpy(data, s, count);
  memset(data + count, 0, capacity - count + 1);

  self->size = count;
  self->capacity = capacity;
  self->data = data;
  return self;
}

struct cc_string*
cc_string_copy(const struct cc_string* other)
{
  if (other)
  {
    void* buffer = malloc(sizeof(struct cc_string));
    struct cc_string* self = (struct cc_string*) buffer;
    if (!self)
    {
      return NULL;
    }

    if (!cc_string_copier(self, other, sizeof(struct cc_string)))
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
cc_string_delete(struct cc_string* self)
{
  cc_string_deleter(self);
  free(self);
}

void
cc_string_assign(struct cc_string* self, size_t count, char ch)
{
  if (self)
  {
    cc_string_reserve(self, count);
    memset(self->data, ch, count);
    memset(self->data + count, 0, self->capacity - count + 1);
    self->size = count;
  }
}

void
cc_string_set(struct cc_string* self, size_t pos, char ch)
{
  if (self && self->data && pos < self->size)
  {
    self->data[pos] = ch;
  }
}

char
cc_string_get(const struct cc_string* self, size_t pos)
{
  if (self && self->data && pos < self->size)
  {
    return self->data[pos];
  }
  else
  {
    return '\0';
  }
}

char
cc_string_front(const struct cc_string* self)
{
  if (self && self->data && self->size > 0)
  {
    return self->data[0];
  }
  else
  {
    return '\0';
  }
}

char
cc_string_back(const struct cc_string* self)
{
  if (self && self->data && self->size > 0)
  {
    return self->data[self->size - 1];
  }
  else
  {
    return '\0';
  }
}

const char*
cc_string_data(const struct cc_string* self)
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

struct cc_string_iterator
cc_string_begin(struct cc_string* self)
{
  if (self)
  {
    return (struct cc_string_iterator){.data = self->data};
  }
  else
  {
    return (struct cc_string_iterator){.data = NULL};
  }
}

struct cc_string_iterator
cc_string_end(struct cc_string* self)
{
  if (self)
  {
    return (struct cc_string_iterator){.data = self->data + self->size};
  }
  else
  {
    return (struct cc_string_iterator){.data = NULL};
  }
}

bool
cc_string_empty(const struct cc_string* self)
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
cc_string_size(const struct cc_string* self)
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
cc_string_reserve(struct cc_string* self, size_t new_cap)
{
  size_t capacity = _cc_string_capacity(new_cap);
  if (self && capacity > self->capacity)
  {
    char* data = malloc(capacity + 1);
    if (!data)
    {
      return;
    }

    memcpy(data, self->data, self->size);
    memset(data + self->size, 0, capacity - self->size + 1);
    free(self->data);

    self->capacity = capacity;
    self->data = data;
  }
}

size_t
cc_string_capacity(const struct cc_string* self)
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
cc_string_shrink_to_fit(struct cc_string* self)
{
  if (self && self->capacity > self->size)
  {
    char* data = (char*) malloc(self->size + 1);
    if (!data)
    {
      return;
    }

    memcpy(data, self->data, self->size);
    data[self->size] = '\0';
    free(self->data);

    self->capacity = self->size;
    self->data = data;
  }
}

void
cc_string_clear(struct cc_string* self)
{
  if (self)
  {
    memset(self->data, 0, self->size);
    self->size = 0;
  }
}

void
cc_string_insert(struct cc_string* self,
                  size_t pos,
                  const char* s,
                  size_t count)
{
  if (self && pos <= self->size && s && count > 0)
  {
    cc_string_reserve(self, self->size + count);

    for (size_t m = 0, n = self->size - 1; m < self->size - pos; ++m, --n)
    {
      self->data[n + count] = self->data[n];
    }
    memcpy(self->data + pos, s, count);

    self->size += count;
    self->data[self->size] = '\0';
  }
}

void
cc_string_erase(struct cc_string* self, size_t first, size_t last)
{
  if (self && self->data && first < self->size && first < last)
  {
    last = last < self->size ? last : self->size;
    size_t erased = last - first;
    char* dest = self->data + first;
    const char* src = self->data + last;
    for (size_t n = 0; n < self->size - last; ++n)
    {
      *dest = *src;
      ++dest;
      ++src;
    }
    self->size -= erased;
    memset(self->data + self->size, 0, erased);
  }
}

void
cc_string_push_back(struct cc_string* self, char ch)
{
  if (self)
  {
    cc_string_reserve(self, _cc_string_capacity(self->size + 1));
    self->data[self->size] = ch;
    self->size += 1;
    self->data[self->size] = '\0';
  }
}

void
cc_string_pop_back(struct cc_string* self)
{
  if (self && self->size > 0)
  {
    self->data[self->size] = '\0';
    self->size -= 1;
  }
}

void
cc_string_append(struct cc_string* self, const char* s, size_t count)
{
  if (self)
  {
    cc_string_reserve(self, _cc_string_capacity(self->size + count));

    memcpy(self->data + self->size, s, count);

    self->size += count;
    self->data[self->size] = '\0';
  }
}

int
cc_string_compare(const struct cc_string* self,
                   const struct cc_string* other)
{
  size_t rlen = self->size < other->size ? self->size : other->size;
  int result = strncmp(self->data, other->data, rlen);

  if (result == 0)
  {
    return self->size < other->size ? -1 : (self->size > other->size ? 1 : 0);
  }

  return result;
}

bool
cc_string_starts_with(const struct cc_string* self, const char* s)
{
  return strncmp(self->data, s, strlen(s)) == 0;
}

bool
cc_string_ends_with(const struct cc_string* self, const char* s)
{
  return strncmp(self->data + self->size - strlen(s), s, strlen(s)) == 0;
}

void
cc_string_replace(struct cc_string* self,
                   size_t pos,
                   size_t count,
                   const char* s,
                   size_t count2)
{
  cc_string_erase(self, pos, pos + count);
  cc_string_insert(self, pos, s, count2);
}

struct cc_string*
cc_string_substr(const struct cc_string* self, size_t pos, size_t count)
{
  return cc_string_from_chars(self->data + pos, count);
}

void
cc_string_resize(struct cc_string* self, size_t count, char ch)
{
  if (self)
  {
    if (count < self->size)
    {
      memset(self->data + count, 0, self->size - count);
      self->size = count;
    }
    else if (count > self->size)
    {
      cc_string_reserve(self, count);
      memset(self->data + self->size, ch, count - self->size);
      self->size = count;
    }
  }
}

void
cc_string_swap(struct cc_string* self, struct cc_string* other)
{
  if (self && other)
  {
    size_t size = self->size;
    size_t capacity = self->capacity;
    void* data = self->data;

    self->size = other->size;
    self->capacity = other->capacity;
    self->data = other->data;

    other->size = size;
    other->capacity = capacity;
    other->data = data;
  }
}

size_t
cc_string_find(const struct cc_string* self,
                const char* s,
                size_t pos,
                size_t count)
{
  if (self && self->data && self->size > pos && s && count > 0)
  {
    const char ch = s[0];
    const char* p = strchr(self->data + pos, ch);

    while (p)
    {
      if (strncmp(p, s, count) == 0)
      {
        return p - self->data;
      }
      p = strchr(p + 1, ch);
      if (p - self->data > self->size - count)
      {
        break;
      }
    }
  }

  return cc_npos;
}

size_t
cc_string_rfind(const struct cc_string* self,
                 const char* s,
                 size_t pos,
                 size_t count)
{
  if (self && self->data && s && count > 0 && self->size > pos && self->size > pos + count)
  {
    const char ch = s[0];
    const char* source = self->data + pos;
    size_t length = self->size - pos - count + 1;
    const char* p = _cc_strrchr(source, ch, length);

    while (p)
    {
      if (strncmp(p, s, count) == 0)
      {
        return p - self->data;
      }
      --length;
      p = _cc_strrchr(source, ch, length);
    }
  }

  return cc_npos;
}

size_t
cc_string_find_first_of(const struct cc_string* self,
                         const char* s,
                         size_t pos,
                         size_t count)
{
  if (self && self->data && self->size > pos && s && count > 0)
  {
    size_t m;
    for (size_t n = pos; n < self->size; ++n)
    {
      for (m = 0; m < count; ++m)
      {
        if (self->data[n] == s[m])
        {
          return n;
        }
      }
    }
  }
  return cc_npos;
}

size_t
cc_string_find_first_not_of(const struct cc_string* self,
                             const char* s,
                             size_t pos,
                             size_t count)
{
  if (self && self->data && self->size > pos && s && count > 0)
  {
    size_t m;
    for (size_t n = pos; n < self->size; ++n)
    {
      for (m = 0; m < count; ++m)
      {
        if (self->data[n] == s[m])
        {
          break;
        }
      }
      if (m == count)
      {
        return n;
      }
    }
  }
  return cc_npos;
}

size_t
cc_string_find_last_of(const struct cc_string* self,
                        const char* s,
                        size_t pos,
                        size_t count)
{
  if (self && self->data && s && count > 0)
  {
    size_t m;
    pos = pos < self->size ? pos : self->size;
    for (size_t n = 0; n < pos; ++n)
    {
      for (m = 0; m < count; ++m)
      {
        if (self->data[pos - n - 1] == s[m])
        {
          return pos - n - 1;
        }
      }
    }
  }
  return cc_npos;
}

size_t
cc_string_find_last_not_of(const struct cc_string* self,
                            const char* s,
                            size_t pos,
                            size_t count)
{
  if (self && self->data && s && count > 0)
  {
    size_t m;
    pos = pos < self->size ? pos : self->size;
    for (size_t n = 0; n < pos; ++n)
    {
      for (m = 0; m < count; ++m)
      {
        if (self->data[pos - n - 1] == s[m])
        {
          break;
        }
      }
      if (m == count)
      {
        return pos - n - 1;
      }
    }
  }
  return cc_npos;
}

void
cc_string_iterator_increment(struct cc_string_iterator* self)
{
  ++self->data;
}

void
cc_string_iterator_decrement(struct cc_string_iterator* self)
{
  --self->data;
}

void
cc_string_iterator_advance(struct cc_string_iterator* self, ptrdiff_t n)
{
  self->data = self->data + n;
}

ptrdiff_t
cc_string_iterator_distance_to(const struct cc_string_iterator self,
                               const struct cc_string_iterator other)
{
  return (other.data - self.data);
}

char*
cc_string_iterator_dereference(const struct cc_string_iterator self)
{
  return self.data;
}

bool
cc_string_iterator_eq(const struct cc_string_iterator self,
                      const struct cc_string_iterator other)
{
  return self.data == other.data;
}

bool
cc_string_iterator_ne(const struct cc_string_iterator self,
                      const struct cc_string_iterator other)
{
  return self.data != other.data;
}
