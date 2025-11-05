# Day 17: Conway Cubes 

## Part 1

3D Conway's Game of Life. i
- Start with 2D plane with `#` as active and `.` as inactive
- Evaluate all 26 3D neighbors
- if active and 2 or 3 neighbors, remain active, else inactive
- if inactive and 3 neighbors, become active, else inactive

Run for 6 generateions, count number of active cubes.

Expect part 2 to be for more generations (bigger map)

- Make active a `unordered_set` of 3D `point`s.
- Track `bounds` of universe
- How to trim evaluations:
 - need only look around active sites, all neighbors of active
 - make set of "to be evaluated" from neighbors of active sites

General Idea:

```
current = vector of active sites (point_t)
while (generation++ < generations) {
    active_neighbors = neighbors_of(current)
    next = next_state(current, active_neighbors)
    current = next
}
```

## Part 2

Well then. 4 dimensional space. That's a twist. I think at least my sparse
approach will be useful and the `point_t` struct already supports a `w`.
Though not for hashing and putting in an `unordered_set`.

With only 6 cycles (generations) the board will be small enough, change
the hash function to hash in the `w` with 16 bits for each dimension.

The only significant change was to add a 4D `get_neighbors` that would generate
the list of possible neighbors in 4D space. Used a global function pointer
to hold what `get_neighbors` to call for part 1 vs part 2.

Update the `point_t` to really use the `w` dimension.