# Day 15: Rambunctious Recitation


## Part 1

Brute force. Put the seed numbers into a vector. Then for each turn, just do
the straightforward lookup and difference in the positions. This worked ok for
the short 2020 turns.

## Part 2

With 30,000,000 the brute force would take too long (or it took too long when I 
was trying). Instead, keep a map of the number and the last two turns that number
was spoken. This trims the amount of data needed. Each turn, check the map to see
if the number was spoken 2 times, if so, use the difference, else use 0 as the next
number and advance the turn. Somewhere around 2s to complete.

I think I can just use a map of the time before the last? rather than a map of pairs.

The performance of a straight vector here is 100x faster than the map.