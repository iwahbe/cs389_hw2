#include "cache.hh"
#include <unordered_map>

class Cache::Impl {
    public:
  Impl(Cache::size_type maxmem, float max_load_factor, Evictor *evictor,
       Cache::hash_func hasher)
      : maxmem(maxmem), max_load_factor(max_load_factor), evictor(evictor),
        hasher(hasher), current_mem(0)
  {
  }
  ~Impl() { delete evictor; }

  Cache::size_type maxmem;
  float max_load_factor;
  Evictor *evictor;
  Cache::hash_func hasher;
  std::unordered_map<key_type, Cache::val_type> val_map;
  std::unordered_map<key_type, Cache::size_type> size_map;
  Cache::size_type current_mem;
};

Cache::Cache(Cache::size_type maxmem, float max_load_factor, Evictor *evictor,
             Cache::hash_func hasher)
    : pImpl_(new Cache::Impl(maxmem, max_load_factor, evictor, hasher))
{
}

Cache::~Cache() {}

// Cache::set: Adds a value to the cache
//
// key_type key: the key
// Cache::val_type val: the value
// Cache::size_type size: the size of the value in size_type

void Cache::set(key_type key, Cache::val_type val, Cache::size_type size)
{
  // TODO: implement deep copy
  Cache::size_type current = 0;
  get(key, current);
  auto newsize = size + pImpl_->current_mem - current;
  if (newsize > pImpl_->maxmem) {
    // TODO: this is where the eviction policy takes place
    return;
  }
  pImpl_->size_map.insert_or_assign(key, size);
  pImpl_->val_map.insert_or_assign(key, val);
}

// Cache::get: retrieve a value from the cache
//
// key_type key: a key
// Cache::size_type &val_size: where to place the size of the associated value
//
// return Cache::val_type: the value associated with key

Cache::val_type Cache::get(key_type key, Cache::size_type &val_size) const
{
  auto search_size = pImpl_->size_map.find(key);
  if (search_size != pImpl_->size_map.end()) {
    val_size = search_size->second;
    return pImpl_->val_map.find(key)->second;
  }
  else {
    return nullptr;
  }
}

// Cache::del: Delete a value from the cache
//
// key_type key: a key
//
// return bool: if the value was found

bool Cache::del(key_type key)
{
  auto found = pImpl_->size_map.find(key);
  if (found != pImpl_->size_map.end()) {
    pImpl_->current_mem -= found->second;
    // we have found one
    pImpl_->size_map.erase(key);
    pImpl_->val_map.erase(key);
    return true;
  }
  else {
    return false;
  }
}

// Cache::space_used: The amount of memory used by the cache

Cache::size_type Cache::space_used() const {
  return pImpl_->current_mem;
}

// Cache::reset: Removes all elements from the cache

void Cache::reset()
{
  pImpl_->current_mem = 0;
  pImpl_->size_map.clear();
  pImpl_->val_map.clear();
}
