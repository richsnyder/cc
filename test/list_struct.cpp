/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include "doctest/doctest.h"
#include "cc.h"
#include "point.hpp"
#include "list.hpp"

TEST_SUITE_BEGIN("lists");

TEST_CASE("list construction [struct]")
{
  SUBCASE("empty list")
  {
    cc_list_t u = cc_list_new(sizeof(point));
    check_list<point>(u, { });
    cc_list_delete(u);
  }

  SUBCASE("create from an array")
  {
    std::vector<point> x = { {1.2, 3.4}, {5.6, 7.8} };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(point));
    check_list(u, x);
    cc_list_delete(u);
  }

  SUBCASE("copy")
  {
    std::vector<point> x = { {-1.0, -2.0}, {0.0, 0.0}, {1.0, 2.0} };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(point));
    cc_list_t v = cc_list_copy(u);
    check_list(v, x);
    cc_list_delete(u);
    cc_list_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_list_t u = cc_list_new(sizeof(point));
    cc_list_t v = cc_list_copy(u);
    check_list<point>(v, { });
    cc_list_delete(u);
    cc_list_delete(v);
  }
}

TEST_CASE("list assignment [struct]")
{
  point a = {1.2, 3.4};
  std::vector<point> x = { {1.2, 3.4}, {1.2, 3.4}, {1.2, 3.4}, {1.2, 3.4} };
  cc_list_t u = cc_list_new(sizeof(point));
  cc_list_assign(u, 4, &a);
  check_list(u, x);
  cc_list_delete(u);
}

TEST_CASE("list element access [struct]")
{
  std::vector<point> x = { {2., 3.}, {5., 7.}, {11., 13.}, {17., 19.} };
  cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(point));

  SUBCASE("front")
  {
    CHECK(((point*) cc_list_front(u))->x == doctest::Approx(2.0));
    CHECK(((point*) cc_list_front(u))->y == doctest::Approx(3.0));
  }

  SUBCASE("back")
  {
    CHECK(((point*) cc_list_back(u))->x == doctest::Approx(17.0));
    CHECK(((point*) cc_list_back(u))->y == doctest::Approx(19.0));
  }

  cc_list_delete(u);
}

TEST_CASE("list iteration [struct]")
{
  std::vector<point> x = { {1., 2.}, {3., 4.}, {5., 6.}, {7., 8.} };
  cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(point));
  auto eq = std::equal_to<point>();

  cc_list_iterator_t p = cc_list_begin(u);
  cc_list_iterator_t e = cc_list_end(u);
  for (size_t n = 0; n < x.size(); ++n)
  {
    REQUIRE(cc_list_iterator_ne(p, e));
    CHECK(eq(*(const point*) cc_list_iterator_dereference(p), x[n]));
    cc_list_iterator_increment(&p);
  }
  REQUIRE(cc_list_iterator_eq(p, e));

  cc_list_delete(u);
}

TEST_CASE("list capacity [struct]")
{
  point a = {7.7, -8.8};
  std::vector<point> x = { {1.1, -2.2}, {3.3, -4.4}, {5.5, -6.6} };
  cc_list_t u = cc_list_new(sizeof(point));
  cc_list_t v = cc_list_from_array(x.data(), x.size(), sizeof(point));

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

TEST_CASE("list modification [struct]")
{
  point a = {13.0, 21.0};
  std::vector<point> x = { {1.0, 1.0}, {2.0, 3.0}, {5.0, 8.0} };
  cc_list_t u = cc_list_new(sizeof(point));
  cc_list_t v = cc_list_from_array(x.data(), x.size(), sizeof(point));

  SUBCASE("clear")
  {
    cc_list_clear(u);
    cc_list_clear(v);
    CHECK(cc_list_empty(u));
    CHECK(cc_list_empty(v));
  }

  SUBCASE("insert")
  {
    a = {2.1, 2.2};
    cc_list_insert(u, cc_list_begin(u), &a);
    check_list<point>(u, { {2.1, 2.2} });
    a = {1.1, 1.2};
    cc_list_insert(u, cc_list_begin(u), &a);
    check_list<point>(u, { {1.1, 1.2}, {2.1, 2.2} });
    a = {3.1, 3.2};
    cc_list_insert(u, cc_list_end(u), &a);
    check_list<point>(u, { {1.1, 1.2}, {2.1, 2.2}, {3.1, 3.2} });

    a = {0.0, 0.0};
    cc_list_iterator_t p = cc_list_end(v);
    cc_list_iterator_decrement(&p);
    cc_list_insert(v, p, &a);
    check_list<point>(v, { {1.0, 1.0}, {2.0, 3.0}, {0.0, 0.0}, {5.0, 8.0} });
  }

  SUBCASE("erase")
  {
    cc_list_iterator_t first = cc_list_begin(v);
    cc_list_iterator_t last = cc_list_end(v);
    cc_list_iterator_increment(&first);
    cc_list_erase(v, first, last);
    check_list<point>(v, { {1.0, 1.0} });

    first = cc_list_begin(v);
    last = cc_list_end(v);
    cc_list_erase(v, first, last);
    check_list<point>(v, { });
  }

  SUBCASE("push back")
  {
    cc_list_push_back(u, &a);
    cc_list_push_back(v, &a);
    check_list<point>(u, { {13.0, 21.0} });
    check_list<point>(v, { {1.0, 1.0}, {2.0, 3.0}, {5.0, 8.0}, {13.0, 21.0} });
  }

  SUBCASE("pop back")
  {
    cc_list_pop_back(v);
    check_list<point>(v, { {1.0, 1.0}, {2.0, 3.0} });
  }

  SUBCASE("push front")
  {
    cc_list_push_front(u, &a);
    cc_list_push_front(v, &a);
    check_list<point>(u, { {13.0, 21.0} });
    check_list<point>(v, { {13.0, 21.0}, {1.0, 1.0}, {2.0, 3.0}, {5.0, 8.0} });
  }

  SUBCASE("pop front")
  {
    cc_list_pop_front(v);
    check_list<point>(v, { {2.0, 3.0}, {5.0, 8.0} });
  }

  SUBCASE("resize")
  {
    cc_list_resize(u, 2, &a);
    cc_list_resize(v, 2, &a);
    check_list<point>(u, { {13.0, 21.0}, {13.0, 21.0} });
    check_list<point>(v, { {1.0, 1.0}, {2.0, 3.0} });
  }

  SUBCASE("swap")
  {
    cc_list_swap(u, v);
    check_list<point>(u, { {1.0, 1.0}, {2.0, 3.0}, {5.0, 8.0} });
    check_list<point>(v, { });
  }

  cc_list_delete(u);
  cc_list_delete(v);
}

TEST_CASE("list comparison [struct]")
{
  std::vector<point> x = { {1., 2.}, {3., 4.}, {5., 6.} };
  std::vector<point> y = { {1., 2.}, {3., 4.}, {5., 6.}, {7., 8.} };
  std::vector<point> z = { {1., 2.}, {3., 4.}, {5., 9.}, {7., 8.} };

  cc_list_t a = cc_list_from_array(x.data(), x.size(), sizeof(point));
  cc_list_t b = cc_list_from_array(x.data(), x.size(), sizeof(point));
  cc_list_t c = cc_list_from_array(y.data(), y.size(), sizeof(point));
  cc_list_t d = cc_list_from_array(z.data(), z.size(), sizeof(point));

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
