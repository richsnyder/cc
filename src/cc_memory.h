/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#ifndef CC_MEMORY_H
#define CC_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

typedef uint64_t (*cc_hash_fn)(const void* buffer, size_t size);

typedef void* (*cc_copy_fn)(void* dest, const void* src, size_t size);

typedef void (*cc_delete_fn)(void* ptr);

typedef bool (*cc_equal_fn)(const void* left, const void* right, size_t size);

struct cc_functions
{
  cc_hash_fn hasher;
  cc_copy_fn copier;
  cc_delete_fn deleter;
  cc_equal_fn equality;
};

extern const struct cc_functions cc_default_functions;

uint64_t
cc_default_hasher(const void* buffer, size_t size);

void*
cc_default_copier(void* dest, const void* src, size_t size);

void
cc_default_deleter(void* ptr);

bool
cc_default_equality(const void* left, const void* right, size_t size);

void
cc_hash_combine(uint64_t* seed, uint64_t value);

#if defined(__cplusplus)
}
#endif

#endif // CC_MEMORY_H
