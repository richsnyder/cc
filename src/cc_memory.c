/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include <stdlib.h>
#include <string.h>
#include "cc_memory.h"
#include "xxhash/xxhash.h"

const struct cc_functions cc_default_functions = (struct cc_functions){
  .hasher = cc_default_hasher,
  .copier = cc_default_copier,
  .deleter = cc_default_deleter,
  .equality = cc_default_equality
};

uint64_t
cc_default_hasher(const void* buffer, size_t size)
{
  return (uint64_t) XXH64(buffer, size, 0);
}

void*
cc_default_copier(void* dest, const void* src, size_t size)
{
  return memcpy(dest, src, size);
}

void
cc_default_deleter(void* ptr)
{
  // empty
}

bool
cc_default_equality(const void* left, const void* right, size_t size)
{
  return memcmp(left, right, size) == 0;
}

void
cc_hash_combine(uint64_t* seed, uint64_t value)
{
  // C++ Boost hash combine function
  *seed ^= value + 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
}
