# Day 20: Jurassic Jigsaw

## Part 1

Use `regex` to parse the input, giving it a go rather than hand-parsing. `data` is vector of tiles.

NOTE: std::regex is notoriously slow!

Parse the edges (top, left, bottom, and right) as `size_t` where `. == 0` and `# == 1` bits in order. Example: `....####` is `0x0F`. With 10x10 tiles, these are 10-bit ints.

Add these edges into the tile's list of edges. Also add the reverse for where we might flip and rotate the tile. Example `....####` is `0x0F` and it's reverse is `0xF0`, so they both get pushed as edges of the tile.

Create an edge map that maps the edge number to the vector of tiles that have that edge (neighbors).

To find the corners, iterate over all the tiles and the edges of those. If a tile has 4 neighbors in the edge list, it is a corner. Actually two neighbors, but because we added the reverse bit pattern for each edge, there will be 4 in the edge list.

## Part 2

Will need the tile data without the edges. Add that to the `tile_t`

Wow, this one took a long time! Did all of 2025 while it sat and I thought
about it.

Completely changed approach. 

Simplified `tile_t` to keep raw vector of strings and `top()`, `left()`,
`right()`, and `bottom()` functions to return the edges as `strings`. Then
use these to make comparisons with other tile edges in `find_below()`,
`find_left()` functions.

Generated list of all orientations of all tiles, e.g. all rotations and then
all flips of those rotations. 8 versions for every original tile. This was the
key insight. It was a set of nested loops to then generate the arrangement
of tiles using find_left, find_below.

Sea monster searching (and erasing) was using a vector of vectors that had
the positions of a monster from any offset. Brute force search all orientations
of the merged tile set to try and erase sea monsters. If we erase any then
return the number of remaining '#' characters.
