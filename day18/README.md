# Day 18: Operation Order

## Part 1

while open paren
    reduce

eval(tokens)
    if len(tokens) <= 1 || open paren
        return digit

    if close paren
        eval(cdr(tokens))

    return digit operator eval(remainder)
```
1 + 2 * 3 + 4 * 5 + 6 becomes 71.
tokens = 6 + 5 * 4 + 3 * 2 + 1
    tokens = 5 * 4 + 3 * 2 + 1
        tokens = 4 + 3 * 2 + 1
            tokens = 3 * 2 + 1
                tokens = 2 + 1
                    tokens = 1
                    return 1 (1)
                return 2 + 1 (3)
            return 3 * 3 (9)
        return 4 + 9 (13)
    return 5 * 13 (65)
return 6 + 65 (71)

2 * 3 + ( 4 * 5 ) becomes 26.
tokens = 5) * (4 + 3 * 2
    tokens = * (4 + 3 * 2


5 + (8 *3 + 9 + 3* 4 *3) becomes 437.
5* 9 *(7* 3 *3 + 9* 3 + (8 + 6 *4)) becomes 12240.
((2 + 4* 9) *(6 + 9* 8 + 6) + 6) + 2 + 4 * 2 becomes 13632.
```

Template code and common code for Advent of Code