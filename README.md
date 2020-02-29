# cs389_hw2
Software Cache Implementation - Ian Wahbe (No partner - Eitan Approved)

## Part 1: basic cache operations (60%)
I have a `Cache` that links with `cache.hh`. See `cache_lib.cc` for more details.

## Part 2: Testing (not graded)
`test_cache.cc` contains my unit tests for the raw cache (without an evictor). `test_evictors.cc`
has my evictor unit tests. Both can be run with `ctest` after compiling. 

## Part 3: Performance (10%)
It's amazing how good template meta-programming is. I used the standard `std::unordered_map`,
as attempting to beat it's performance for a school assignment seemed unnecessary (not to 
mention) difficult. I default to the `std::hash` function, as required in `cache.hh`.

## Part 4: Collision resolution (5%)
I (using the stl) maintain a bucket (collision) list. Not only does this still get $O(1)$ time, 
but it is required by the standard. This makes it the best tested, safest approach, while still
running in constant time. 

## Part 5: Dynamic resizing (5%)
Again, using `std::unordered_map` instead of implementing my own hash-map makes this trivial.
I can simply set my maps `max_load_factor` to $0.75$ or any other value. Resizing is then 
taken care of by the standard template library. 

## Part 6: Eviction policy (20%)
This is managed by the `std::queue` abstraction. A touch simply adds to the queue, and evict
pops the front of the queue.

## Part 7: LRU (10% extra credit)
For my LRU Evictor (see `lru_evictor.hh`, `lru_evictor.cc`), I maintain a hybrid data structure
composed of a map and a doubly linked list to provide the necessary $O(1)$ guarantees.  Ordering
is provided by the linked list. Note: a linked list has constant time `pop_front` and `push_back`.
To deal with updates in $O(1)$ time, I map into the nodes of my linked list. If `touch_key` is 
called on a node already in the linked list, it is found with the map, then removed and appended
to the back of the list.
