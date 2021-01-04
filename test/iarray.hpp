/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#ifndef CC_TEST_IARRAY_HPP
#define CC_TEST_IARRAY_HPP

#include <stddef.h>
#include <functional>
#include "cc.h"

struct iarray
{
  size_t size;
  int* data;
};

extern const struct cc_functions iarray_functions;

uint64_t
iarray_hash(const void* buffer, size_t size);

void*
iarray_copy(void* dest, const void* src, size_t size);

void
iarray_delete(void* ptr);

bool
iarray_equal(const void* left, const void* right, size_t size);

bool
operator<(const struct iarray& left, const struct iarray& right);

namespace std {

template <>
struct equal_to<struct iarray>
{
  bool
  operator()(const struct iarray& left, const struct iarray& right) const
  {
    return iarray_equal(&left, &right, sizeof(struct iarray));
  }
};

}

#endif // CC_TEST_IARRAY_HPP
