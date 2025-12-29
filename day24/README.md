# Day 24: Lobby Layout

Iteration, hexagonal grid, points.

## Part 1

Use `point_t` translating rectangular grid to hex grid. Parse compass directions
to point directions. Start with reference tile (0,0) and apply directions to
flip or un-flip destination tile.

[Amit’s Thoughts on Grids](http://www-cs-students.stanford.edu/~amitp/game-programming/grids/) is a great reference for using recangular grid for a hex pattern. Helped a lot here.

## Part 2

Conway's Game of Life on a hexagonal grid. Just need to stick with the hex grid
pattern. Outset the bounds of the world we know of, then iterate over all the
interior `x, y` coordinates and check for live in the next iteration.
