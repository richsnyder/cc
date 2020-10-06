/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
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
