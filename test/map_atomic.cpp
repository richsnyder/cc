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
#include "map.hpp"

#include <iostream>

TEST_SUITE_BEGIN("maps");

TEST_CASE("map construction [atomic]")
{
  SUBCASE("empty list")
  {
    cc_map_t u = cc_map_new(sizeof(int), sizeof(int));
    CHECK(cc_map_size(u) == 0);
    cc_map_delete(u);
  }

  SUBCASE("create from arrays")
  {
    std::vector<int> ks = { 1, 2, 3, 4 };
    std::vector<double> vs = { 1.1, 2.2, 3.3, 4.4 };
    std::map<int, double> x = { {1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4} };
    cc_map_t u = cc_map_from_arrays(
        ks.data(),
        vs.data(),
        ks.size(),
        sizeof(int),
        sizeof(double)
      );
    cc_map_t v = create_map(x);
    check_map(u, x);
    check_map(v, x);
    cc_map_delete(u);
    cc_map_delete(v);
  }

  SUBCASE("copy")
  {
    std::map<int, double> x = { {1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4} };
    cc_map_t u = create_map(x);
    cc_map_t v = cc_map_copy(u);
    check_map(v, x);
    cc_map_delete(u);
    cc_map_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_map_t u = cc_map_new(sizeof(int), sizeof(int));
    cc_map_t v = cc_map_copy(u);
    check_map<int, int>(v, { });
    cc_map_delete(u);
    cc_map_delete(v);
  }
}

TEST_CASE("map iteration [atomic]")
{
  std::map<int, double> x = { {1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4} };
  std::map<int, double>::const_iterator q;
  cc_map_t u = create_map(x);
  cc_map_key_value_t kv;

  SUBCASE("incrementing")
  {
    cc_map_iterator_t p = cc_map_begin(u);
    cc_map_iterator_t e = cc_map_end(u);

    for (size_t n = 0; n < x.size(); ++n)
    {
      REQUIRE(cc_map_iterator_ne(p, e));
      kv = cc_map_iterator_dereference(p);
      REQUIRE(kv.key);
      REQUIRE(kv.value);
      q = x.find(*(int*) kv.key);
      REQUIRE(q != x.end());
      CHECK(*(double*) kv.value == doctest::Approx(q->second));
      cc_map_iterator_increment(&p);
    }
    REQUIRE(cc_map_iterator_eq(p, e));
  }

  SUBCASE("decrementing")
  {
    cc_map_iterator_t b = cc_map_begin(u);
    cc_map_iterator_t p = cc_map_end(u);

    for (size_t n = 0; n < x.size(); ++n)
    {
      REQUIRE(cc_map_iterator_ne(p, b));
      cc_map_iterator_decrement(&p);
      kv = cc_map_iterator_dereference(p);
      REQUIRE(kv.key);
      REQUIRE(kv.value);
      q = x.find(*(int*) kv.key);
      REQUIRE(q != x.end());
      CHECK(*(double*) kv.value == doctest::Approx(q->second));
    }
    REQUIRE(cc_map_iterator_eq(p, b));
  }

  cc_map_delete(u);
}

TEST_CASE("map capacity [atomic]")
{
  int key = 99;
  int value = 198;
  std::map<int, int> x;
  for (auto n = 0; n < 12; ++n) x[n] = 2 * n;

  cc_map_t u = cc_map_new(sizeof(int), sizeof(int));
  cc_map_t v = create_map(x);

  SUBCASE("empty")
  {
    CHECK(cc_map_empty(u));
    CHECK(!cc_map_empty(v));
  }

  SUBCASE("size")
  {
    CHECK(cc_map_size(u) == 0);
    CHECK(cc_map_size(v) == 12);
  }

  SUBCASE("capacity")
  {
    CHECK(cc_map_capacity(u) == 16);
    CHECK(cc_map_capacity(v) == 16);

    cc_map_reserve(u, 20);
    CHECK(cc_map_capacity(u) == 32);

    cc_map_insert(u, &key, &value);
    CHECK(cc_map_capacity(u) == 32);

    cc_map_insert(v, &key, &value);
    CHECK(cc_map_capacity(v) == 32);
  }

  cc_map_delete(u);
  cc_map_delete(v);
}

