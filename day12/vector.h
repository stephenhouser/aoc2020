#if !defined(VECTOR_T_H)
#define VECTOR_T_H

#include <cstring>	 // strtok, strdup
#include <iomanip>	 // setw and setprecision on output
#include <iostream>	 // cout
#include <string>	 // std::string
#include <tuple>
#include <vector>
#include <cassert>

#include "point.h"

struct vector_t {
	point_t p = {0, 0};
	point_t dir = {0, 0};

	vector_t(const point_t& p, const point_t& dir) : p(p), dir(dir) {
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	vector_t(Tx x, Ty y, Tx dx, Ty dy) : p(x, y), dir(dx, dy) {
	}

	template <std::convertible_to<dimension_t> Tx,
			  std::convertible_to<dimension_t> Ty,
			  std::convertible_to<dimension_t> Tz>
	vector_t(Tx x, Ty y, Tz z, Tx dx, Ty dy, Tz dz) : p(x, y, z), dir(dx, dy, dz) {
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	vector_t(const std::pair<Tx, Ty>& p, const std::pair<Tx, Ty>& dir) : p(p), dir(dir) {
	}

	bool operator<(const vector_t& rhs) const {
		if (p == rhs.p) {
			return dir < rhs.dir;
		}

		return p < rhs.p;
	}

	bool operator==(const vector_t& other) const {
		return this->p == other.p && this->dir == other.dir;
	}

	std::string direction_name() {
		// Can only work with unit vectors
		assert(this->dir.x == 0 || this->dir.x == 1 || this->dir.x == -1);
		assert(this->dir.y == 0 || this->dir.y == 1 || this->dir.y == -1);

		// change in direction when rotating left in 45 degree steps
		// East (1,0) -> North East (1,-1) -> North (0,1), etc...
		std::vector<point_t> directions = {
			{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

		std::vector<std::string> names = {
			"east", "northeast", "north", "northwest", "west", "southwest", "south", "southeast"};

		// find offset of vector's direction in direction table
		size_t dir_n = 0;
		for (; dir_n < directions.size(); dir_n++) {
			if (this->dir == directions[dir_n]) {
				return names[dir_n];
			}
		}

		return "unknown";
	}

	// changes direction vector to the left in increments of 90-degrees
	void turn_left(dimension_t angle) {
		// only works with 90 degree increments
		assert(angle == 90 || angle == 180 || angle == 270);

		point_t next = this->dir;
		while (angle > 0) {
			auto temp = next.x;
			next.x = -next.y;
			next.y = temp;
			angle -= 90;
		}

		this->dir = next;
	}

	// changes direction vector to the right in increments of 90-degrees
	void turn_right(dimension_t angle) {
		this->turn_left(360 - angle);
	}
};

std::ostream& operator<<(std::ostream& os, const vector_t& p);

/* Hash function for a vector, so that it can be in unordered set and map */
template <>
struct std::hash<vector_t> {
	size_t operator()(const vector_t& v) const {
		return std::hash<size_t>()((((size_t)v.p.x & 0xFFFF) << 32) | (((size_t)v.p.y & 0xFFFF) << 24) | (((size_t)v.dir.x & 0xFFFF) << 16) | (((size_t)v.dir.y & 0xFFFF)));
	}
};
#endif