#include "cache.hh"
#include "fifo_evictor.hh"
#include "lru_evictor.hh"
#include <cassert>
#include <catch2/catch.hpp>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

// This will handle tests of the cache_lib and evictor_lib files
void set_string(Cache &c, string key, char const *val)
{
  c.set(key, val, strlen(val) + 1);
}

TEST_CASE("plain cache", "[cache]")
{
  Cache c(1024);
  uint32_t size;
  SECTION("basic insertion")
  {
    char const *should_be = "bar";

    set_string(c, "Foo", should_be);
    const char *res = c.get("Foo", size);
    REQUIRE(size == strlen(should_be) + 1);
    REQUIRE(strcmp(res, should_be) == 0);
  }

  SECTION("empty query")
  {
    REQUIRE(c.get("baz", size) == nullptr);
    REQUIRE(size == 0);

    set_string(c, "foo", "bar");

    REQUIRE(c.get("baz", size) == nullptr);
    REQUIRE(size == 0);

    REQUIRE(c.get("foo", size) != nullptr);
    REQUIRE(size == 4);

    REQUIRE(c.get("baz", size) == nullptr);
    REQUIRE(size == 0);
  }

  SECTION("basic delete")
  {
    set_string(c, "foo", "bar");
    set_string(c, "foo2", "barz");
    set_string(c, "foo3", "baz");
    REQUIRE(c.space_used() == 13);
    REQUIRE(c.del("foo2") == true);
    REQUIRE(c.space_used() == 8);
    REQUIRE(c.get("foo2", size) == nullptr);
    REQUIRE(c.del("bar") == false);
  }

  SECTION("reset")
  {
    vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};

    for (unsigned long x = 0; x < s.size(); x++) {

      set_string(c, to_string(x), s[x]);
    }
    c.reset();
    REQUIRE(c.space_used() == 0);
    REQUIRE(c.get("bol", size) == nullptr);
  }

  SECTION("duplicates")
  {
    vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};

    for (unsigned long x = 0; x < s.size(); x++) {
      for (int i = 0; i < 5; i++) {
        set_string(c, to_string(x), s[x]);
      }
    }
    REQUIRE(c.space_used() == 24);
  }
}

TEST_CASE("cache integration", "[cache]")
{
  SECTION("fifo integration")
  {
    FifoEvictor e;
    Cache c(16, 0.75, &e);
    uint32_t size;
    SECTION("basic")
    {
      vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
      for (unsigned long i = 0; i < s.size(); i++) {
        set_string(c, to_string(i), s[i]);
      }
      for (int i = 0; i < 2; i++) {
        REQUIRE(c.get(to_string(i), size) == nullptr);
      }
      for (unsigned long i = 2; i < s.size(); i++) {
        REQUIRE(strcmp(c.get(to_string(i), size), s[i]) == 0);
      }
      REQUIRE(c.space_used() <= 16);
    }

    SECTION("duplicates")
    {

      vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
      vector<string> k = {"zero", "one", "two", "three", "four", "five"};
      for (unsigned long i = 0; i < 6; i++) {
        c.get(k[i], size);
        for (unsigned long j = 0; j < 1; j++) {
          set_string(c, k[i], s[i]);
        }
        c.get(k[i], size);
      }
      for (int i = 0; i < 2; i++) {
        REQUIRE(c.get(k[i], size) == nullptr);
      }
      for (unsigned long i = 2; i < s.size(); i++) {
        REQUIRE(c.get(k[i], size) != nullptr);
      }
      REQUIRE(c.space_used() <= 16);
    }

    SECTION("unsafe key")
    {
      vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
      for (int i = 0; i < 6; i++) {
        set_string(c, to_string(i), s[i]);
      }
      auto res = c.get(to_string(2), size);
      REQUIRE(res != nullptr);
    }
  }

  SECTION("lru integration")
  {
    LruEvictor e{};
    Cache c(16, 0.75, &e);
    uint32_t size;

    SECTION("basic")
    {
      vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
      for (int i = 0; i < 6; i++) {
        c.get(to_string(0), size);
        set_string(c, to_string(i), s[i]);
      }
      REQUIRE(c.get(to_string(0), size) != nullptr);
      REQUIRE(c.get(to_string(1), size) == nullptr);
    }

    SECTION("juggle")
    {
      Cache::val_type got;
      vector<const char *> foobarz = {"foo", "barz", "flube"};
      set_string(c, "f", foobarz[0]);
      for (int i = 0; i < 50; i++) {
        REQUIRE(strncmp((got = c.get("f", size)), foobarz[0], size) == 0);
        set_string(c, to_string(i), foobarz[2]);
        c.get("b", size);
      }
      set_string(c, "b", foobarz[1]);
      for (int i = 50; i < 100; i++) {
        REQUIRE(strncmp((got = c.get("f", size)), foobarz[0], size) == 0);
        set_string(c, to_string(i), foobarz[2]);
        REQUIRE(strncmp((got = c.get("b", size)), foobarz[1], size) == 0);
      }
    }
  }
}
