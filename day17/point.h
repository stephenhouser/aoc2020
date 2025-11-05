#if !defined(POINT_T_H)
#define POINT_T_H

#include <concepts>
#include <iomanip>	 // setw and setprecision on output
#include <iostream>	 // cout
#include <string>	 // std::string
#include <tuple>
#include <vector>
#include <cassert>

using dimension_t = long;
using value_t = long;

struct point_t {
	dimension_t x = 0;
	dimension_t y = 0;
	dimension_t z = 0;

	bool feature_z_sort = false;	// true=sort by z,y,x, false=x,y,z

	// value_t u = 0;
	// value_t v = 0;
	value_t w = 0;

	point_t() {}

	/* Use templates for constructor so I don't have to worry about
	 * mixing int and unsigned int, size_t, etc. on creation. */
	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	point_t(Tx x, Ty y, Tx z, Tx w) : x(static_cast<dimension_t>(x)),
									  y(static_cast<dimension_t>(y)),
									  z(static_cast<dimension_t>(z)),
									  w(static_cast<dimension_t>(w)) {
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	point_t(Tx x, Ty y, Tx z) : x(static_cast<dimension_t>(x)),
								y(static_cast<dimension_t>(y)),
								z(static_cast<dimension_t>(z)) {
	}
	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	point_t(Tx x, Ty y) : x(static_cast<dimension_t>(x)),
						  y(static_cast<dimension_t>(y)),
						  z(0),
						  w(0) {
	}

	point_t(const point_t& p) : x(static_cast<dimension_t>(p.x)),
								y(static_cast<dimension_t>(p.y)),
								z(static_cast<dimension_t>(p.z)),
								// u(static_cast<dimension_t>(p.u)),
								// v(static_cast<dimension_t>(p.v)),
								w(static_cast<dimension_t>(p.w)) {
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	point_t(const std::pair<Tx, Ty>& p) : x(static_cast<dimension_t>(p.first)),
										  y(static_cast<dimension_t>(p.second)),
										  z(0) {
	}

	template <std::convertible_to<dimension_t> T>
	point_t(const std::vector<T>& v) {
		if (v.size() > 0) {
			this->x = static_cast<dimension_t>(v[0]);
			if (v.size() > 1) {
				this->y = static_cast<dimension_t>(v[1]);
				if (v.size() > 2) {
					this->z = static_cast<dimension_t>(v[2]);
					if (v.size() > 3) {
						this->w = static_cast<dimension_t>(v[3]);
					}
				}
			}
		}
	}

	/* Rotate point left (counter-clockwise) around origin in increments of 90-degrees 
	 * Currently only tested with 90, 180, and 270 degrees (e.g. charmap_t things)
	 */
	void rotate_left(dimension_t angle) {
		// only works with 90 degree increments
		assert(angle == 90 || angle == 180 || angle == 270);

		auto next_x = x;
		auto next_y = y;
		while (angle > 0) {
			auto temp = next_x;
			next_x = -next_y;
			next_y = temp;
			angle -= 90;
		}

		this->x = next_x;
		this->y = next_y;
	}

	/* Rotate point right (clockwise) around origin in increments of 90-degrees
	 */
	void rotate_right(dimension_t angle) {
		this->rotate_left(360 - angle);
	}

	bool operator<(const point_t& rhs) const {
		if (feature_z_sort) {
			// sorts by z, then y, then x
			if (z == rhs.z) {
				if (y == rhs.y) {
					return x < rhs.x;
				}

				return y < rhs.y;
			}
			
			return z < rhs.z;
		}

		// sorts by y, then x (default)
		if (x == rhs.x) {
			if (y == rhs.y) {
				if (z == rhs.z) {
					return w < rhs.w;
				}

				return z < rhs.z;
			}

			return y < rhs.y;
		}

		return x < rhs.x;
	}

	bool operator==(const point_t& other) const {
		return this->x == other.x 
			&& this->y == other.y 
			&& this->z == other.z
			&& this->w == other.w;
	}

	bool operator!=(const point_t& other) const {
		return !(*this == other);
	}

	point_t& operator=(const point_t& other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		// this->u = other.u;
		// this->v = other.v;
		this->w = other.w;
		return *this;
	}

	point_t& operator+=(const point_t& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		// this->u += rhs.u;
		// this->v += rhs.v;
		this->w += rhs.w;
		return *this;
	}

	// friends defined inside class body are inline and are hidden from non-ADL lookup
	friend point_t operator+(point_t lhs, const point_t& rhs) {
		lhs += rhs;	 // reuse compound assignment
		return lhs;	 // return the result by value (uses move constructor)
	}

	point_t& operator-=(const point_t& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		// this->u -= rhs.u;
		// this->v -= rhs.v;
		this->w -= rhs.w;
		return *this;
	}

	// friends defined inside class body are inline and are hidden from non-ADL lookup
	friend point_t operator-(point_t lhs, const point_t& rhs) {
		lhs -= rhs;	 // reuse compound assignment
		return lhs;	 // return the result by value (uses move constructor)
	}

	static point_t from_string(const std::string& str) {
		auto split_numbers = [](const std::string& str) {
			const std::string digits{"-0123456789"};
			std::vector<long> result;

			size_t start = str.find_first_of(digits, 0);
			size_t end = str.find_first_not_of(digits, start);
			while (start != std::string::npos) {
				result.push_back(std::stol(str.substr(start)));

				start = str.find_first_of(digits, end);
				end = str.find_first_not_of(digits, start);
			}

			return result;
		};

		return {split_numbers(str)};
	}

	friend struct std::formatter<point_t>;
};

std::ostream& operator<<(std::ostream& os, const point_t& p);
std::ostream& operator<<(std::ostream& os, const std::vector<point_t>& v);
std::istream& operator>>(std::istream& is, point_t& p);

/* hash function so can be put in unordered_map or set */
template <>
struct std::hash<point_t> {
	size_t operator()(const point_t& p) const {
		return std::hash<size_t>()(
			  (((size_t)p.w & 0xFFFF) << 48)
			| (((size_t)p.z & 0xFFFF) << 32) 
			| (((size_t)p.x & 0xFFFF) << 24) 
			| (((size_t)p.y & 0xFFFF)));
	}
};

template <>
struct std::formatter<point_t> {
	constexpr auto parse(std::format_parse_context& context) {
		return context.begin();
	}

	auto format(const point_t& p, std::format_context& context) const {
		auto out = context.out();

		std::format_to(out, "({},{}", p.x, p.y);
		if (p.z) {
			std::format_to(out, ",{}", p.z);
			if (p.w) {
				std::format_to(out, ",{}", p.w);
			}
		}
		std::format_to(out, ")");

		return out;
	}
};

inline dimension_t manhattan_distance(const point_t& p1, const point_t& p2) {
	dimension_t dx = p1.x > p2.x ? p1.x - p2.x : p2.x - p1.x;
	dimension_t dy = p1.y > p2.y ? p1.y - p2.y : p2.y - p1.y;
	dimension_t dz = p1.z > p2.z ? p1.z - p2.z : p2.z - p1.z;
	// dimension_t du = p1.u > p2.u ? p1.u - p2.u : p2.u - p1.u;
	// dimension_t dv = p1.v > p2.v ? p1.v - p2.v : p2.v - p1.v;
	dimension_t dw = p1.w > p2.w ? p1.w - p2.w : p2.w - p1.w;
	return dx + dy + dz + dw;
}

// read points until we hit an empty line
std::vector<point_t> read_points(std::istream& is, void (*fn)(point_t& point, const std::string& line) = nullptr);

#endif