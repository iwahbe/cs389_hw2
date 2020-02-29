#include "cache.hh"
#include "fifo_evictor.hh"
#include "lru_evictor.hh"
#include <iostream>
#include <vector>
using namespace std;

uint32_t set_string(Cache &c, string key, char const *val)
{
  auto size = strlen(val) + 1;
  c.set(key, val, size);
  return size;
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
  return !(fifo_basic() && fifo_duplicates() && fifo_unsafe_key() &&
           lru_basic() && lru_juggle());
}
