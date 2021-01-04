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
#include "point.hpp"
#include "map.hpp"

TEST_SUITE_BEGIN("maps");

TEST_CASE("map construction [struct]")
{
  SUBCASE("empty list")
  {
    cc_map_t u = cc_map_new(sizeof(point), sizeof(point));
    CHECK(cc_map_size(u) == 0);
    cc_map_delete(u);
  }

  SUBCASE("create from arrays")
  {
    std::vector<point> ks = { {0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}, {0.7, 0.8} };
    std::vector<point> vs = { {1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}, {7.0, 8.0} };
    std::map<point, point> x = {
      { {0.1, 0.2}, {1.0, 2.0} },
      { {0.3, 0.4}, {3.0, 4.0} },
      { {0.5, 0.6}, {5.0, 6.0} },
      { {0.7, 0.8}, {7.0, 8.0} }
    };

    cc_map_t u = cc_map_from_arrays(
        ks.data(),
        vs.data(),
        ks.size(),
        sizeof(point),
        sizeof(point)
      );
    cc_map_t v = create_map(x);
    check_map(u, x);
    check_map(v, x);
    cc_map_delete(u);
    cc_map_delete(v);
  }

  SUBCASE("copy")
  {
    std::map<point, point> x = {
      { {0.1, 0.2}, {1.0, 2.0} },
      { {0.3, 0.4}, {3.0, 4.0} },
      { {0.5, 0.6}, {5.0, 6.0} },
      { {0.7, 0.8}, {7.0, 8.0} }
    };
    cc_map_t u = create_map(x);
    cc_map_t v = cc_map_copy(u);
    check_map(v, x);
    cc_map_delete(u);
    cc_map_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_map_t u = cc_map_new(sizeof(point), sizeof(point));
    cc_map_t v = cc_map_copy(u);
    check_map<point, point>(v, { });
    cc_map_delete(u);
    cc_map_delete(v);
  }
}

TEST_CASE("map iteration [struct]")
{
  std::map<point, point> x = {
    { {0.1, 0.2}, {1.0, 2.0} },
    { {0.3, 0.4}, {3.0, 4.0} },
    { {0.5, 0.6}, {5.0, 6.0} },
    { {0.7, 0.8}, {7.0, 8.0} }
  };
  auto eq = std::equal_to<point>();
  std::map<point, point>::const_iterator q;
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
      q = x.find(*(point*) kv.key);
      REQUIRE(q != x.end());
      CHECK(eq(*(point*) kv.value, q->second));
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
      q = x.find(*(point*) kv.key);
      REQUIRE(q != x.end());
      CHECK(eq(*(point*) kv.value, q->second));
    }
    REQUIRE(cc_map_iterator_eq(p, b));
  }

  cc_map_delete(u);
}

TEST_CASE("map capacity [struct]")
{
  point key = {99.0, 99.0};
  point value = {198.0, 198.0};
  std::map<point, point> x;
  for (auto n = 0; n < 12; ++n) x[{1.0 * n, 1.0 * n}] = {2.0 * n, 2.0 * n};

  cc_map_t u = cc_map_new(sizeof(point), sizeof(point));
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

TEST_CASE("map modification [struct]")
{
  std::pair<point, point> a = { {0.9, 0.0}, {9.0, 0.0} };
  std::map<point, point> x = {
    { {0.1, 0.2}, {1.0, 2.0} },
    { {0.3, 0.4}, {3.0, 4.0} },
    { {0.5, 0.6}, {5.0, 6.0} },
    { {0.7, 0.8}, {7.0, 8.0} }
  };
  cc_map_t u = cc_map_new(sizeof(point), sizeof(point));
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
    check_map<point, point>(u, {
      { {0.9, 0.0}, {9.0, 0.0} }
    });
    check_map<point, point>(v, {
      { {0.1, 0.2}, {1.0, 2.0} },
      { {0.3, 0.4}, {3.0, 4.0} },
      { {0.5, 0.6}, {5.0, 6.0} },
      { {0.7, 0.8}, {7.0, 8.0} },
      { {0.9, 0.0}, {9.0, 0.0} }
    });
  }

  SUBCASE("erase")
  {
    point k = {0.3, 0.4};
    cc_map_erase(v, &k);
    check_map<point, point>(v, {
      { {0.1, 0.2}, {1.0, 2.0} },
      { {0.5, 0.6}, {5.0, 6.0} },
      { {0.7, 0.8}, {7.0, 8.0} }
    });
  }

  SUBCASE("swap")
  {
    cc_map_swap(u, v);
    check_map<point, point>(u, {
      { {0.1, 0.2}, {1.0, 2.0} },
      { {0.3, 0.4}, {3.0, 4.0} },
      { {0.5, 0.6}, {5.0, 6.0} },
      { {0.7, 0.8}, {7.0, 8.0} }
    });
    check_map<point, point>(v, { });
  }

  cc_map_delete(u);
  cc_map_delete(v);
}

TEST_CASE("map hash policy [struct]")
{
  std::map<point, point> x = {
    { {0.1, 0.2}, {1.0, 2.0} },
    { {0.3, 0.4}, {3.0, 4.0} },
    { {0.5, 0.6}, {5.0, 6.0} },
    { {0.7, 0.8}, {7.0, 8.0} }
  };
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

TEST_CASE("map comparison [struct]")
{
  std::map<point, point> x = {
    { {0.1, 0.2}, {1.0, 2.0} },
    { {0.3, 0.4}, {3.0, 4.0} },
    { {0.5, 0.6}, {5.0, 6.0} }
  };
  std::map<point, point> y = {
    { {0.1, 0.2}, {1.0, 2.0} },
    { {0.3, 0.4}, {3.0, 4.0} },
    { {0.5, 0.6}, {5.0, 6.0} },
    { {0.7, 0.8}, {7.0, 8.0} }
  };
  std::map<point, point> z = {
    { {0.1, 0.2}, {1.0, 2.0} },
    { {0.3, 0.4}, {3.0, 4.0} },
    { {0.5, 0.6}, {5.0, 9.0} },
    { {0.7, 0.8}, {7.0, 8.0} }
  };

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
