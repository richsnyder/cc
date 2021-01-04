/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#ifndef CC_TEST_POINT_HPP
#define CC_TEST_POINT_HPP

#include <stdbool.h>
#include <functional>

struct point
{
  double x;
  double y;
};

bool
operator<(const struct point& left, const struct point& right);

namespace std {

template <>
struct equal_to<struct point>
{
  bool
  operator()(const struct point& left, const struct point& right) const
  {
    return left.x == right.x && left.y == right.y;
  }
};

}

#endif // CC_TEST_POINT_HPP
