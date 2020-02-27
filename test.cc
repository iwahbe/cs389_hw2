#include "cache.hh"
#include <iostream>
using namespace std;

// This will handle tests of the cache_lib and evictor_lib files

void set_string(Cache &c, string key, char const *val)
{
  c.set(key, val, strlen(val) + 1);
}

bool basic_insert()
{
  char const * should_be = "bar";
  Cache c(1024);
  set_string(c, "Foo", should_be);
  uint32_t size;
  const char *res = c.get("Foo", size);
  if (res != nullptr && strcmp(res, should_be)) {
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

int main()
{
  basic_insert();
  return 0;
}
