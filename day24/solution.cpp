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
#include <vector>  // collection
#include <unordered_set>

#include "point.h"

using namespace std;

/* Update with data type and result types */
using data_t = vector<vector<point_t>>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Return the direction on hexagonal grid to go based on hexagonal directions */
vector<point_t> parse_directions(const string &line) {
	vector<point_t> dirs;

	size_t pos = 0;
	while (pos < line.length()) {
		if (line[pos] == 'e') {
			dirs.push_back({1, -1});
		}

		if (line[pos] == 'w') {
			dirs.push_back({-1, 1});
		}

		if (line[pos] == 's') {
			pos++;

			if (line[pos] == 'e') {
				dirs.push_back({0, -1});
			}

			if (line[pos] == 'w') {
				dirs.push_back({-1, 0});
			}
		}

		if (line[pos] == 'n') {
			pos++;

			if (line[pos] == 'e') {
				dirs.push_back({1, 0});
			}

			if (line[pos] == 'w') {
				dirs.push_back({0, 1});
			}
		}

		pos++;
	}

	return dirs;
}

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back(parse_directions(line));
		}
	}

	return data;
}

/* Return the set of active cells based on the hexagonal navigation
 * directions given in `data`.
 */
unordered_set<point_t> build_active(const data_t & data) {
	unordered_set<point_t> active;

	for (const auto & dirs : data) {
		point_t p {0, 0};

		for (const auto & dir : dirs) {
			p += dir;
		}

		if (active.contains(p)) {
			active.erase(p);
		} else {
			active.insert(p);
		}
	}

	return active;
}

/* Part 1 */
result_t part1(const data_t& data) {
	unordered_set<point_t> current = build_active(data);
	return current.size();
}

/* Return the min/max bounds of the current cells. */
pair<point_t, point_t> get_bounds(const unordered_set<point_t>& current) {
	point_t p_min {*current.begin()};
	point_t p_max {*current.begin()};

	for (const auto & p : current) {
		p_min.x = min(p_min.x, p.x);
		p_min.y = min(p_min.y, p.y);

		p_max.x = max(p_max.x, p.x);
		p_max.y = max(p_max.y, p.y);
	}

	return {p_min, p_max};
}

/* Return the min/max bounds outset by given offset */
pair<point_t, point_t> outset(const pair<point_t, point_t> & r, dimension_t offset) {
	return {{r.first.x - offset, r.first.y - offset}, 
			{r.second.x + offset, r.second.y + offset}};
}

/* Return the number of active neighbors based on hexagonal grid.
 * There are 6 neighboring cells
 */
size_t neighbor_count(const unordered_set<point_t>& current, const point_t &p) {
	vector<point_t> dirs = {{1, -1}, {-1, 1}, {0, -1}, {-1, 0}, {1, 0}, {0, 1}};

	size_t neighbors = 0;
	for (const auto& d : dirs) {
		if (current.contains(p + d)) {
			neighbors += 1;
		}
	}

	return neighbors;
}

/* Return the next iteration of the world, based on:
 * - active tile with 1 or 2 neighbors persists to next iteration
 * - inactive tile becomes active with exactly 2 neighbors
 */
unordered_set<point_t> step(const unordered_set<point_t> & current) {
	// outset the bounds enough to account for hexagonal grid
	auto [p_min, p_max] = outset(get_bounds(current), 2);

	// generate next set of active cells
	unordered_set<point_t> next;
	for (auto y = p_min.y; y < p_max.y; y++) {
		for (auto x = p_min.x; x < p_max.x; x++) {
			const point_t p {x, y};
			auto neighbors = neighbor_count(current, p);
			auto is_active = current.contains(p);

			if (is_active && (neighbors == 1 || neighbors == 2)) {
				// is active and has 1 or 2 neighbors, persist
				next.insert(p);
			} else if (!is_active && neighbors == 2) {
				// is inactive and has exactly 2 neighbors, become active
				next.insert(p);
			}
		}
	}

	return next;
}

/* Part 2 */
result_t part2(const data_t& data) {
	unordered_set<point_t> current = build_active(data);

	for (size_t day = 0; day < 100; day++) {
		auto next = step(current);		
		swap(next, current);
	}

	return current.size();
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
