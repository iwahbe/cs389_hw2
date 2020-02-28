#pragma once

#include "evictor.hh"

class LruEvictor : public Evictor {
    public:
  LruEvictor() = default;
  ~LruEvictor() override;
  void touch_key(const key_type &key) override;
  const key_type evict() override;

    protected:
  // unknown
};
