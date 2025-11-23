# Day 20: Jurassic Jigsaw

## Part 1

Use `regex` to parse the input, giving it a go rather than hand-parsing. `data` is vector of tiles.

Parse the edges (top, left, bottom, and right) as `size_t` where `. == 0` and `# == 1` bits in order. Example: `....####` is `0x0F`. With 10x10 tiles, these are 10-bit ints.

Add these edges into the tile's list of edges. Also add the reverse for where we might flip and rotate the tile. Example `....####` is `0x0F` and it's reverse is `0xF0`, so they both get pushed as edges of the tile.

Create an edge map that maps the edge number to the vector of tiles that have that edge (neighbors).

To find the corners, iterate over all the tiles and the edges of those. If a tile has 4 neighbors in the edge list, it is a corner. Actually two neighbors, but because we added the reverse bit pattern for each edge, there will be 4 in the edge list.

## Part 2

Will need the tile data without the edges. Add that to the `tile_t`