/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 *
 *  DISTRIBUTION STATEMENT A.  Approved for public release; distribution is
 *  unlimited.  Granted clearance per 88ABW-2020-3430.
 */

#include "doctest/doctest.h"
#include "cc.h"
#include "iarray.hpp"
#include "list.hpp"

cc_list_t
iarray_list_new()
{
  return cc_list_new_f(sizeof(struct iarray), iarray_functions);
}

cc_list_t
iarray_list_from_array(const std::vector<iarray>& x)
{
  return cc_list_from_array_f(
      x.data(),
      x.size(),
      sizeof(iarray),
      iarray_functions
    );
}

TEST_SUITE_BEGIN("lists");

TEST_CASE("list construction [deep]")
{
  SUBCASE("empty list")
  {
    cc_list_t u = cc_list_new(sizeof(iarray));
    check_list<iarray>(u, { });
    cc_list_delete(u);
  }

  SUBCASE("create from an array")
  {
    int x1[1] = { 111 };
    int x2[2] = { 222, 333 };
    int x3[3] = { 444, 555, 666 };
    std::vector<iarray> x = { {1, x1}, {2, x2}, {3, x3} };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(iarray));
    check_list(u, x);
    cc_list_delete(u);
  }

  SUBCASE("copy")
  {
    int x1[1] = { 111 };
    int x2[2] = { 222, 333 };
    int x3[3] = { 444, 555, 666 };
    std::vector<iarray> x = { {1, x1}, {2, x2}, {3, x3} };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(iarray));
    cc_list_t v = cc_list_copy(u);
    check_list(v, x);
    cc_list_delete(u);
    cc_list_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_list_t u = cc_list_new(sizeof(iarray));
    cc_list_t v = cc_list_copy(u);
    check_list<iarray>(v, { });
    cc_list_delete(u);
    cc_list_delete(v);
  }
}

TEST_CASE("list assignment [deep]")
{
  int a1[6] = { 1, 4, 9, 16, 25, 36 };
  iarray a = { 6, a1 };
  std::vector<iarray> x = { {6, a1}, {6, a1}, {6, a1}, {6, a1} };
  cc_list_t u = cc_list_new(sizeof(iarray));
  cc_list_assign(u, 4, &a);
  check_list(u, x);
  cc_list_delete(u);
}

TEST_CASE("list element access [deep]")
{
  int x1[2] = {2, 3};
  int x2[3] = {5, 7, 11};
  int x3[6] = {13, 17, 19, 23, 29, 31};
  std::vector<iarray> x = { {2, x1}, {3, x2}, {6, x3} };
  cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(iarray));
  auto eq = std::equal_to<iarray>();

  SUBCASE("front")
  {
    CHECK(eq(*(const iarray*) cc_list_front(u), x[0]));
  }

  SUBCASE("back")
  {
    CHECK(eq(*(const iarray*) cc_list_back(u), x[2]));
  }

  cc_list_delete(u);
}

TEST_CASE("list iteration [deep]")
{
  int x1[4] = { 1, 3, 5, 7 };
  int x2[3] = { 9, 11, 13 };
  int x3[2] = { 15, 17 };
  int x4[1] = { 19 };
  int x5[0] = { };
  std::vector<iarray> x = { {4, x1}, {3, x2}, {2, x3}, {1, x4}, {0, x5} };
  cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(iarray));
  auto eq = std::equal_to<iarray>();

  cc_list_iterator_t p = cc_list_begin(u);
  cc_list_iterator_t e = cc_list_end(u);
  for (size_t n = 0; n < x.size(); ++n)
  {
    REQUIRE(cc_list_iterator_ne(p, e));
    CHECK(eq(*(const iarray*) cc_list_iterator_dereference(p), x[n]));
    cc_list_iterator_increment(&p);
  }
  REQUIRE(cc_list_iterator_eq(p, e));

  cc_list_delete(u);
}

TEST_CASE("list capacity [deep]")
{
  int a1[3] = { -1, 0, 1 };
  iarray a = {3, a1};
  int x1[2] = { 1, 2 };
  int x2[2] = { 3, 4 };
  int x3[2] = { 5, 6 };
  std::vector<iarray> x = { {2, x1}, {2, x2}, {2, x3} };
  cc_list_t u = cc_list_new(sizeof(iarray));
  cc_list_t v = cc_list_from_array(x.data(), x.size(), sizeof(iarray));

  SUBCASE("empty")
  {
    CHECK(cc_list_empty(u));
    CHECK(!cc_list_empty(v));
  }

  SUBCASE("size")
  {
    CHECK(cc_list_size(u) == 0);
    CHECK(cc_list_size(v) == 3);
  }

  cc_list_delete(u);
  cc_list_delete(v);
}

