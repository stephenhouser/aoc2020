# Day 21: Allergen Assessment

## Part 1

Consisted mostly of creating a reverse map of `allergen` -> `ingredients`.
The first time we encounter an allergen, add all the ingredients as possible
names for the allergen containing item. Then each time we encounter the
allergen again, replace the set of possibilities with the intersection of the
new ingredients and the previous possible ones.

Then, go through the map looking for a mapping of an allergen to one ingredient.
Move that to a confirmed map (allergen -> ingredient), remove it from the
allergen map and remove all other occurrences of the ingredient from any other
set of possible matches. Repeat until there are none left and everything is
on the confirmed list. This is our allergen->ingredient map.

Now go through all the food items and count the ingredients that are not in
the allergen mapping.

## Part 2

I did all the hard work on Part 1!! Happy day. Spent time here just sorting
the allergen ingredients by the allergen names (compare function) and formatting
the output/results.