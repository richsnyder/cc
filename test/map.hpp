/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#ifndef CC_TEST_MAP_HPP
#define CC_TEST_MAP_HPP

#include <functional>
#include <map>
#include <vector>
#include "cc_map.h"

template <class T, class U>
cc_map_t
create_map(const std::map<T, U>& map)
{
  std::vector<T> keys;
  std::vector<U> values;
  keys.reserve(map.size());
  values.reserve(map.size());
  for (const auto& kv : map)
  {
    keys.push_back(kv.first);
    values.push_back(kv.second);
  }
  return cc_map_from_arrays(
      keys.data(),
      values.data(),
      map.size(),
      sizeof(T),
      sizeof(U)
    );
}

template <class T, class U>
void
check_map(cc_map_t actual, const std::map<T, U>& expected)
{
  const U* x;
  auto eq = std::equal_to<U>();
  REQUIRE(cc_map_size(actual) == expected.size());
  for (const auto& kv : expected)
  {
    x = static_cast<const U*>(cc_map_find(actual, &kv.first));
    REQUIRE(x);
    CHECK(eq(*x, kv.second));
  }
}

#endif // CC_TEST_MAP_HPP