TEST_CASE("list modification [deep]")
{
  int a1[1] = {-1};
  iarray a = {1, a1};
  int x1[2] = {1, 1};
  int x2[2] = {2, 3};
  int x3[2] = {5, 8};
  std::vector<iarray> x = { {2, x1}, {2, x2}, {2, x3} };
  cc_list_t u = cc_list_new(sizeof(iarray));
  cc_list_t v = cc_list_from_array(x.data(), x.size(), sizeof(iarray));

  SUBCASE("clear")
  {
    cc_list_clear(u);
    cc_list_clear(v);
    CHECK(cc_list_empty(u));
    CHECK(cc_list_empty(v));
  }

  SUBCASE("insert")
  {
    cc_list_iterator_t p;

    p = cc_list_begin(u);
    cc_list_insert(u, p, &a);
    p = cc_list_begin(v);
    cc_list_iterator_increment(&p);
    cc_list_insert(v, p, &a);
    check_list<iarray>(u, { {1, a1} });
    check_list<iarray>(v, { {2, x1}, {1, a1}, {2, x2}, {2, x3} });
  }

  SUBCASE("erase")
  {
    cc_list_iterator_t first = cc_list_begin(v);
    cc_list_iterator_t last = cc_list_end(v);
    cc_list_iterator_increment(&first);
    cc_list_erase(v, first, last);
    check_list<iarray>(v, { {2, x1} });
  }

  SUBCASE("push back")
  {
    cc_list_push_back(u, &a);
    cc_list_push_back(v, &a);
    check_list<iarray>(u, { {1, a1} });
    check_list<iarray>(v, { {2, x1}, {2, x2}, {2, x3}, {1, a1} });
  }

  SUBCASE("pop back")
  {
    cc_list_pop_back(v);
    check_list<iarray>(v, { {2, x1}, {2, x2} });
  }

  SUBCASE("push front")
  {
    cc_list_push_front(u, &a);
    cc_list_push_front(v, &a);
    check_list<iarray>(u, { {1, a1} });
    check_list<iarray>(v, { {1, a1}, {2, x1}, {2, x2}, {2, x3} });
  }

  SUBCASE("pop front")
  {
    cc_list_pop_front(v);
    check_list<iarray>(v, { {2, x2}, {2, x3} });
  }

  SUBCASE("resize")
  {
    cc_list_resize(u, 2, &a);
    cc_list_resize(v, 2, &a);
    check_list<iarray>(u, { {1, a1}, {1, a1} });
    check_list<iarray>(v, { {2, x1}, {2, x2} });
  }

  SUBCASE("swap")
  {
    cc_list_swap(u, v);
    check_list<iarray>(u, x);
    check_list<iarray>(v, { });
  }

  cc_list_delete(u);
  cc_list_delete(v);
}

TEST_CASE("list comparison [deep]")
{
  int x1[2] = {1, 2};
  int x2[4] = {3, 4, 5, 6};
  int x3[1] = {0};
  int x4[4] = {3, 5, 4, 6};
  std::vector<iarray> x = { {2, x1}, {4, x2} };
  std::vector<iarray> y = { {2, x1}, {4, x2}, {1, x3} };
  std::vector<iarray> z = { {2, x1}, {4, x4}, {1, x3} };

  cc_list_t a = cc_list_from_array(x.data(), x.size(), sizeof(iarray));
  cc_list_t b = cc_list_from_array(x.data(), x.size(), sizeof(iarray));
  cc_list_t c = cc_list_from_array(y.data(), y.size(), sizeof(iarray));
  cc_list_t d = cc_list_from_array(z.data(), z.size(), sizeof(iarray));

  SUBCASE("equality")
  {
    CHECK(cc_list_eq(a, b));
    CHECK(cc_list_eq(b, a));
    CHECK(!cc_list_eq(a, c));
    CHECK(!cc_list_eq(c, a));
    CHECK(!cc_list_eq(b, c));
    CHECK(!cc_list_eq(c, b));
  }

  SUBCASE("inequality")
  {
    CHECK(!cc_list_ne(a, b));
    CHECK(!cc_list_ne(b, a));
    CHECK(cc_list_ne(a, c));
    CHECK(cc_list_ne(c, a));
    CHECK(cc_list_ne(b, c));
    CHECK(cc_list_ne(c, b));
  }

  cc_list_delete(a);
  cc_list_delete(b);
  cc_list_delete(c);
  cc_list_delete(d);
}

TEST_SUITE_END();
