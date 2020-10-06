/*
 *  cc - C Containers library
 *
 *  cc is, per 17 USC § 101, a work of the U.S. Government and is not subject to
 *  copyright protection in the United States.
 */

#include "doctest/doctest.h"
#include "cc.h"
#include "string.hpp"

std::string
to_string(const cc_string_t s)
{
  return std::string(cc_string_data(s), cc_string_size(s));
}

TEST_SUITE_BEGIN("strings");

TEST_CASE("string construction")
{
  SUBCASE("empty string")
  {
    cc_string_t s = cc_string_new();
    CHECK(cc_string_size(s) == 0);
    cc_string_delete(s);
  }

  SUBCASE("create from a character string")
  {
    cc_string_t s = cc_string_from_chars("Hello, world!", 13);
    CHECK(cc_string_size(s) == 13);
    CHECK(to_string(s) == "Hello, world!");
    cc_string_delete(s);
  }

  SUBCASE("copy")
  {
    cc_string_t s = cc_string_from_chars("Hello, world!", 13);
    cc_string_t t = cc_string_copy(s);
    CHECK(cc_string_size(t) == 13);
    CHECK(to_string(t) == "Hello, world!");
    cc_string_delete(s);
    cc_string_delete(t);
  }
}

TEST_CASE("string assignment")
{
  cc_string_t s = cc_string_new();
  cc_string_assign(s, 4, '#');
  CHECK(cc_string_size(s) == 4);
  CHECK(to_string(s) == "####");
  cc_string_delete(s);
}

TEST_CASE("string element access")
{
  cc_string_t s = cc_string_from_chars("Hello, world!", 13);

  SUBCASE("get")
  {
    CHECK(cc_string_get(s, 1) == 'e');
    CHECK(cc_string_get(s, 3) == 'l');
    CHECK(cc_string_get(s, 5) == ',');
    CHECK(cc_string_get(s, 7) == 'w');
    CHECK(cc_string_get(s, 9) == 'r');
  }

  SUBCASE("front")
  {
    CHECK(cc_string_front(s) == 'H');
  }

  SUBCASE("back")
  {
    CHECK(cc_string_back(s) == '!');
  }

  SUBCASE("data")
  {
    const char* data = cc_string_data(s);
    CHECK(data[1] == 'e');
    CHECK(data[3] == 'l');
    CHECK(data[5] == ',');
    CHECK(data[7] == 'w');
    CHECK(data[9] == 'r');
  }

  cc_string_delete(s);
}

TEST_CASE("string iteration")
{
  std::string a = "1223334444";
  cc_string_t s = cc_string_from_chars(a.data(), a.size());

  cc_string_iterator_t p = cc_string_begin(s);
  cc_string_iterator_t e = cc_string_end(s);
  for (size_t n = 0; n < a.size(); ++n)
  {
    REQUIRE(cc_string_iterator_ne(p, e));
    CHECK(*cc_string_iterator_dereference(p) == a[n]);
    cc_string_iterator_increment(&p);
  }
  REQUIRE(cc_string_iterator_eq(p, e));

  cc_string_delete(s);
}

TEST_CASE("string capacity")
{
  cc_string_t s = cc_string_new();
  cc_string_t t = cc_string_from_chars("ABCDEFGHIJKLMNOPQRSTUVWXYZ01234", 31);
  cc_string_t u = cc_string_from_chars("The quick brown fox jumped over the lazy dog.", 45);

  SUBCASE("empty")
  {
    CHECK(cc_string_empty(s));
    CHECK(!cc_string_empty(t));
    CHECK(!cc_string_empty(u));
  }

  SUBCASE("size")
  {
    CHECK(cc_string_size(s) == 0);
    CHECK(cc_string_size(t) == 31);
    CHECK(cc_string_size(u) == 45);
  }

  SUBCASE("capacity")
  {
    CHECK(cc_string_capacity(s) == 15);
    CHECK(cc_string_capacity(t) == 31);
    CHECK(cc_string_capacity(u) == 63);

    cc_string_reserve(s, 16);
    CHECK(cc_string_capacity(s) == 31);

    cc_string_push_back(s, '#');
    CHECK(cc_string_capacity(s) == 31);

    cc_string_push_back(t, '#');
    CHECK(cc_string_capacity(t) == 63);

    cc_string_push_back(u, '#');
    CHECK(cc_string_capacity(u) == 63);

    cc_string_shrink_to_fit(t);
    CHECK(cc_string_capacity(t) == 32);

    cc_string_shrink_to_fit(u);
    CHECK(cc_string_capacity(u) == 46);
  }

  cc_string_delete(s);
  cc_string_delete(t);
  cc_string_delete(u);
}

