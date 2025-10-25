# Day 7: Handy Haversacks

## Part 1

Create rules as a map from string to vector of pairs<string, int>. Invert
the map to give a set of rules for what each color can be contained within.
Then just follow the rules and collect the set of "parent" bags with the 
inverted rules.

## Part 2

Brute force recursive counting of the bags that must be in each bag.

## Notes

- Updated solution.cpp to use `std::print` instead of `cout`
- Updated Makefile, default target is test, live is now `input`
- Updated Makefile and added a `summary` target so the top level can `make summary` and get all the summaries.
