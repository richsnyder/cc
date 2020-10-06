/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#ifndef CC_TEST_LIST_HPP
#define CC_TEST_LIST_HPP

#include <functional>
#include <vector>
#include "cc_list.h"

template <class T>
void
check_list(cc_list_t actual, const std::vector<T>& expected)
{
  const T* x;
  auto eq = std::equal_to<T>();
  REQUIRE(cc_list_size(actual) == expected.size());
  const struct cc_list_node* node = actual->front;
  for (size_t n = 0; n < expected.size(); ++n)
  {
    x = static_cast<const T*>(node->data);
    CHECK(eq(*x, expected[n]));
    node = node->next;
  }
}

#endif // CC_TEST_LIST_HPP
