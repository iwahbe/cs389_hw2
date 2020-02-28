# cs389_hw2
Software Cache Implementation - Ian Wahbe (No partner - Eitan Approved)

## Part 1: basic cache operations (60%)
I have a `Cache` that links with `cache.hh`. See `cache_lib.cc` for more details.

## Part 2: Testing (not graded)
`test.cc` contains my unit tests. It is compiled by cmake. 

## Part 3: Performance (10%)
It's amazing how good template meta-programming is. I used the standard `std::unordered_map`,
as attempting to beat it's performance for a school assignment seemed unnecessary (not to 
mention) difficult. I default to the `std::hash` function, as required in `cache.hh`. 

## Part 4: Collision resolution (5%)
I maintain a bucket (collision) list. Not only does this still get $O(1)$ time, but it is
required by the standard. This makes it the best tested, safest approach, while still
running in constant time. 

## Part 5: Dynamic resizing (5%)
Again, using `std::unordered_map` instead of implementing my own hash-map makes this trivial.
I can simply set my maps `max_load_factor` to $0.75$ or any other value. Resizing is then 
taken care of for me. 
