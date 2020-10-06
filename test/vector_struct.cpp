/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include "doctest/doctest.h"
#include "cc.h"
#include "point.hpp"
#include "vector.hpp"

TEST_SUITE_BEGIN("vectors");

TEST_CASE("vector construction [struct]")
{
  SUBCASE("empty vector")
  {
    cc_vector_t u = cc_vector_new(sizeof(point));
    check_vector<point>(u, { });
    cc_vector_delete(u);
  }

  SUBCASE("create from an array")
  {
    std::vector<point> x = { {1.2, 3.4}, {5.6, 7.8} };
    cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(point));
    check_vector(u, x);
    cc_vector_delete(u);
  }

  SUBCASE("copy")
  {
    std::vector<point> x = { {-1.0, -2.0}, {0.0, 0.0}, {1.0, 2.0} };
    cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(point));
    cc_vector_t v = cc_vector_copy(u);
    check_vector(v, x);
    cc_vector_delete(u);
    cc_vector_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_vector_t u = cc_vector_new(sizeof(point));
    cc_vector_t v = cc_vector_copy(u);
    check_vector<point>(v, { });
    cc_vector_delete(u);
    cc_vector_delete(v);
  }
}

TEST_CASE("vector assignment [struct]")
{
  point a = {1.2, 3.4};
  std::vector<point> x = { {1.2, 3.4}, {1.2, 3.4}, {1.2, 3.4}, {1.2, 3.4} };
  cc_vector_t u = cc_vector_new(sizeof(point));
  cc_vector_assign(u, 4, &a);
  check_vector(u, x);
  cc_vector_delete(u);
}

TEST_CASE("vector element access [struct]")
{
  std::vector<point> x = { {2., 3.}, {5., 7.}, {11., 13.}, {17., 19.} };
  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(point));

  SUBCASE("get")
  {
    check_vector(u, x);
  }

  SUBCASE("front")
  {
    CHECK(((point*) cc_vector_front(u))->x == doctest::Approx(2.0));
    CHECK(((point*) cc_vector_front(u))->y == doctest::Approx(3.0));
  }

  SUBCASE("back")
  {
    CHECK(((point*) cc_vector_back(u))->x == doctest::Approx(17.0));
    CHECK(((point*) cc_vector_back(u))->y == doctest::Approx(19.0));
  }

  SUBCASE("data")
  {
    point* data = (point*) cc_vector_data(u);
    CHECK(data[0].x == 2.0);
    CHECK(data[1].y == 7.0);
    CHECK(data[2].x == 11.0);
    CHECK(data[3].y == 19.0);
  }

  cc_vector_delete(u);
}

TEST_CASE("vector iteration [struct]")
{
  std::vector<point> x = { {1., 2.}, {3., 4.}, {5., 6.}, {7., 8.} };
  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(point));
  auto eq = std::equal_to<point>();

  cc_vector_iterator_t p = cc_vector_begin(u);
  cc_vector_iterator_t e = cc_vector_end(u);
  for (size_t n = 0; n < x.size(); ++n)
  {
    REQUIRE(cc_vector_iterator_ne(p, e));
    CHECK(eq(*(const point*) cc_vector_iterator_dereference(p), x[n]));
    cc_vector_iterator_increment(&p);
  }
  REQUIRE(cc_vector_iterator_eq(p, e));

  cc_vector_delete(u);
}

TEST_CASE("vector capacity [struct]")
{
  point a = {7.7, -8.8};
  std::vector<point> x = { {1.1, -2.2}, {3.3, -4.4}, {5.5, -6.6} };
  cc_vector_t u = cc_vector_new(sizeof(point));
  cc_vector_t v = cc_vector_from_array(x.data(), x.size(), sizeof(point));

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

TEST_CASE("vector modification [struct]")
{
  point a = {13.0, 21.0};
  std::vector<point> x = { {1.0, 1.0}, {2.0, 3.0}, {5.0, 8.0} };
  cc_vector_t u = cc_vector_new(sizeof(point));
  cc_vector_t v = cc_vector_from_array(x.data(), x.size(), sizeof(point));

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
    check_vector<point>(u, { {13.0, 21.0} });
    check_vector<point>(v, { {1., 1.}, {13., 21.}, {2., 3.}, {5., 8.} });
  }

  SUBCASE("erase")
  {
    cc_vector_erase(v, 1, 10);
    check_vector<point>(v, { {1.0, 1.0} });
  }

  SUBCASE("push back")
  {
    cc_vector_push_back(u, &a);
    cc_vector_push_back(v, &a);
    check_vector<point>(u, { {13.0, 21.0} });
    check_vector<point>(v, { {1., 1.}, {2., 3.}, {5., 8.}, {13., 21.} });
  }

  SUBCASE("pop back")
  {
    cc_vector_pop_back(v);
    check_vector<point>(v, { {1.0, 1.0}, {2.0, 3.0} });
  }

  SUBCASE("resize")
  {
    cc_vector_resize(u, 2, &a);
    cc_vector_resize(v, 2, &a);
    check_vector<point>(u, { {13.0, 21.0}, {13.0, 21.0} });
    check_vector<point>(v, { {1.0, 1.0}, {2.0, 3.0} });
  }

  SUBCASE("swap")
  {
    cc_vector_swap(u, v);
    check_vector<point>(u, { {1.0, 1.0}, {2.0, 3.0}, {5.0, 8.0} });
    check_vector<point>(v, { });
  }

  cc_vector_delete(u);
  cc_vector_delete(v);
}

TEST_CASE("vector comparison [struct]")
{
  std::vector<point> x = { {1., 2.}, {3., 4.}, {5., 6.} };
  std::vector<point> y = { {1., 2.}, {3., 4.}, {5., 6.}, {7., 8.} };
  std::vector<point> z = { {1., 2.}, {3., 4.}, {5., 9.}, {7., 8.} };

  cc_vector_t a = cc_vector_from_array(x.data(), x.size(), sizeof(point));
  cc_vector_t b = cc_vector_from_array(x.data(), x.size(), sizeof(point));
  cc_vector_t c = cc_vector_from_array(y.data(), y.size(), sizeof(point));
  cc_vector_t d = cc_vector_from_array(z.data(), z.size(), sizeof(point));
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
