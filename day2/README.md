# Day 2: Password Philosophy

## Part 1

A brute force, parse all the rules and passwords, then just check they meet the criteria. Nothing special.

## Part 2

Again, nothing special other than a reminder to read the directions to actually get the rules right the first time. Had 2 failed submissions for not reading or interpreting the new rules correctly.


## Notes

Using the new `split()` worked pretty well for this problem. I remain curious how the alternate versions (in `split.cpp`) perform compared to the straight forward loop that is the default. E.g. the functional versions using filter and transform. Might be nice to make a `split()` that takes a vector describing the types of each to return. E.g. build the vector directly therein. This would be more like the one that takes a lambda -- which I removed :-(