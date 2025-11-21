# Day 8: Handheld Halting

## Part 1

More or less, just implement the simulated CPU with instructions. Execute while
keeping track of which instructions have already been executed (bool vector).
Stop and return when we hit one twice.

## Part 2

Modify to stop execution if we run off the end of the instructions (program).
Then go through each `nop` and `jmp` in order and try replacing them, re-run
the program and see if it looped or ran to the end.