TEST_CASE("string modification")
{
  cc_string_t s = cc_string_new();
  cc_string_t t = cc_string_from_chars("Hello, world!", 13);

  SUBCASE("clear")
  {
    cc_string_clear(s);
    cc_string_clear(t);
    CHECK(cc_string_empty(s));
    CHECK(cc_string_empty(t));
  }

  SUBCASE("insert")
  {
    cc_string_t u;

    cc_string_insert(s, 0, "###", 3);
    CHECK(cc_string_size(s) == 3);
    CHECK(to_string(s) == "###");

    u = cc_string_copy(t);
    cc_string_insert(u, 0, "---", 3);
    CHECK(cc_string_size(u) == 16);
    CHECK(to_string(u) == "---Hello, world!");
    cc_string_delete(u);

    u = cc_string_copy(t);
    cc_string_insert(u, 6, "---", 3);
    CHECK(cc_string_size(u) == 16);
    CHECK(to_string(u) == "Hello,--- world!");
    cc_string_delete(u);

    cc_string_insert(t, cc_string_size(t), "---", 3);
    CHECK(cc_string_size(t) == 16);
    CHECK(to_string(t) == "Hello, world!---");
  }

  SUBCASE("erase")
  {
    cc_string_erase(t, 3, 9);
    CHECK(cc_string_size(t) == 7);
    CHECK(to_string(t) == "Helrld!");
  }

  SUBCASE("push back")
  {
    cc_string_push_back(s, '#');
    cc_string_push_back(t, '#');
    CHECK(cc_string_size(s) == 1);
    CHECK(cc_string_size(t) == 14);
    CHECK(cc_string_back(s) == '#');
    CHECK(cc_string_back(t) == '#');
  }

  SUBCASE("pop back")
  {
    cc_string_pop_back(t);
    CHECK(cc_string_size(t) == 12);
    CHECK(cc_string_back(t) == 'd');
  }

  SUBCASE("resize")
  {
    cc_string_resize(s, 3, '#');
    cc_string_resize(t, 3, '#');
    CHECK(cc_string_size(s) == 3);
    CHECK(cc_string_size(t) == 3);
    CHECK(to_string(s) == "###");
    CHECK(to_string(t) == "Hel");

    cc_string_resize(s, 6, '%');
    cc_string_resize(t, 6, '%');
    CHECK(cc_string_size(s) == 6);
    CHECK(cc_string_size(t) == 6);
    CHECK(to_string(s) == "###%%%");
    CHECK(to_string(t) == "Hel%%%");
  }

  SUBCASE("swap")
  {
    cc_string_swap(s, t);
    CHECK(cc_string_size(s) == 13);
    CHECK(cc_string_size(t) == 0);
    CHECK(to_string(s) == "Hello, world!");
    CHECK(to_string(t) == "");
  }

  cc_string_delete(s);
  cc_string_delete(t);
}

