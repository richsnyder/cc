/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#include "point.hpp"

bool
operator<(const struct point& left, const struct point& right)
{
  if (left.x < right.x)
  {
    return true;
  }
  else if (left.x > right.x)
  {
    return false;
  }
  else
  {
    return left.y < right.y;
  }
}
