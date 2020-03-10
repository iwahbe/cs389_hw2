#include "cache.hh"
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include "fifo_evictor.hh"
#include "lru_evictor.hh"

using namespace std;

// This will handle tests of the cache_lib and evictor_lib files

void set_string(Cache &c, string key, char const *val)
{
  c.set(key, val, strlen(val) + 1);
}

bool basic_insert_plus_queary()
{
  char const *should_be = "bar";
  Cache c(1024);
  set_string(c, "Foo", should_be);
  uint32_t size;
  const char *res = c.get("Foo", size);
  if (size != strlen(should_be) + 1) {
    cout << "basic_insert(failure), size(" << size << ") expected("
         << strlen(should_be) + 1 << ")\n";
    return true;
  }
  if (res != nullptr && strcmp(res, should_be) == 0) {
    cout << "basic_insert(success)\n";
    return true;
  }
  else if (res != nullptr) {
    cout << "basic_insert(failure), found(" << res << "), should_be("
         << should_be << ")\n";
    return false;
  }
  else {
    cout << "basic_insert(failure), found(nullptr), should_be(" << should_be
         << ")\n";
    return false;
  }
}

bool empty_query()
{
  Cache c(1024);
  uint32_t size;
  if (c.get("baz", size) != nullptr || size != 0) {
    cout << "empty_query(failure) expected(nullptr) size(0) expected (" << size
         << ")\n";
    return false;
  }
  set_string(c, "foo", "bar");
  if (c.get("baz", size) != nullptr || size != 0) {
    cout << "empty_query2(failure) expected(nullptr)\n";
    return false;
  }
  if (c.get("foo", size) == nullptr || size != 4) {
    cout << "empty_query3(failure), expected(someptr)\n";
    return false;
  }
  if (c.get("baz", size) != nullptr || size != 0) {
    cout << "empty_query4(failure), expected(nullptr)\n";
    return false;
  }
  std::cout << "empty_query(success)\n";
  return true;
}

bool basic_delete()
{
  Cache c(1024);
  set_string(c, "foo", "bar");
  set_string(c, "foo2", "barz");
  set_string(c, "foo3", "baz");
  // 13 = 3 + 3 + 4 + (3 * \0)
  if (c.space_used() != 13) {
    cout << "basic_delete expected(13) found(" << c.space_used() << ")\n";
    return false;
  }
  if (!c.del("foo2")) {
    cout << "expected to find foo2";
    return false;
  }
  if (c.space_used() != 8) {
    cout << "basic_delete expected(8) found(" << c.space_used() << ")\n";
    return false;
  }
  c.reset();
  if (c.space_used() != 0) {
    cout << "basic_delete clear failed found(" << c.space_used() << ")\n";
    return false;
  }
  cout << "basic_delete(success)\n";
  return true;
}

bool duplicates()
{
  // can the cache handle duplicates
  Cache c(1024);
  //                         0      1      2      3      4      5
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};

  for (unsigned long x = 0; x < s.size(); x++) {
    for (int i = 0; i < 5; i++) {
      set_string(c, to_string(x), s[x]);
    }
  }
  assert(c.space_used() == 24);
  return true;
}

bool fifo_basic()
{
  auto e = FifoEvictor();
  Cache c(16, 0.75, &e);
  uint32_t four(4);
  //                         0      1      2      3      4      5
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  for (unsigned long i = 0; i < s.size(); i++) {
    set_string(c, to_string(i), s[i]);
  }
  for (int i = 0; i < 2; i++) {
    assert(c.get(to_string(i), four) == nullptr);
  }
  for (unsigned long i = 2; i < s.size(); i++) {
    assert(c.get(to_string(i), four) != nullptr);
  }
  if (c.space_used() > 16) {
    cout << "fifo_basic(failure) expected(space_used <= 16) "
            "found(space_used = "
         << c.space_used() << ")\n";
    return false;
  }

  cout << "fifo_evictor(success)\n";
  return true;
}

