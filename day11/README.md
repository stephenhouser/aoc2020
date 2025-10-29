# Day 11: Seating System

Cellular Automota

## Part 1

Put together the cellular automota as a `charmap_t` (my old character map class).
Write the `next_state` function that iterates the world from the current to next
state. It uses a next_seat function to count neighbors and returns the next seat
value based on the rules.

Iterate until the map/world stops changing, e.g. stable state.

## Part 2

Really this was just writing a new next_seat function that evaluates each seat
to count the closest seat in each direction we can see. Again the `charmap_t`
was handy in that it has some `is_valid()` functions to let me iterate over
them quickly.