TEST_CASE("string operations")
{
  cc_string_t s = cc_string_from_chars("The cat stretched.", 18);
  cc_string_t t = cc_string_from_chars("She opened the door.", 20);

  SUBCASE("append")
  {
    cc_string_append(s, " She meowed.", 12);
    CHECK(to_string(s) == "The cat stretched. She meowed.");
  }

  SUBCASE("compare")
  {
    cc_string_t u = cc_string_copy(t);
    CHECK(cc_string_compare(s, t) != 0);
    CHECK(cc_string_compare(t, u) == 0);
    cc_string_delete(u);
  }

  SUBCASE("start/end with")
  {
    CHECK(cc_string_starts_with(s, "The cat"));
    CHECK(!cc_string_starts_with(t, "The cat"));
    CHECK(cc_string_ends_with(s, "stretched."));
    CHECK(!cc_string_ends_with(t, "stretched."));
  }

  SUBCASE("substring")
  {
    cc_string_t u = cc_string_substr(s, 4, 3);
    CHECK(to_string(u) == "cat");
    cc_string_delete(u);
  }

  SUBCASE("replace")
  {
    cc_string_replace(s, 4, 3, "elephant", 8);
    CHECK(to_string(s) == "The elephant stretched.");
  }

  cc_string_delete(s);
  cc_string_delete(t);
}

TEST_CASE("string search")
{
  cc_string_t s = cc_string_from_chars("The quick brown fox jumped.", 27);
  cc_string_t t = cc_string_from_chars("one two three one two three", 27);

  SUBCASE("find")
  {
    CHECK(cc_string_find(s, "T", 0, 1) == 0);
    CHECK(cc_string_find(s, ".", 0, 1) == 26);
    CHECK(cc_string_find(s, "quick brown", 0, 11) == 4);
    CHECK(cc_string_find(s, "quick brown", 4, 11) == 4);
    CHECK(cc_string_find(s, "quick brown", 5, 11) == cc_npos);
    CHECK(cc_string_find(s, "quick orange", 0, 12) == cc_npos);

    CHECK(cc_string_find(t, "one", 0, 3) == 0);
    CHECK(cc_string_find(t, "two", 0, 3) == 4);
    CHECK(cc_string_find(t, "three", 0, 5) == 8);
  }

  SUBCASE("reverse find")
  {
    CHECK(cc_string_rfind(s, "T", 0, 1) == 0);
    CHECK(cc_string_rfind(s, ".", 0, 1) == 26);
    CHECK(cc_string_rfind(s, "quick brown", 0, 11) == 4);
    CHECK(cc_string_rfind(s, "quick brown", 4, 11) == 4);
    CHECK(cc_string_rfind(s, "quick brown", 5, 11) == cc_npos);
    CHECK(cc_string_rfind(s, "quick orange", 0, 12) == cc_npos);

    CHECK(cc_string_rfind(t, "one", 0, 3) == 14);
    CHECK(cc_string_rfind(t, "two", 0, 3) == 18);
    CHECK(cc_string_rfind(t, "three", 0, 3) == 22);
  }

  SUBCASE("first of")
  {
    CHECK(cc_string_find_first_of(s, "abc", 0, 3) == 7);
    CHECK(cc_string_find_first_of(s, "abc", 8, 3) == 10);
    CHECK(cc_string_find_first_of(s, "agl", 0, 3) == cc_npos);
  }

  SUBCASE("find not of")
  {
    CHECK(cc_string_find_first_not_of(s, "abc", 0, 3) == 0);
    CHECK(cc_string_find_first_not_of(s, "abc", 7, 3) == 8);
    CHECK(cc_string_find_first_not_of(s, "The ", 0, 4) == 4);
    CHECK(cc_string_find_first_not_of(s, "brown", 10, 5) == 15);
  }

  SUBCASE("last of")
  {
    CHECK(cc_string_find_last_of(s, "abc", cc_npos, 3) == 10);
    CHECK(cc_string_find_last_of(s, "abcd", cc_npos, 4) == 25);
    CHECK(cc_string_find_last_of(s, "abcd", 20, 4) == 10);
    CHECK(cc_string_find_last_of(s, "agl", cc_npos, 3) == cc_npos);
  }

  SUBCASE("last not of")
  {
    CHECK(cc_string_find_last_not_of(s, "abc", cc_npos, 3) == 26);
    CHECK(cc_string_find_last_not_of(s, "abc", 11, 3) == 9);
    CHECK(cc_string_find_last_not_of(s, " jumped.", cc_npos, 8) == 18);
    CHECK(cc_string_find_last_not_of(s, "fox", 19, 3) == 15);
  }

  cc_string_delete(s);
  cc_string_delete(t);
}

TEST_SUITE_END();
