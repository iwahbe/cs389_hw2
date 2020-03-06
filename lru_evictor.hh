#pragma once

#include "evictor.hh"
#include <unordered_map>
#include <memory>
#include <cassert>

class LruEvictor : public Evictor {
    public:
  LruEvictor() = default;
  ~LruEvictor() override;
  void touch_key(const key_type &key) override;
  const key_type evict() override;

    protected:
  // Theory: We want the constant time inserts and
  // deletes of a linked list, but also the constant
  // time access of a map. We combine these, storing
  // the most recently used keys in a linked list, and
  // indexing into the list with a map.
  struct ListNode {
    key_type key;
    std::shared_ptr<ListNode> next;
    ListNode *prev;
  };
  std::shared_ptr<LruEvictor::ListNode> del_in_list(const key_type &key);
  std::unordered_map<key_type, std::shared_ptr<ListNode>> map;
  std::shared_ptr<ListNode> head;
  ListNode *tail;
};