TEST_CASE("map modification [atomic]")
{
  std::pair<int, double> a = {5, 5.5};
  std::map<int, double> x = { {1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4} };
  cc_map_t u = cc_map_new(sizeof(int), sizeof(double));
  cc_map_t v = create_map(x);

  SUBCASE("clear")
  {
    cc_map_clear(u);
    cc_map_clear(v);
    CHECK(cc_map_empty(u));
    CHECK(cc_map_empty(v));
  }

  SUBCASE("insert")
  {
    cc_map_insert(u, &a.first, &a.second);
    cc_map_insert(v, &a.first, &a.second);
    check_map<int, double>(u, { {5, 5.5} });
    check_map<int, double>(v, { {1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4}, {5, 5.5} });
  }

  SUBCASE("erase")
  {
    int k = 3;
    cc_map_erase(v, &k);
    check_map<int, double>(v, { {1, 1.1}, {2, 2.2}, {4, 4.4} });
  }

  SUBCASE("swap")
  {
    cc_map_swap(u, v);
    check_map<int, double>(u, { {1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4} });
    check_map<int, double>(v, { });
  }

  SUBCASE("merge")
  {
    a = {2, 0.2};
    cc_map_insert(u, &a.first, &a.second);
    a = {5, 0.5};
    cc_map_insert(u, &a.first, &a.second);
    cc_map_merge(u, v);
    check_map<int, double>(u, { {1, 1.1}, {2, 0.2}, {3, 3.3}, {4, 4.4}, {5, 0.5} });
    check_map<int, double>(v, { {2, 2.2} });
  }

  cc_map_delete(u);
  cc_map_delete(v);
}

TEST_CASE("map hash policy [atomic]")
{
  std::map<int, double> x = { {1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4} };
  cc_map_t u = create_map(x);

  SUBCASE("load factor")
  {
    CHECK(cc_map_capacity(u) == 16);
    CHECK(cc_map_load_factor(u) == doctest::Approx(0.25));

    cc_map_reserve(u, 51);
    CHECK(cc_map_capacity(u) == 64);
    CHECK(cc_map_load_factor(u) == doctest::Approx(0.0625));

    cc_map_reserve(u, 52);
    CHECK(cc_map_capacity(u) == 128);
    CHECK(cc_map_load_factor(u) == doctest::Approx(0.03125));
  }

  SUBCASE("maximum load factor")
  {
    CHECK(cc_map_capacity(u) == 16);
    CHECK(cc_map_max_load_factor(u) == doctest::Approx(0.8));

    cc_map_reserve(u, 51);
    CHECK(cc_map_capacity(u) == 64);
    CHECK(cc_map_max_load_factor(u) == doctest::Approx(0.8));

    cc_map_reserve(u, 52);
    CHECK(cc_map_capacity(u) == 128);
    CHECK(cc_map_max_load_factor(u) == doctest::Approx(0.8));
  }

  SUBCASE("setting maximum load factor")
  {
    cc_map_set_max_load_factor(u, 0.9);

    CHECK(cc_map_capacity(u) == 16);
    CHECK(cc_map_max_load_factor(u) == doctest::Approx(0.9));

    cc_map_reserve(u, 51);
    CHECK(cc_map_capacity(u) == 64);
    CHECK(cc_map_max_load_factor(u) == doctest::Approx(0.9));

    cc_map_reserve(u, 52);
    CHECK(cc_map_capacity(u) == 64);
    CHECK(cc_map_max_load_factor(u) == doctest::Approx(0.9));

    cc_map_reserve(u, 58);
    CHECK(cc_map_capacity(u) == 128);
    CHECK(cc_map_max_load_factor(u) == doctest::Approx(0.9));
  }

  cc_map_delete(u);
}

TEST_CASE("map comparison [atomic]")
{
  std::map<int, int> x = { {1, 2}, {3, 4}, {5, 6} };
  std::map<int, int> y = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };
  std::map<int, int> z = { {1, 2}, {3, 4}, {5, 9}, {7, 8} };

  cc_map_t a = create_map(x);
  cc_map_t b = create_map(x);
  cc_map_t c = create_map(y);
  cc_map_t d = create_map(z);
  cc_map_reserve(b, x.size() + 16);

  SUBCASE("equality")
  {
    CHECK(cc_map_eq(a, b));
    CHECK(cc_map_eq(b, a));
    CHECK(!cc_map_eq(a, c));
    CHECK(!cc_map_eq(c, a));
    CHECK(!cc_map_eq(b, c));
    CHECK(!cc_map_eq(c, b));
  }

  SUBCASE("inequality")
  {
    CHECK(!cc_map_ne(a, b));
    CHECK(!cc_map_ne(b, a));
    CHECK(cc_map_ne(a, c));
    CHECK(cc_map_ne(c, a));
    CHECK(cc_map_ne(b, c));
    CHECK(cc_map_ne(c, b));
  }

  cc_map_delete(a);
  cc_map_delete(b);
  cc_map_delete(c);
  cc_map_delete(d);
}

TEST_SUITE_END();
