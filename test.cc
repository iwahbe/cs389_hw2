#include "cache.hh"
#include "fifo_evictor.hh"
#include <iostream>
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

int main()
{
  basic_insert_plus_queary();
  empty_query();
  basic_delete();
  return 0;
}
