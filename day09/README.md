# Day 9: Encoding Error

## Part 1

For each position and `target` number past the preamble, create a set of
numbers that includes all the numbers in the "window" (preamble).
For each number in the set `n` if the set contains the number `target - n`.
If so, `target` is valid, move on to the next position and target number
until we find one that is not valid.

## Part 2

Find the first invalid number from part 1. Iterate over the list, from the
first number to see of there is a 2+ number sequence from that position that
equals the invalid number. Use a `minmax` function to get the min and max
numbers in the range where the sum was found.