# Day 18: Operation Order

## Part 1

Tried (and failed) to do it 'one-shot' read through the string parsing into
tokens then reverse the list and account for open and close parens.

Solved with a recursive descent parser to create a tree of the equation, then an `evaluate`
recursive function to compute the result. Hardest part was getting the parser
written.

```
    expr : term (('+' | '*') term)*
    term : number | '(' expr ')'
```

`parse_expr()` for the `expr` parsing, `parse_term()` for the term, and a 
`parse_number()` for the numbers.

Used a regex for the lexical analyzer, to return the next token from the string
and advance the position for the next token. Had a version that was simple
C-string handling, but this seemed more C++-like.

Encapsulated the lexer and parser into a `parser_t` class.

Evaluation was depth-first traversal, calculating the left and right child
nodes throughout.

## Part 2

Rewrote the parser to be a superclass with the base parsing, then a `parser1_t`
for part 1 where we evaluate w/o precedence and `parser2_t` where we have another
level in our grammar.

```
    expr   : (term ('*' term))*
    factor : (term ('+' term))*
    term   : number | '(' expr ')'
```

Evaluation did not have to change as the new tree takes into account the order.


```
while open paren
    reduce

eval(tokens)
    if len(tokens) <= 1 || open paren
        return digit

    if close paren
        eval(cdr(tokens))

    return digit operator eval(remainder)

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
