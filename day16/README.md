# Day 16: Ticket Translation

## Part 1

Noodled around a while after solving trying to make it more functional.

Finally settled on a transform to transform a ticket (vector of numbers) to
a vector of invalid numbers (e.g. ones that do not match any field). This was
used as a filter to get any of these that were non-zero length (had any invalid
numbers). Use join to flatten the vector of vectors into a single vector. Then
just add them up with reduce.

## Part 2

OMG this one was ugly. Took quite a while as I kept getting field positions
and ticket positions mixed up in my head.

First solution:
Filter the ticket list for valid tickets.
Run through the valid tickets (except the first one, mine).
For each number in the positions, determine the possible fields it could belong to, add these to a set and intersect that set with the previous possible fields (for that position).
This results in a vector (for each position) of sets (of possible fields this position could represent).
Next, deduce the fields that must belong to each position by looking for a position that has only one possibility.
Add this as a known mapping (position to field).
Remove this as a possibility from all other positions.
Loop until all the positions have been mapped.
Compute the product of the numbers on my ticket that are in positions that start with "departure".

