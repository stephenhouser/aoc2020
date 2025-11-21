# Day 19: Monster Messages

## Part 1

Another parsing problem. Read in the rules to a map of rules, read in the messages
to a vector of strings. Then a recursive `match_rule` function to see if the
rule matches. This function returns the number of tokens consumed or 0 if the rule
does not match. This way the caller can adjust where in the message to try and
match the next rule.

Hard part was getting the rule matching recursive function working. Until the
realization I could return the number of tokens consumed from the message. Then
it was sorting the small details.

## Part 2

Rewrote the match to still use recursion but to prepend the left and right
rulesets (one at a time) to the current ruleset sequence and see if those
match.