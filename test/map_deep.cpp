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
#include "map.hpp"

cc_map_t
iarray_map_new()
{
  return cc_map_new_f(
      sizeof(struct iarray),
      sizeof(struct iarray),
      iarray_functions,
      iarray_functions
    );
}

cc_map_t
iarray_map_from_arrays(const std::map<iarray, iarray>& map)
{
  std::vector<iarray> keys;
  std::vector<iarray> values;
  keys.reserve(map.size());
  values.reserve(map.size());
  for (const auto& kv : map)
  {
    keys.push_back(kv.first);
    values.push_back(kv.second);
  }
  return cc_map_from_arrays_f(
      keys.data(),
      values.data(),
      map.size(),
      sizeof(iarray),
      sizeof(iarray),
      iarray_functions,
      iarray_functions
    );
}

TEST_SUITE_BEGIN("maps");

TEST_CASE("map construction [deep]")
{
  SUBCASE("empty list")
  {
    cc_map_t u = iarray_map_new();
    check_map<iarray, iarray>(u, { });
    cc_map_delete(u);
  }

  SUBCASE("create from arrays")
  {
    int x1[1] = { 111 };
    int x2[2] = { 222, 333 };
    int x3[3] = { 444, 555, 666 };
    std::vector<iarray> ks = { {1, x1}, {2, x2}, {3, x3} };
    std::vector<iarray> vs = { {2, x2}, {3, x3}, {1, x1} };
    std::map<iarray, iarray> x = {
      { {1, x1}, {2, x2} },
      { {2, x2}, {3, x3} },
      { {3, x3}, {1, x1} }
    };

    cc_map_t u = cc_map_from_arrays_f(
        ks.data(),
        vs.data(),
        ks.size(),
        sizeof(iarray),
        sizeof(iarray),
        iarray_functions,
        iarray_functions
      );

    cc_map_t v = iarray_map_from_arrays(x);
    check_map(u, x);
    check_map(v, x);
    cc_map_delete(u);
    cc_map_delete(v);
  }

  SUBCASE("copy")
  {
    int x1[1] = { 111 };
    int x2[2] = { 222, 333 };
    int x3[3] = { 444, 555, 666 };
    std::map<iarray, iarray> x = {
      { {1, x1}, {2, x2} },
      { {2, x2}, {3, x3} },
      { {3, x3}, {1, x1} }
    };
    cc_map_t u = iarray_map_from_arrays(x);
    cc_map_t v = cc_map_copy(u);
    check_map(v, x);
    cc_map_delete(u);
    cc_map_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_map_t u = cc_map_new(sizeof(iarray), sizeof(iarray));
    cc_map_t v = cc_map_copy(u);
    check_map<iarray, iarray>(v, { });
    cc_map_delete(u);
    cc_map_delete(v);
  }
}

TEST_CASE("map iteration [deep]")
{
  int x1[1] = { 111 };
  int x2[2] = { 222, 333 };
  int x3[3] = { 444, 555, 666 };
  std::map<iarray, iarray> x = {
    { {1, x1}, {2, x2} },
    { {2, x2}, {3, x3} },
    { {3, x3}, {1, x1} }
  };

  auto eq = std::equal_to<iarray>();
  std::map<iarray, iarray>::const_iterator q;
  cc_map_t u = iarray_map_from_arrays(x);
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
      q = x.find(*(iarray*) kv.key);
      REQUIRE(q != x.end());
      CHECK(eq(*(iarray*) kv.value, q->second));
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
      q = x.find(*(iarray*) kv.key);
      REQUIRE(q != x.end());
      CHECK(eq(*(iarray*) kv.value, q->second));
    }
    REQUIRE(cc_map_iterator_eq(p, b));
  }

  cc_map_delete(u);
}

