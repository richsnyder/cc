/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include <stdlib.h>
#include <string.h>
#include "iarray.hpp"

const struct cc_functions iarray_functions = (struct cc_functions){
  .hasher = iarray_hash,
  .copier = iarray_copy,
  .deleter = iarray_delete,
  .equality = iarray_equal
};

uint64_t
iarray_hash(const void* buffer, size_t size)
{
  uint64_t hash = 0;
  size_t length = ((iarray*) buffer)->size;
  int* data = ((iarray*) buffer)->data;
  for (size_t n = 0; n < length; ++n)
  {
    cc_hash_combine(&hash, cc_default_hasher(data + n, sizeof(int)));
  }
  return hash;
}

void*
iarray_copy(void* dest, const void* src, size_t size)
{
  iarray* d = (iarray*) dest;
  const iarray* s = (const iarray*) src;

  size_t count = s->size * sizeof(int);
  void* data = malloc(count);
  if (!data)
  {
    return NULL;
  }
  memcpy(data, s->data, count);

  d->size = s->size;
  d->data = (int*) data;

  return dest;
}

void
iarray_delete(void* ptr)
{
  iarray* a = (iarray*) ptr;
  if (a->data)
  {
    free(a->data);
    a->size = 0;
    a->data = NULL;
  }
}

bool
iarray_equal(const void* left, const void* right, size_t size)
{
  const iarray* left_array = (const iarray*) left;
  const iarray* right_array = (const iarray*) right;
  int* l = left_array->data;
  int* r = right_array->data;
  int n = left_array->size;

  if (n != right_array->size)
  {
    return false;
  }
  for (size_t i = 0; i < n; ++i, ++l, ++r)
  {
    if (*l != *r)
    {
      return false;
    }
  }
  return true;
}

bool
operator<(const struct iarray& left, const struct iarray& right)
{
  if (left.size < right.size)
  {
    return true;
  }
  else if (left.size > right.size)
  {
    return false;
  }
  else
  {
    for (size_t n = 0; n < left.size; ++n)
    {
      if (left.data[n] < right.data[n])
      {
        return true;
      }
      else if (left.data[n] > right.data[n])
      {
        return false;
      }
    }
    return false;
  }
}
