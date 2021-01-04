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
#include "list.hpp"

TEST_SUITE_BEGIN("lists");

TEST_CASE("list construction [atomic]")
{
  SUBCASE("empty list")
  {
    cc_list_t u = cc_list_new(sizeof(int));
    check_list<int>(u, { });
    cc_list_delete(u);
  }

  SUBCASE("create from an array")
  {
    std::vector<int> x = { 1, 4, 9, 16, 25 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    check_list(u, x);
    cc_list_delete(u);
  }

  SUBCASE("copy")
  {
    std::vector<double> x = { 1.2, 3.4, 5.6 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(double));
    cc_list_t v = cc_list_copy(u);
    check_list(v, x);
    cc_list_delete(u);
    cc_list_delete(v);
  }

  SUBCASE("copy empty")
  {
    cc_list_t u = cc_list_new(sizeof(double));
    cc_list_t v = cc_list_copy(u);
    check_list<double>(v, { });
    cc_list_delete(u);
    cc_list_delete(v);
  }
}

TEST_CASE("list assignment [atomic]")
{
  int a = 123;
  std::vector<int> x = { 123, 123, 123, 123 };
  cc_list_t u = cc_list_new(sizeof(int));
  cc_list_assign(u, 4, &a);
  check_list(u, x);
  cc_list_delete(u);
}

TEST_CASE("list element access [atomic]")
{
  std::vector<int> x = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };
  cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));

  SUBCASE("front")
  {
    CHECK(*(int*) cc_list_front(u) == 2);
  }

  SUBCASE("back")
  {
    CHECK(*(int*) cc_list_back(u) == 29);
  }

  cc_list_delete(u);
}

TEST_CASE("list iteration [atomic]")
{
  std::vector<int> x = { 1, 22, 333, 4444 };
  cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));

  cc_list_iterator_t p = cc_list_begin(u);
  cc_list_iterator_t e = cc_list_end(u);
  for (size_t n = 0; n < x.size(); ++n)
  {
    REQUIRE(cc_list_iterator_ne(p, e));
    CHECK(*(int*) cc_list_iterator_dereference(p) == x[n]);
    cc_list_iterator_increment(&p);
  }
  REQUIRE(cc_list_iterator_eq(p, e));

  cc_list_delete(u);
}

TEST_CASE("list capacity [atomic]")
{
  double a = -6.6;
  std::vector<double> x = { 1.1, -2.2, 3.3, -4.4, 5.5 };
  cc_list_t u = cc_list_new(sizeof(double));
  cc_list_t v = cc_list_from_array(x.data(), x.size(), sizeof(double));

  SUBCASE("empty")
  {
    CHECK(cc_list_empty(u));
    CHECK(!cc_list_empty(v));
  }

  SUBCASE("size")
  {
    CHECK(cc_list_size(u) == 0);
    CHECK(cc_list_size(v) == 5);
  }

  cc_list_delete(u);
  cc_list_delete(v);
}

TEST_CASE("list modification [atomic]")
{
  double a = -6.6;
  std::vector<double> x = { 1.1, -2.2, 3.3, -4.4, 5.5 };
  cc_list_t u = cc_list_new(sizeof(double));
  cc_list_t v = cc_list_from_array(x.data(), x.size(), sizeof(double));

  SUBCASE("clear")
  {
    cc_list_clear(u);
    cc_list_clear(v);
    CHECK(cc_list_empty(u));
    CHECK(cc_list_empty(v));
  }

  SUBCASE("insert")
  {
    a = 2.0;
    cc_list_insert(u, cc_list_begin(u), &a);
    check_list<double>(u, { 2.0 });
    a = 1.0;
    cc_list_insert(u, cc_list_begin(u), &a);
    check_list<double>(u, { 1.0, 2.0 });
    a = 3.0;
    cc_list_insert(u, cc_list_end(u), &a);
    check_list<double>(u, { 1.0, 2.0, 3.0 });

    a = 0.0;
    cc_list_iterator_t p = cc_list_end(v);
    cc_list_iterator_decrement(&p);
    cc_list_insert(v, p, &a);
    check_list<double>(v, { 1.1, -2.2, 3.3, -4.4, 0.0, 5.5 });
  }

  SUBCASE("erase")
  {
    cc_list_iterator_t first = cc_list_begin(v);
    cc_list_iterator_t last = cc_list_end(v);
    cc_list_iterator_increment(&first);
    cc_list_iterator_decrement(&last);
    cc_list_iterator_decrement(&last);
    cc_list_erase(v, first, last);
    check_list<double>(v, { 1.1, -4.4, 5.5 });

    first = cc_list_begin(v);
    last = cc_list_begin(v);
    cc_list_iterator_increment(&last);
    cc_list_erase(v, first, last);
    check_list<double>(v, { -4.4, 5.5 });

    first = cc_list_end(v);
    last = cc_list_end(v);
    cc_list_iterator_decrement(&first);
    cc_list_erase(v, first, last);
    check_list<double>(v, { -4.4 });

    first = cc_list_begin(v);
    last = cc_list_end(v);
    cc_list_erase(v, first, last);
    check_list<double>(v, { });
  }

  SUBCASE("push back")
  {
    cc_list_push_back(u, &a);
    cc_list_push_back(v, &a);
    check_list<double>(u, { -6.6 });
    check_list<double>(v, { 1.1, -2.2, 3.3, -4.4, 5.5, -6.6 });
  }

  SUBCASE("pop back")
  {
    cc_list_pop_back(v);
    check_list<double>(v, { 1.1, -2.2, 3.3, -4.4 });
    cc_list_pop_back(v);
    check_list<double>(v, { 1.1, -2.2, 3.3 });
    cc_list_pop_back(v);
    check_list<double>(v, { 1.1, -2.2 });
    cc_list_pop_back(v);
    check_list<double>(v, { 1.1 });
    cc_list_pop_back(v);
    check_list<double>(v, { });
    cc_list_pop_back(v);
    check_list<double>(v, { });
  }

  SUBCASE("push front")
  {
    cc_list_push_front(u, &a);
    cc_list_push_front(v, &a);
    check_list<double>(u, { -6.6 });
    check_list<double>(v, { -6.6, 1.1, -2.2, 3.3, -4.4, 5.5 });
  }

  SUBCASE("pop front")
  {
    cc_list_pop_front(v);
    check_list<double>(v, { -2.2, 3.3, -4.4, 5.5 });
    cc_list_pop_front(v);
    check_list<double>(v, { 3.3, -4.4, 5.5 });
    cc_list_pop_front(v);
    check_list<double>(v, { -4.4, 5.5 });
    cc_list_pop_front(v);
    check_list<double>(v, { 5.5 });
    cc_list_pop_front(v);
    check_list<double>(v, { });
    cc_list_pop_front(v);
    check_list<double>(v, { });
  }

  SUBCASE("resize")
  {
    cc_list_resize(u, 3, &a);
    cc_list_resize(v, 3, &a);
    check_list<double>(u, { -6.6, -6.6, -6.6 });
    check_list<double>(v, { 1.1, -2.2, 3.3 });
  }

  SUBCASE("swap")
  {
    cc_list_swap(u, v);
    check_list<double>(u, { 1.1, -2.2, 3.3, -4.4, 5.5 });
    check_list<double>(v, { });
  }

  cc_list_delete(u);
  cc_list_delete(v);
}