TEST_CASE("map capacity [deep]")
{
  iarray key;
  iarray value;
  cc_map_t u = iarray_map_new();
  cc_map_t v = iarray_map_new();

  for (auto n = 1; n <= 12; ++n)
  {
    std::vector<int> k_(1, n);
    std::vector<int> v_(n, n);
    key = { k_.size(), k_.data() };
    value = { v_.size(), v_.data() };
    cc_map_insert(v, &key, &value);
  }

  int xk[1] = { 13 };
  int xv[13] = { 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 };
  key = { 1, xk };
  value = { 13, xv };

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

TEST_CASE("map modification [deep]")
{
  int x1[1] = { 111 };
  int x2[2] = { 222, 333 };
  int x3[3] = { 444, 555, 666 };
  int x4[2] = { 777, 888 };
  int x5[1] = { 999 };
  std::pair<iarray, iarray> a = { {1, x5}, {1, x1} };
  std::map<iarray, iarray> x = {
    { {1, x1}, {2, x2} },
    { {2, x2}, {3, x3} },
    { {3, x3}, {2, x4} },
    { {2, x4}, {1, x5} }
  };
  cc_map_t u = iarray_map_new();
  cc_map_t v = iarray_map_from_arrays(x);

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
    check_map<iarray, iarray>(u, {
      { {1, x5}, {1, x1} }
    });
    check_map<iarray, iarray>(v, {
      { {1, x1}, {2, x2} },
      { {2, x2}, {3, x3} },
      { {3, x3}, {2, x4} },
      { {2, x4}, {1, x5} },
      { {1, x5}, {1, x1} }
    });
  }

  SUBCASE("erase")
  {
    iarray k = {2, x2};
    cc_map_erase(v, &k);
    check_map<iarray, iarray>(v, {
      { {1, x1}, {2, x2} },
      { {3, x3}, {2, x4} },
      { {2, x4}, {1, x5} }
    });
  }

  SUBCASE("swap")
  {
    cc_map_swap(u, v);
    check_map<iarray, iarray>(u, {
      { {1, x1}, {2, x2} },
      { {2, x2}, {3, x3} },
      { {3, x3}, {2, x4} },
      { {2, x4}, {1, x5} }
    });
    check_map<iarray, iarray>(v, { });
  }

  cc_map_delete(u);
  cc_map_delete(v);
}

TEST_CASE("map hash policy [deep]")
{
  int x1[1] = { 111 };
  int x2[2] = { 222, 333 };
  int x3[3] = { 444, 555, 666 };
  int x4[2] = { 777, 888 };
  int x5[1] = { 999 };
  std::map<iarray, iarray> x = {
    { {1, x1}, {2, x2} },
    { {2, x2}, {3, x3} },
    { {3, x3}, {2, x4} },
    { {2, x4}, {1, x5} }
  };

  cc_map_t u = iarray_map_from_arrays(x);

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

TEST_CASE("map comparison [deep]")
{
  int x1[1] = { 111 };
  int x2[2] = { 222, 333 };
  int x3[3] = { 444, 555, 666 };
  int x4[2] = { 777, 888 };
  int x5[1] = { 999 };
  int x6[3] = { 444, 595, 666 };
  std::map<iarray, iarray> x = {
    { {1, x1}, {2, x2} },
    { {2, x2}, {3, x3} },
    { {3, x3}, {2, x4} }
  };
  std::map<iarray, iarray> y = {
    { {1, x1}, {2, x2} },
    { {2, x2}, {3, x3} },
    { {3, x3}, {2, x4} },
    { {2, x4}, {1, x5} }
  };
  std::map<iarray, iarray> z = {
    { {1, x1}, {2, x2} },
    { {2, x2}, {3, x6} },
    { {3, x3}, {2, x4} },
    { {2, x4}, {1, x5} }
  };

  cc_map_t a = iarray_map_from_arrays(x);
  cc_map_t b = iarray_map_from_arrays(x);
  cc_map_t c = iarray_map_from_arrays(y);
  cc_map_t d = iarray_map_from_arrays(z);
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
