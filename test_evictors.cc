#include "cache.hh"
#include "fifo_evictor.hh"
#include "lru_evictor.hh"
#include <iostream>
#include <vector>
using namespace std;

void set_string(Cache &c, string key, char const *val)
{
  c.set(key, val, strlen(val) + 1);
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
  auto res = c.get(s[2], four);
  if (res == nullptr) {
    cout << "fifo_unsafe_key(failure) expected(someptr) found (" << res
         << ")\n";
    return false;
  }
  return true;
}

int main() { return !(fifo_basic() && fifo_duplicates() && fifo_unsafe_key()); }
