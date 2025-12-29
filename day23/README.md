# Day 23: Crab Cups

Linked list, iteration. Fixed-vector linked list.

## Part 1

Built a linked list to represent the ring of cups. Worked perfectly. A simple
iteration for the 100 moves.

## Part 2

Linked list was taking way too long for 1,000,000 cups and 10,000,000 moves.
The time to traverse the list when searching for the destination was huge.
Unlinking and re-linking however would be reasonable.

Converted to a fixed vector representation of the linked list where the index of
the vector is the cup number and the value at an index is the next cup in the
sequence from that cup. This made re-arrangement relatively easy. It's the same
"link pointer" updates but with O(n) lookup for destinations.
