#include "fifo_evictor.hh"

FifoEvictor::~FifoEvictor() {}

void FifoEvictor::touch_key(const key_type &key) { queue.push(key); }

const key_type FifoEvictor::evict()
{
  auto ret = queue.front();
  queue.pop();
  return ret;
}
