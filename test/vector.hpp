/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#ifndef CC_TEST_VECTOR_HPP
#define CC_TEST_VECTOR_HPP

#include <functional>
#include <vector>
#include "cc_vector.h"

template <class T>
void
check_vector(cc_vector_t actual, const std::vector<T>& expected)
{
  const T* x;
  auto eq = std::equal_to<T>();
  REQUIRE(cc_vector_size(actual) == expected.size());
  for (size_t n = 0; n < expected.size(); ++n)
  {
    x = static_cast<const T*>(cc_vector_get(actual, n));
    CHECK(eq(*x, expected[n]));
  }
}

#endif // CC_TEST_VECTOR_HPP
