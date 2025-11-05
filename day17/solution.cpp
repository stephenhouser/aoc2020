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
#include <unordered_set>

#include "point.h"

using namespace std;

/* Update with data type and result types */
using data_t = unordered_set<point_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	long y = 0;
	string line;
	while (getline(ifs, line)) {
		long x = 0;
		for (char c : line) {
			if (c == '#') {
				point_t p(x, y, 0l, 0l);
				data.insert(p);
			}
			x++;
		}
		y++;
	}

	return data;
}

void print_state(const data_t& state) {
	long min_x = 0, max_x = 0;
	long min_y = 0, max_y = 0;
	long min_z = 0, max_z = 0;
	long min_w = 0, max_w = 0;

	for (const auto &p : state) {
		min_x = min(min_x, p.x);
		max_x = max(max_x, p.x);
		min_y = min(min_y, p.y);
		max_y = max(max_y, p.y);
		min_z = min(min_z, p.z);
		max_z = max(max_z, p.z);
		min_w = min(min_w, p.w);
		max_w = max(max_w, p.w);
	}

	// print("min_x={}, max_x={}\n", min_x, max_x);
	// print("min_y={}, max_y={}\n", min_y, max_y);
	// print("min_z={}, max_z={}\n", min_z, max_z);

	for (long z = min_z; z <= max_z; z++) {
		print("z={}\n", z);
		for (long y = min_y; y <= max_y; y++) {
			for (long x = min_x; x <= max_x; x++) {
				const point_t p(x, y, z);
				if (state.contains(p)) {
					print("#");
				} else {
					print(".");
				}
			}
			print("\n");
		}
		print("\n");
	}
}

/* Return set of sites that neighbor site(p) in 3D space */
data_t get_neighbors_3d(const point_t& p) {
	data_t neighbors;

	for (long z = -1; z <= 1; z++) {
		for (long y = -1; y <= 1; y++) {
			for (long x = -1; x <= 1; x++) {
				if (x != 0 || y != 0 || z != 0) {
					point_t off(x, y, z);
					neighbors.insert(p + off);
				}
			}
		}
	}

	return neighbors;
}

/* Return set of sites that neighbor site(p) in 4D space */
data_t get_neighbors_4d(const point_t& p) {
	data_t neighbors;

	for (long w = -1; w <= 1; w++) {
		for (long z = -1; z <= 1; z++) {
			for (long y = -1; y <= 1; y++) {
				for (long x = -1; x <= 1; x++) {
					if (x != 0 || y != 0 || z != 0 || w != 0) {
						point_t off(x, y, z, w);
						neighbors.insert(p + off);
					}
				}
			}
		}
	}

	return neighbors;
}

/* Global function pointer, default is 3D for part 1
 * change to 4D for part 2.
 */
data_t (*get_neighbors)(const point_t& p) = get_neighbors_3d;


/* Return the set of all neighbors for all the sites in the passed set. */
data_t get_all_neighbors(const data_t& sites) {
	data_t all_neighbors;

	for (const auto& p : sites) {
		auto neighbors = get_neighbors(p);
		all_neighbors.insert(neighbors.begin(), neighbors.end());
	}

	return all_neighbors;
}

/* Return the number of active neighboring sites for site(p)*/
size_t count_neighbors(const data_t& state, const point_t& p) {
	size_t neighbor_count = 0;
	for (const auto& neighbor : get_neighbors(p)) {
		if (state.contains(neighbor)) {
			neighbor_count++;
		}
	}

	return neighbor_count;
}

/* Return the next state for the given site(p) */
bool next_state(const data_t& state, const point_t& p) {
	size_t neighbors = count_neighbors(state, p);
	if (state.contains(p)) {	// active
		return neighbors == 2 || neighbors == 3;
	} else {
		return neighbors == 3;	// birth
	}

	return false;
}

/* Return the next state for the entirety of the current state */
data_t next_state(const data_t& current) {
	data_t next;

	data_t active_neighbors = get_all_neighbors(current);
	for (const auto& neighbor : active_neighbors) {
		if (next_state(current, neighbor)) {
			next.insert(neighbor);
		}
	}

	return next;
}

/* Return the state after the number of generations */
data_t next_state(const data_t& start, size_t generations) {
	data_t current = start;
	// print_state(current);
	while (generations--) {
		data_t next = next_state(current);
		current = next;
		// print_state(current);
	}

	return current;
}

/* Part 1 */
result_t part1(const data_t& data) {
	/* Return the number of active sites after 6 generations */
	data_t final = next_state(data, 6);
	return final.size();
}

/* Part 2*/
result_t part2([[maybe_unused]] const data_t& data) {
	/* Return the number of active sites after 6 generations in 4D space */
	get_neighbors = get_neighbors_4d;
	data_t final = next_state(data, 6);
	return final.size();
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
