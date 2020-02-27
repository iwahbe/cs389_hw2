#include "cache.hh"
#include <unordered_map>

class Cache::Impl {
    public:
  Impl(Cache::size_type maxmem, float max_load_factor, Evictor *evictor,
       hash_func hasher);

    protected:
  Cache::size_type maxmem;
  float max_load_factor;
  Evictor *evictor;
  hash_func hasherl;
};

Cache::Impl::Impl(Cache::size_type maxmem, float max_load_factor,
                  Evictor *evictor, hash_func hasher)
    : maxmem(maxmem), max_load_factor(max_load_factor), evictor(evictor),
      hasherl(hasher)

{
}

Cache::Cache(Cache::size_type maxmem, float max_load_factor, Evictor *evictor,
             hash_func hasher)
{

  pImpl_ = std::unique_ptr<Cache::Impl>(
      new Cache::Impl(maxmem, max_load_factor, evictor, hasher));
}

Cache::~Cache() {}

void Cache::set(key_type key, val_type val, Cache::size_type size)
{
  // TODO: should do something
}

Cache::val_type Cache::get(key_type key, Cache::size_type &val_size) const
{
  return nullptr;
}

bool Cache::del(key_type key) { return false; }

Cache::size_type Cache::space_used() const { return 0; }

void Cache::reset()
{
  // TODO: should be something
}

