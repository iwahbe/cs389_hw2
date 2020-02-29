#include "lru_evictor.hh"

LruEvictor::~LruEvictor() {}

void LruEvictor::touch_key(const key_type &key)
{
  // removes old element from list
  std::shared_ptr<LruEvictor::ListNode> node;
  if ((del_in_list(key) = node) == nullptr) {
    // we didn't find a node
    // so we create a new one
    node = std::shared_ptr<LruEvictor::ListNode>(new LruEvictor::ListNode);
  }
  // reset node correctly
  node->next = nullptr;
  node->prev = tail;
  tail->next = node;
  tail = node.get();
  map.insert_or_assign(key, node);
}

// LruEvictor::evict: removes leading element from LList, returning it
//
// return const key_type: the least recently touched element

const key_type LruEvictor::evict()
{
  if (head == nullptr) {
    return nullptr;
  }
  auto node = del_in_list(head->key);
  map.erase(node->key);
  return node->key;
}

// LruEvictor::del_in_list: Deletes the node with `key` from the internal linked
// list. does not delete from the map. O(1)
//
// const key_type &key: the key to delete
//
// return: the node removed

std::shared_ptr<LruEvictor::ListNode>
LruEvictor::del_in_list(const key_type &key)
{
  auto found = map.find(key);
  if (found != map.end()) {
    // to get a shared_ptr instead of just a ptr
    auto node = found->second;
    if (node->prev == nullptr && node->next == nullptr) {
      // only node in list
      head = nullptr;
      tail = nullptr;
      return node;
    }
    else if (node->prev == nullptr) {
      // node at head of the list
      node->next->prev = nullptr;
      head = node->next;
      return node;
    }
    else if (node->next == nullptr) {
      // node at tail of the list
      tail = node->prev;
      tail->next = nullptr;
      return node;
    }
    else {
      // node in center of the list
      auto prev = node->prev;
      auto next = node->next;
      prev->next = next;
      next->prev = prev;
      return node;
    }
  }
  return nullptr;
}
