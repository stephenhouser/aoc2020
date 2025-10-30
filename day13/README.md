# Day 0: Template

Template code and common code for Advent of Code



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