TEST_CASE("list operations [atomic]")
{
  auto less = [](void* p, void* q) {
      int* a = reinterpret_cast<int*>(p);
      int* b = reinterpret_cast<int*>(q);
      return *a < *b;
    };

  SUBCASE("merge")
  {
    std::vector<int> x = { 1, 3, 5, 6 };
    std::vector<int> y = { 2, 4, 7, 8, 9 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    cc_list_t v = cc_list_from_array(y.data(), y.size(), sizeof(int));
    cc_list_merge(u, v, less);
    check_list<int>(u, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });
    cc_list_delete(u);
    cc_list_delete(v);
  }

  SUBCASE("splice")
  {
    std::vector<int> x = { 1, 2, 5, 6 };
    std::vector<int> y = { 3, 4 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    cc_list_t v = cc_list_from_array(y.data(), y.size(), sizeof(int));
    cc_list_iterator_t p = cc_list_begin(u);
    cc_list_iterator_increment(&p);
    cc_list_iterator_increment(&p);
    cc_list_splice(u, p, v);
    check_list<int>(u, { 1, 2, 3, 4, 5, 6 });
    check_list<int>(v, { });
    cc_list_delete(u);
    cc_list_delete(v);
  }

  SUBCASE("remove")
  {
    int a = 2;
    std::vector<int> x = { 1, 2, 3, 2, 4, 2, 2 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    cc_list_remove(u, &a);
    check_list<int>(u, { 1, 3, 4 });
    cc_list_delete(u);
  }

  SUBCASE("remove if")
  {
    auto pred = [](void* p){ return *reinterpret_cast<int*>(p) % 2 == 0; };
    std::vector<int> x = { 1, 2, 5, 6 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    cc_list_remove_if(u, pred);
    check_list<int>(u, { 1, 5 });
    cc_list_delete(u);
  }

  SUBCASE("reverse")
  {
    std::vector<int> x = { 1, 2, 5, 6 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    cc_list_reverse(u);
    check_list<int>(u, { 6, 5, 2, 1 });
    cc_list_delete(u);
  }

  SUBCASE("unique")
  {
    std::vector<int> x = { 1, 2, 2, 3, 3, 3, 4, 4, 4, 4 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    cc_list_unique(u);
    check_list<int>(u, { 1, 2, 3, 4 });
    cc_list_delete(u);
  }

  SUBCASE("sort")
  {
    std::vector<int> x = { 8, 7, 5, 9, 4, 0, 1, 3, 2, 6, 4 };
    cc_list_t u = cc_list_from_array(x.data(), x.size(), sizeof(int));
    cc_list_sort(u, less);
    check_list<int>(u, { 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9 });
    cc_list_delete(u);
  }
}

TEST_CASE("list comparison [atomic]")
{
  std::vector<long> x = { 1, 12, 123, 1234, 12345, 123456, 1234567 };
  std::vector<long> y = { 1, 12, 123, 1234, 12345, 123456, 1234567, 12345678 };
  std::vector<long> z = { 1, 12, 123, 1234, 12345, 123458, 1234567, 12345678 };

  cc_list_t a = cc_list_from_array(x.data(), x.size(), sizeof(long));
  cc_list_t b = cc_list_from_array(x.data(), x.size(), sizeof(long));
  cc_list_t c = cc_list_from_array(y.data(), y.size(), sizeof(long));
  cc_list_t d = cc_list_from_array(z.data(), z.size(), sizeof(long));

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
