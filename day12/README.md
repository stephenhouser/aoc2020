# Day 12: Rain Risk

## Part 1

Using the `point_t` and `vector_t`, simply move the ship around the world.
The instruction decoding was straightforward, except the rotate operations.
Had a few versions of these. settled on `x = -y; y = x` for rotating left in
90 degree increments. This code should be useful elsewhere. Also wrote a direction
to name (north, south, east, west...) decoder that can also be useful. Putting them
in the `point_t` files.

## Part 2

In this one, just remember the waypoint is relative to the ship. Then
make the movements affect the waypoint or ship accordingly.