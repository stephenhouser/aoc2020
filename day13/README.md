# Day 13: Shuttle Search

## Part 1

Filter out `x` busses, then transform the bus ids to the soonest time after
the target time (our waiting time) that they will arrive: `bus - (target % bus)`.
Then all we need is to find the bus with the shortest waiting time.

## Part 2

It sure seemed like a Chinese Remainder Theorem problem... and it turns out
that was the correct thought! Each bus will arrive every multiple of it's number.

With the string `17,x,13,19`, bus IDs are:

- `17` at index `0`
- `13` at index `2`
- `19` at index `3`

So we need to find time `t` where

- bus `17` arrives at `t`
- bus `13` arrives at `t+2`
- bus `19` arrives at `t+3`

We are looking for

```
 t      % 17 == 0   --> t      == 0      == 0
(t + 2) % 13 == 0   --> t % 13 == 13 - 2 == 11
(t + 3) % 19 == 0   --> t % 19 == 19 - 3 == 16
```

Using the Chinese Remainder Theorem (where `x` is our `t`):

```
x =  0 (mod 17)
x = 11 (mod 13)
x = 16 (mod 19)

x = 3417
```

In code, enumerate the buses so we have pairs with their index/position and the bus id.
Then filter out the `x` busses (they have no value). Now, transform the index/position
to the bus's remainder (as above) `(bus - position) % bus` where `bus` is the bus' id
and `position` is the offset from the target time (`x` in the CRT). Once we have those
pairs, we run the Chinese Remainder on the bus id as the `mod` values and the remainders.
This gives is the smallest time `t` (or `x` in CRT).

Wrote the `chinese_remainder` function and added it to the `day0` template.