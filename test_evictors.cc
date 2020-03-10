#include "fifo_evictor.hh"
#include "lru_evictor.hh"
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

bool fifo_basic()
{
  auto e = FifoEvictor();
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  assert(e.evict() == "");
  for (unsigned long i = 0; i < s.size(); i++) {
    e.touch_key(s[i]);
  }
  for (unsigned long i = 0; i < s.size(); i++) {
    assert(e.evict() == s[i]);
  }
  assert(e.evict() == "");
  cout << "fifo_basic(success)\n";
  return true;
}

bool fifo_duplicates()
{
  auto e = FifoEvictor();
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  assert(e.evict() == "");
  for (unsigned long i = 0; i < s.size(); i++) {
    e.touch_key(s[i]);
  }
  for (unsigned long i = 0; i < s.size(); i++) {
    e.touch_key(s[i]);
  }
  for (unsigned long i = 0; i < s.size(); i++) {
    assert(e.evict() == s[i]);
  }
  for (unsigned long i = 0; i < s.size(); i++) {
    assert(e.evict() == s[i]);
  }
  assert(e.evict() == "");
  cout << "fifi_duplicates(success)\n";
  return true;
}

bool fifo_unsafe_key()
{
  FifoEvictor e{};
  assert(e.evict() == "");
  for (unsigned long i = 0; i < 20; i++) {
    e.touch_key(to_string(i));
  }
  for (unsigned long i = 0; i < 20; i++) {
    assert(e.evict() == to_string(i));
  }
  assert(e.evict() == "");
  cout << "fifi_unsafe_key(success)\n";
  return true;
}

bool lru_basic()
{
  LruEvictor e{};
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  assert(e.evict() == "");
  for (unsigned long i = 0; i < s.size(); i++) {
    e.touch_key(s[i]);
  }
  for (unsigned long i = 0; i < s.size(); i++) {
    assert(e.evict() == s[i]);
  }
  assert(e.evict() == "");
  cout << "lru_basic(success)\n";
  return true;
}

bool lru_juggle()
{
  LruEvictor e{};
  vector<char const *> s = {"foo", "bar", "baz", "bim", "bam", "bol"};
  assert(e.evict() == "");
  for (unsigned long i = 0; i < s.size(); i++) {
    e.touch_key(s[i]);
    e.touch_key(s[0]);
  }
  for (unsigned long i = 1; i < s.size(); i++) {
    assert(e.evict() == s[i]);
  }
  assert(e.evict() == s[0]);
  assert(e.evict() == "");
  cout << "lru_juggle(success)\n";
  return true;
}

int main()
{
  return !(fifo_basic() && fifo_duplicates() && fifo_unsafe_key() &&
           lru_basic() && lru_juggle());
}