bool fifo_duplicates()
{
  FifoEvictor e{};
  Cache c(16, 0.75, &e);
  uint32_t klen;
  //                         0      1      2      3      4      5
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  vector<string> k = {"zero", "one", "two", "three", "four", "five"};
  for (unsigned long i = 0; i < 6; i++) {
    klen = static_cast<uint32_t>(k[i].length() + 1);
    c.get(k[i], klen);
    for (unsigned long j = 0; j < 1; j++) {
      set_string(c, k[i], s[i]);
    }
    c.get(k[i], klen);
  }
  for (int i = 0; i < 2; i++) {
    klen = static_cast<uint32_t>(k[i].length() + 1);
    if (c.get(k[i], klen) != nullptr) {
      cout << "fifo_duplicates(failure) expected(nullptr) on '" << k[i]
           << "' found(" << c.get(k[i], klen) << ")\n";
      return false;
    }
  }
  for (unsigned long i = 2; i < s.size(); i++) {
    klen = static_cast<uint32_t>(k[i].length() + 1);
    if (c.get(k[i], klen) == nullptr) {
      cout << "fifo_duplicates(failure) expected(someptr) on '" << k[i]
           << "'\n";
      return false;
    }
  }
  if (c.space_used() > 16) {
    cout << "fifo_duplicates(failure) expected(space_used <= 16) "
            "found(space_used = "
         << c.space_used() << ")\n";
    return false;
  }
  cout << "fifo_duplicates(success)\n";
  return true;
}

bool fifo_unsafe_key()
{
  // This ensures that the string copy for key works correctly
  // Otherwise the test will crash
  FifoEvictor e{};
  Cache c(16, 0.75, &e);
  uint32_t four(4);
  //                         0      1      2      3      4      5
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  for (int i = 0; i < 6; i++) {
    set_string(c, to_string(i), s[i]);
  }
  auto res = c.get(to_string(2), four);
  if (res == nullptr) {
    cout << "fifo_unsafe_key(failure) expected(someptr) found (nullptr)\n";
    return false;
  }
  if (c.space_used() > 16) {
    cout << "fifo_unsafe_key(failure) expected(space_used <= 16) "
            "found(space_used = "
         << c.space_used() << ")\n";
    return false;
  }
  return true;
}

bool lru_basic()
{
  LruEvictor e{};
  Cache c(16, 0.75, &e);
  uint32_t four(4);
  //                         0      1      2      3      4      5
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  for (int i = 0; i < 6; i++) {
    c.get(to_string(0), four);
    set_string(c, to_string(i), s[i]);
  }
  auto res = c.get(to_string(0), four);
  if (res == nullptr) {
    cout << "lru_basic(failure) expected(foo) found(nullptr)\n";
    return false;
  }
  if (c.space_used() > 16) {
    cout << "lru_basic_key(failure) expected(space_used <= 16) "
            "found(space_used = "
         << c.space_used() << ")\n";
    return false;
  }
  cout << "lru_basic(success)\n";
  return true;
}

bool lru_juggle()
{
  LruEvictor e{};
  Cache c(32, 0.75, &e);
  vector<const char *> foobarz = {"foo", "barz", "flube"};
  uint32_t four(4);
  uint32_t five(5);
  Cache::val_type got;
  set_string(c, "f", foobarz[0]);
  for (int i = 0; i < 50; i++) {
    if (strncmp((got = c.get("f", four)), foobarz[0], 4)) {
      cout << "lru_juggle(failure) expected(foo) found(" << got << ")\n";
      return false;
    }
    set_string(c, to_string(i), foobarz[2]);
    c.get("b", five);
  }
  set_string(c, "b", foobarz[1]);
  for (int i = 50; i < 100; i++) {
    if (strncmp((got = c.get("f", four)), foobarz[0], 4)) {
      cout << "lru_juggle(failure2) expected(foo) found(" << got << ")\n";
      return false;
    }
    set_string(c, to_string(i), foobarz[2]);
    if (strncmp((got = c.get("b", five)), foobarz[1], 5)) {
      cout << "lru_juggle(failure2) expected(" << foobarz[1] << ") found("
           << got << ")\n";
      return false;
    }
  }

  cout << "lru_juggle(success)\n";
  return true;
}


int main()
{
  // runs a series of tests
  return !(basic_insert_plus_queary() && empty_query() && basic_delete() &&
           fifo_basic() && fifo_duplicates() && fifo_unsafe_key() &&
           lru_basic() && lru_juggle());
}
