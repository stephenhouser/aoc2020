# Day 1: Report Repair

An easy day 1. A little optimization rather than cold hard brute force.

## Part 1

Sort the numbers. Then for each number (n) use binary search for the compliment
number (`2020 - n`) that will solve the problem. If we don't find it in the vector
just continue on. Otherwise, we are done.

## Part 2

More or less the same as part 1, added a nested loop for the second number (m).
Then we look for `2020 - n - m` in the vector. Same as before.
