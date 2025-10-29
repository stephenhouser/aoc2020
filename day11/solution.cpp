#include <getopt.h>	 // getopt

#include <algorithm>  // sort
#include <cassert>	  // assert macro
#include <chrono>	  // high resolution timer
#include <cstring>	  // strtok, strdup
#include <fstream>	  // ifstream (reading file)
#include <numeric>	  // max, reduce, etc.
#include <print>
#include <ranges>  // ranges and views
#include <string>  // strings
#include <vector>  // collectin

#include "mrf.h"	// map, reduce, filter templates
#include "charmap.h"

using namespace std;

/* Update with data type and result types */
using data_t = charmap_t;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	return charmap_t::from_file(filename);
}

using eval_func_t = std::function<char(const charmap_t&, const point_t&)>;

/* Return the next state for the current map.
 * Use `eval` function to get next state of an individual cell.
 */
pair<charmap_t, result_t> next_state(const charmap_t& current, eval_func_t eval) {
	result_t changes = 0;
	charmap_t next(current.size_x, current.size_y);

	for (const auto [x, y, seat] : current.all_xy()) {
		auto next_s = eval(current, {x, y});
		next.set(x, y, next_s);
		changes += (seat == next_s) ? 0 : 1;
	}

	return {next, changes};
}

vector<point_t> directions = {
	{-1, -1}, {0, -1}, {1, -1},
	{-1,  0},          {1,  0},
	{-1,  1}, {0,  1}, {1,  1}
};

/* Part 1 */
result_t count_neighbors(const charmap_t& map, const point_t& p) {
	return reduce<point_t, result_t>(directions, 0ul, [&map, &p](result_t a, const point_t& d) {
		return (result_t)map.is_char(p+d, '#') ? a + 1 : a;
	});
}

char part1_next_seat(const charmap_t& map, const point_t& p) {
	auto seat = map.get(p);
	auto neighbors = count_neighbors(map, p);

	if (seat == 'L' && neighbors == 0) {
		return '#';
	}

	if (seat == '#' && 4 <= neighbors ) {
		return 'L';
	}

	return seat;
}

result_t part1(const data_t& data) {
	charmap_t current = data;
	result_t changes;

	do {
		auto p = next_state(current, part1_next_seat);
		current = p.first;
		changes = p.second;
	} while (changes != 0);

	auto seats = current.all_points() | std::views::common;
	result_t occupied = std::accumulate(seats.begin(), seats.end(), 0ul, [](auto a, const auto& seat) {
		return (seat.w == '#') ? a + 1 : a;
	});

	return occupied;
}

/* Part 2 */
result_t count_visible(const charmap_t& map, const point_t& p) {
	// find first seat we can see in direction dir (or first invalid location)
	auto first_seat = [&map, &p](const point_t& dir) {
		point_t n = p + dir;
		while (map.is_char(n, '.')) {
			n += dir;
		}

		return n;
	};

	return reduce<point_t, result_t>(directions, 0ul, [&map, &p, first_seat](result_t a, const point_t& dir) {
		return map.is_char(first_seat(dir), '#') ? a + 1 : a;
	});
}

char part2_next_seat(const charmap_t& map, const point_t& p) {
	auto seat = map.get(p);
	auto neighbors = count_visible(map, p);

	if (seat == 'L' && neighbors == 0) {
		return '#';
	}

	if (seat == '#' && 5 <= neighbors ) {
		return 'L';
	}

	return seat;
}

result_t part2(const data_t& data) {
	charmap_t current = data;
	result_t changes;

	do {
		auto p = next_state(current, part2_next_seat);
		current = p.first;
		changes = p.second;
	} while (changes != 0);

	auto seats = current.all_points() | std::views::common;
	result_t occupied = std::accumulate(seats.begin(), seats.end(), 0ul, [](auto a, const auto& seat) {
		return (seat.w == '#') ? a + 1 : a;
	});

	return occupied;
}

int main(int argc, char* argv[]) {
	bool verbose = false;

	int c;
	while ((c = getopt(argc, argv, "v")) != -1) {
		switch (c) {
			case 'v':
				verbose = !verbose;
				break;
			default:
				std::print(stderr, "ERROR: Unknown option \"{}\"\n", c);
				exit(1);
		}
	}

	argc -= optind;
	argv += optind;

	const char* input_file = argv[0];
	if (argc != 1) {
		std::print(stderr, "ERROR: No input file specified\n");
		exit(2);
	}

	auto start_time = chrono::high_resolution_clock::now();

	auto data = read_data(input_file);

	auto parse_complete = chrono::high_resolution_clock::now();
	duration_t parse_time = parse_complete - start_time;
	if (verbose) {
		print("{:>15} ({:>10.4f}ms)\n", "parse", parse_time.count());
	}

	result_t p1_result = part1(data);

	auto p1_complete = chrono::high_resolution_clock::now();
	duration_t p1_time = p1_complete - parse_complete;
	print("{:>15} ({:>10.4f}ms){}", p1_result, p1_time.count(), verbose ? "\n" : "");

	result_t p2_result = part2(data);

	auto p2_complete = chrono::high_resolution_clock::now();
	duration_t p2_time = p2_complete - p1_complete;
	print("{:>15} ({:>10.4f}ms){}", p2_result, p2_time.count(), verbose ? "\n" : "");

	duration_t total_time = p2_complete - start_time;
	print("{:>15} ({:>10.4f}ms)\n", "total", total_time.count());
}
