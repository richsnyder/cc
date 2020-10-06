/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include "doctest/doctest.h"
#include "cc.h"
#include "string.hpp"
#include "vector.hpp"

TEST_SUITE_BEGIN("nested containers");

TEST_CASE("list of strings")
{
  cc_list_t a = cc_list_new_f(cc_string_sizeof, cc_string_functions);

  cc_string_t u = cc_string_from_chars("Hello, world!", 13);
  cc_string_t v = cc_string_from_chars("Hallo Welt!", 11);
  cc_string_t w = cc_string_from_chars("Bonjour le monde!", 17);

  cc_list_push_back(a, u);
  cc_list_push_back(a, v);
  cc_list_push_back(a, w);

  cc_string_delete(u);
  cc_string_delete(v);
  cc_string_delete(w);

  REQUIRE(cc_list_size(a) == 3);

  cc_list_iterator_t p = cc_list_begin(a);
  cc_list_iterator_t e = cc_list_end(a);

  REQUIRE(cc_list_iterator_ne(p, e));
  u = (cc_string_t) cc_list_iterator_dereference(p);
  cc_list_iterator_increment(&p);
  REQUIRE(cc_list_iterator_ne(p, e));
  v = (cc_string_t) cc_list_iterator_dereference(p);
  cc_list_iterator_increment(&p);
  REQUIRE(cc_list_iterator_ne(p, e));
  w = (cc_string_t) cc_list_iterator_dereference(p);
  cc_list_iterator_increment(&p);
  REQUIRE(cc_list_iterator_eq(p, e));

  CHECK(to_string(u) == "Hello, world!");
  CHECK(to_string(v) == "Hallo Welt!");
  CHECK(to_string(w) == "Bonjour le monde!");

  cc_list_delete(a);
}

TEST_CASE("vector of vectors")
{
  cc_vector_t a = cc_vector_new_f(cc_vector_sizeof, cc_vector_functions);

  std::vector<int> x = { 1, 2, 3, 4, 5 };
  std::vector<int> y = { 1, 4, 9, 16, 25 };
  std::vector<int> z = { 1, 8, 27, 64, 125 };

  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(int));
  cc_vector_t v = cc_vector_from_array(y.data(), y.size(), sizeof(int));
  cc_vector_t w = cc_vector_from_array(z.data(), z.size(), sizeof(int));

  cc_vector_push_back(a, u);
  cc_vector_push_back(a, v);
  cc_vector_push_back(a, w);

  cc_vector_delete(u);
  cc_vector_delete(v);
  cc_vector_delete(w);

  REQUIRE(cc_vector_size(a) == 3);

  u = (cc_vector_t) cc_vector_get(a, 0);
  v = (cc_vector_t) cc_vector_get(a, 1);
  w = (cc_vector_t) cc_vector_get(a, 2);

  check_vector(u, x);
  check_vector(v, y);
  check_vector(w, z);

  cc_vector_delete(a);
}

TEST_CASE("map of strings to vectors")
{
  cc_map_t a = cc_map_new_f(
      cc_string_sizeof,
      cc_vector_sizeof,
      cc_string_functions,
      cc_vector_functions
    );

  std::vector<int> x = { 1, 2, 3, 4, 5 };
  std::vector<int> y = { 1, 4, 9, 16, 25 };
  std::vector<int> z = { 1, 8, 27, 64, 125 };

  cc_string_t i = cc_string_from_chars("abc", 3);
  cc_string_t j = cc_string_from_chars("ABC", 3);
  cc_string_t k = cc_string_from_chars("123", 3);

  cc_vector_t u = cc_vector_from_array(x.data(), x.size(), sizeof(int));
  cc_vector_t v = cc_vector_from_array(y.data(), y.size(), sizeof(int));
  cc_vector_t w = cc_vector_from_array(z.data(), z.size(), sizeof(int));

  cc_map_insert(a, i, u);
  cc_map_insert(a, j, v);
  cc_map_insert(a, k, w);

  cc_vector_delete(u);
  cc_vector_delete(v);
  cc_vector_delete(w);

  REQUIRE(cc_map_size(a) == 3);

  u = (cc_vector_t) cc_map_find(a, i);
  v = (cc_vector_t) cc_map_find(a, j);
  w = (cc_vector_t) cc_map_find(a, k);

  check_vector(u, x);
  check_vector(v, y);
  check_vector(w, z);

  cc_string_delete(i);
  cc_string_delete(j);
  cc_string_delete(k);

  cc_map_delete(a);
}

TEST_SUITE_END();
