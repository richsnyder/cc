/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include "doctest/doctest.h"
#include "cc.h"
#include "iarray.hpp"
#include "vector.hpp"

cc_vector_t
iarray_vector_new()
{
  return cc_vector_new_f(sizeof(struct iarray), iarray_functions);
}

cc_vector_t
iarray_vector_from_array(const std::vector<iarray>& x)
{
  return cc_vector_from_array_f(
      x.data(),
      x.size(),
      sizeof(iarray),
      iarray_functions
    );
}

TEST_SUITE_BEGIN("vectors");

TEST_CASE("vector construction [deep]")
{
  SUBCASE("empty vector")
  {
    cc_vector_t u = iarray_vector_new();
    check_vector<iarray>(u, { });
    cc_vector_delete(u);
  }

  SUBCASE("create from an array")
  {
    int x1[1] = { 111 };
    int x2[2] = { 222, 333 };
    int x3[3] = { 444, 555, 666 };
    std::vector<iarray> x = { {1, x1}, {2, x2}, {3, x3} };
    cc_vector_t u = iarray_vector_from_array(x);
    check_vector(u, x);
    cc_vector_delete(u);
  }

  SUBCASE("copy")
  {
    int x1[1] = { 111 };
    int x2[2] = { 222, 333 };
    int x3[3] = { 444, 555, 666 };
    std::vector<iarray> x = { {1, x1}, {2, x2}, {3, x3} };
    cc_vector_t u = iarray_vector_from_array(x);
    cc_vector_t v = cc_vector_copy(u);
    check_vector(v, x);
    cc_vector_delete(u);
    cc_vector_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_vector_t u = cc_vector_new(sizeof(iarray));
    cc_vector_t v = cc_vector_copy(u);
    check_vector<iarray>(v, { });
    cc_vector_delete(u);
    cc_vector_delete(v);
  }
}

TEST_CASE("vector assignment [deep]")
{
  int a1[6] = { 1, 4, 9, 16, 25, 36 };
  iarray a = { 6, a1 };
  std::vector<iarray> x = { {6, a1}, {6, a1}, {6, a1}, {6, a1} };
  cc_vector_t u = cc_vector_new(sizeof(iarray));
  cc_vector_assign(u, 4, &a);
  check_vector(u, x);
  cc_vector_delete(u);
}

TEST_CASE("vector element access [deep]")
{
  int x1[2] = {2, 3};
  int x2[3] = {5, 7, 11};
  int x3[6] = {13, 17, 19, 23, 29, 31};
  std::vector<iarray> x = { {2, x1}, {3, x2}, {6, x3} };
  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(iarray));
  auto eq = std::equal_to<iarray>();

  SUBCASE("get")
  {
    check_vector(u, x);
  }

  SUBCASE("front")
  {
    CHECK(eq(*(const iarray*) cc_vector_front(u), x[0]));
  }

  SUBCASE("back")
  {
    CHECK(eq(*(const iarray*) cc_vector_back(u), x[2]));
  }

  SUBCASE("data")
  {
    iarray* data = (iarray*) cc_vector_data(u);
    CHECK(data[0].size == 2);
    CHECK(data[1].size == 3);
    CHECK(data[2].size == 6);
    CHECK(data[0].data[0] == 2);
    CHECK(data[0].data[1] == 3);
    CHECK(data[1].data[0] == 5);
    CHECK(data[1].data[1] == 7);
    CHECK(data[1].data[2] == 11);
    CHECK(data[2].data[0] == 13);
    CHECK(data[2].data[1] == 17);
    CHECK(data[2].data[2] == 19);
    CHECK(data[2].data[3] == 23);
    CHECK(data[2].data[4] == 29);
    CHECK(data[2].data[5] == 31);
  }

  cc_vector_delete(u);
}

TEST_CASE("vector iteration [deep]")
{
  int x1[4] = { 1, 3, 5, 7 };
  int x2[3] = { 9, 11, 13 };
  int x3[2] = { 15, 17 };
  int x4[1] = { 19 };
  int x5[0] = { };
  std::vector<iarray> x = { {4, x1}, {3, x2}, {2, x3}, {1, x4}, {0, x5} };
  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(iarray));
  auto eq = std::equal_to<iarray>();

  cc_vector_iterator_t p = cc_vector_begin(u);
  cc_vector_iterator_t e = cc_vector_end(u);
  for (size_t n = 0; n < x.size(); ++n)
  {
    REQUIRE(cc_vector_iterator_ne(p, e));
    CHECK(eq(*(const iarray*) cc_vector_iterator_dereference(p), x[n]));
    cc_vector_iterator_increment(&p);
  }
  REQUIRE(cc_vector_iterator_eq(p, e));

  cc_vector_delete(u);
}

