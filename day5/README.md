# Day 5: Binary Boarding

## Part 1

Decoding the passes was the only tricky part. Use templates to create a `map()` function
that can take a function as a parameter and map from a vector of one type to antother.
The just use the decoder as the function. Whamo. Now use `max()` to get the largest
value out of the result vector.

## Part 2

Not a lot more complex. Use the same logic to map from passes to seat ids, then use
`sort()` to sort in ascending order. Last, a while loop to look for the gap and return
the missed number.


A good refinement of the `reduce()` and new `map()` functions for patterns.

Also using `std::print()` for debugging. Need to move to that for more.
