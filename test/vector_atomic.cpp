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
#include "vector.hpp"

TEST_SUITE_BEGIN("vectors");

TEST_CASE("vector construction [atomic]")
{
  SUBCASE("empty vector")
  {
    cc_vector_t u = cc_vector_new(sizeof(int));
    check_vector<int>(u, { });
    cc_vector_delete(u);
  }

  SUBCASE("create from an array")
  {
    std::vector<int> x = { 1, 4, 9, 16, 25 };
    cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(int));
    check_vector(u, x);
    cc_vector_delete(u);
  }

  SUBCASE("copy")
  {
    std::vector<double> x = { 1.2, 3.4, 5.6 };
    cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(double));
    cc_vector_t v = cc_vector_copy(u);
    check_vector(v, x);
    cc_vector_delete(u);
    cc_vector_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_vector_t u = cc_vector_new(sizeof(int));
    cc_vector_t v = cc_vector_copy(u);
    check_vector<int>(v, { });
    cc_vector_delete(u);
    cc_vector_delete(v);
  }
}

TEST_CASE("vector assignment [atomic]")
{
  int a = 123;
  std::vector<int> x = { 123, 123, 123, 123 };
  cc_vector_t u = cc_vector_new(sizeof(int));
  cc_vector_assign(u, 4, &a);
  check_vector(u, x);
  cc_vector_delete(u);
}

TEST_CASE("vector element access [atomic]")
{
  std::vector<int> x = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };
  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(int));

  SUBCASE("get")
  {
    check_vector(u, x);
  }

  SUBCASE("front")
  {
    CHECK(*(int*) cc_vector_front(u) == 2);
  }

  SUBCASE("back")
  {
    CHECK(*(int*) cc_vector_back(u) == 29);
  }

  SUBCASE("data")
  {
    int* data = (int*) cc_vector_data(u);
    CHECK(data[2] == 5);
    CHECK(data[5] == 13);
    CHECK(data[8] == 23);
  }

  cc_vector_delete(u);
}

TEST_CASE("vector iteration [atomic]")
{
  std::vector<int> x = { 1, 22, 333, 4444 };
  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(int));

  cc_vector_iterator_t p = cc_vector_begin(u);
  cc_vector_iterator_t e = cc_vector_end(u);
  for (size_t n = 0; n < x.size(); ++n)
  {
    REQUIRE(cc_vector_iterator_ne(p, e));
    CHECK(*(int*) cc_vector_iterator_dereference(p) == x[n]);
    cc_vector_iterator_increment(&p);
  }
  REQUIRE(cc_vector_iterator_eq(p, e));

  cc_vector_delete(u);
}

TEST_CASE("vector capacity [atomic]")
{
  double a = -6.6;
  std::vector<double> x = { 1.1, -2.2, 3.3, -4.4, 5.5 };
  cc_vector_t u = cc_vector_new(sizeof(double));
  cc_vector_t v = cc_vector_from_array(x.data(), x.size(), sizeof(double));

  SUBCASE("empty")
  {
    CHECK(cc_vector_empty(u));
    CHECK(!cc_vector_empty(v));
  }

  SUBCASE("size")
  {
    CHECK(cc_vector_size(u) == 0);
    CHECK(cc_vector_size(v) == 5);
  }

  SUBCASE("capacity")
  {
    CHECK(cc_vector_capacity(u) == 0);
    CHECK(cc_vector_capacity(v) == 5);

    cc_vector_reserve(u, 3);
    CHECK(cc_vector_capacity(u) == 3);

    cc_vector_push_back(u, &a);
    CHECK(cc_vector_capacity(u) == 3);

    cc_vector_push_back(v, &a);
    CHECK(cc_vector_capacity(v) == 10);

    cc_vector_shrink_to_fit(v);
    CHECK(cc_vector_capacity(v) == 6);
  }

  cc_vector_delete(u);
  cc_vector_delete(v);
}

TEST_CASE("vector modification [atomic]")
{
  double a = -6.6;
  std::vector<double> x = { 1.1, -2.2, 3.3, -4.4, 5.5 };
  cc_vector_t u = cc_vector_new(sizeof(double));
  cc_vector_t v = cc_vector_from_array(x.data(), x.size(), sizeof(double));

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
    check_vector<double>(u, { -6.6 });
    check_vector<double>(v, { 1.1, -6.6, -2.2, 3.3, -4.4, 5.5 });
  }

  SUBCASE("erase")
  {
    cc_vector_erase(v, 1, 3);
    check_vector<double>(v, { 1.1, -4.4, 5.5 });
  }

  SUBCASE("push back")
  {
    cc_vector_push_back(u, &a);
    cc_vector_push_back(v, &a);
    check_vector<double>(u, { -6.6 });
    check_vector<double>(v, { 1.1, -2.2, 3.3, -4.4, 5.5, -6.6 });
  }

  SUBCASE("pop back")
  {
    cc_vector_pop_back(v);
    CHECK(cc_vector_size(v) == 4);
    CHECK(*(double*) cc_vector_back(v) == doctest::Approx(-4.4));
  }

  SUBCASE("resize")
  {
    cc_vector_resize(u, 3, &a);
    cc_vector_resize(v, 3, &a);
    check_vector<double>(u, { -6.6, -6.6, -6.6 });
    check_vector<double>(v, { 1.1, -2.2, 3.3 });
  }

  SUBCASE("swap")
  {
    cc_vector_swap(u, v);
    check_vector<double>(u, { 1.1, -2.2, 3.3, -4.4, 5.5 });
    check_vector<double>(v, { });
  }

  cc_vector_delete(u);
  cc_vector_delete(v);
}

TEST_CASE("vector comparison [atomic]")
{
  std::vector<long> x = { 1, 12, 123, 1234, 12345, 123456, 1234567 };
  std::vector<long> y = { 1, 12, 123, 1234, 12345, 123456, 1234567, 12345678 };
  std::vector<long> z = { 1, 12, 123, 1234, 12345, 123458, 1234567, 12345678 };

  cc_vector_t a = cc_vector_from_array(x.data(), x.size(), sizeof(long));
  cc_vector_t b = cc_vector_from_array(x.data(), x.size(), sizeof(long));
  cc_vector_t c = cc_vector_from_array(y.data(), y.size(), sizeof(long));
  cc_vector_t d = cc_vector_from_array(z.data(), z.size(), sizeof(long));
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