TEST_CASE("vector capacity [deep]")
{
  int a1[3] = { -1, 0, 1 };
  iarray a = {3, a1};
  int x1[2] = { 1, 2 };
  int x2[2] = { 3, 4 };
  int x3[2] = { 5, 6 };
  std::vector<iarray> x = { {2, x1}, {2, x2}, {2, x3} };
  cc_vector_t u = cc_vector_new(sizeof(iarray));
  cc_vector_t v = cc_vector_from_array(x.data(), x.size(), sizeof(iarray));

  SUBCASE("empty")
  {
    CHECK(cc_vector_empty(u));
    CHECK(!cc_vector_empty(v));
  }

  SUBCASE("size")
  {
    CHECK(cc_vector_size(u) == 0);
    CHECK(cc_vector_size(v) == 3);
  }

  SUBCASE("capacity")
  {
    CHECK(cc_vector_capacity(u) == 0);
    CHECK(cc_vector_capacity(v) == 3);

    cc_vector_reserve(u, 3);
    CHECK(cc_vector_capacity(u) == 3);

    cc_vector_push_back(u, &a);
    CHECK(cc_vector_capacity(u) == 3);

    cc_vector_push_back(v, &a);
    CHECK(cc_vector_capacity(v) == 6);

    cc_vector_shrink_to_fit(v);
    CHECK(cc_vector_capacity(v) == 4);
  }

  cc_vector_delete(u);
  cc_vector_delete(v);
}

TEST_CASE("vector modification [deep]")
{
  int a1[1] = {-1};
  iarray a = {1, a1};
  int x1[2] = {1, 1};
  int x2[2] = {2, 3};
  int x3[2] = {5, 8};
  std::vector<iarray> x = { {2, x1}, {2, x2}, {2, x3} };
  cc_vector_t u = cc_vector_new(sizeof(iarray));
  cc_vector_t v = cc_vector_from_array(x.data(), x.size(), sizeof(iarray));

  SUBCASE("clear")
  {
    cc_vector_clear(u);
    cc_vector_clear(v);
    CHECK(cc_vector_empty(u));
    CHECK(cc_vector_empty(v));
  }

  SUBCASE("insert")
  {
    cc_vector_insert(u, 0, &a);
    cc_vector_insert(v, 1, &a);
    check_vector<iarray>(u, { {1, a1} });
    check_vector<iarray>(v, { {2, x1}, {1, a1}, {2, x2}, {2, x3} });
  }

  SUBCASE("erase")
  {
    cc_vector_erase(v, 1, 10);
    check_vector<iarray>(v, { {2, x1} });
  }

  SUBCASE("push back")
  {
    cc_vector_push_back(u, &a);
    cc_vector_push_back(v, &a);
    check_vector<iarray>(u, { {1, a1} });
    check_vector<iarray>(v, { {2, x1}, {2, x2}, {2, x3}, {1, a1} });
  }

  SUBCASE("pop back")
  {
    cc_vector_pop_back(v);
    check_vector<iarray>(v, { {2, x1}, {2, x2} });
  }

  SUBCASE("resize")
  {
    cc_vector_resize(u, 2, &a);
    cc_vector_resize(v, 2, &a);
    check_vector<iarray>(u, { {1, a1}, {1, a1} });
    check_vector<iarray>(v, { {2, x1}, {2, x2} });
  }

  SUBCASE("swap")
  {
    cc_vector_swap(u, v);
    check_vector<iarray>(u, x);
    check_vector<iarray>(v, { });
  }

  cc_vector_delete(u);
  cc_vector_delete(v);
}

TEST_CASE("vector comparison [struct]")
{
  int x1[2] = {1, 2};
  int x2[4] = {3, 4, 5, 6};
  int x3[1] = {0};
  int x4[4] = {3, 5, 4, 6};
  std::vector<iarray> x = { {2, x1}, {4, x2} };
  std::vector<iarray> y = { {2, x1}, {4, x2}, {1, x3} };
  std::vector<iarray> z = { {2, x1}, {4, x4}, {1, x3} };

  cc_vector_t a = cc_vector_from_array(x.data(), x.size(), sizeof(iarray));
  cc_vector_t b = cc_vector_from_array(x.data(), x.size(), sizeof(iarray));
  cc_vector_t c = cc_vector_from_array(y.data(), y.size(), sizeof(iarray));
  cc_vector_t d = cc_vector_from_array(z.data(), z.size(), sizeof(iarray));
  cc_vector_reserve(b, x.size() + 2);

  SUBCASE("equality")
  {
    CHECK(cc_vector_eq(a, b));
    CHECK(cc_vector_eq(b, a));
    CHECK(!cc_vector_eq(a, c));
    CHECK(!cc_vector_eq(c, a));
    CHECK(!cc_vector_eq(b, c));
    CHECK(!cc_vector_eq(c, b));
  }

  SUBCASE("inequality")
  {
    CHECK(!cc_vector_ne(a, b));
    CHECK(!cc_vector_ne(b, a));
    CHECK(cc_vector_ne(a, c));
    CHECK(cc_vector_ne(c, a));
    CHECK(cc_vector_ne(b, c));
    CHECK(cc_vector_ne(c, b));
  }

  cc_vector_delete(a);
  cc_vector_delete(b);
  cc_vector_delete(c);
  cc_vector_delete(d);
}

TEST_SUITE_END();
