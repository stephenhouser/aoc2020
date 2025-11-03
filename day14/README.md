# Day 14: Docking Data


## Part 1

An instruction simulation. Decode the instructions and run them through the
simulator. With the quirk that there's a mask in the logic unit that gets set
and the values put in memory are modified by it. Ignore `X` in the mask.

## Part 2

A weird twist. The `set_memory` in the second part is different that in the first.
Now we use the `X` in the mask to represent `1` or `0` but in the address not
the value. This will generate a ton of addresses to set. Using a recursive function;
if there are no `X` in the mask, set the address (with the `1` mask on the address),
if there are `X` then recurse with the bit set and the bit clear and the mask updated to have that `X` removed. Too many recursions in the sample input. Could I solve this iteratively and 
not have the recursion depth problem?
