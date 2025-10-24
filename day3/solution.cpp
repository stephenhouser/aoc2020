#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <vector>		// collectin
#include <string>		// strings
#include <ranges>		// ranges and views
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <unistd.h>     // getopt
#include <print>		// std::print

#include "charmap.h"

using namespace std;

using data_t = charmap_t;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

const data_t read_data(const string &filename) {
	return charmap_t::from_file(filename);
}

result_t run_trail(const data_t &map, const point_t &move) {
	size_t trees = 0;
	point_t pos = {0, 0};
	while (pos.y < map.size_y) {
		if (map.is_char(pos, '#')) {
			trees++;
		}

		pos.x = (pos.x + move.x) % map.size_x;
		pos.y = (pos.y + move.y);
	}

	return trees;
}

vector<result_t> run_trails(const data_t &map, const vector<point_t> &moves) {
	vector<result_t> trees;

	for (const auto &move : moves) {
		trees.push_back(run_trail(map, move));
	}

	return trees;
}

template <typename T>
T reduce(std::vector<T> vec, const T &start, std::function<T(T, T)> func) {
	return std::accumulate(vec.begin(), vec.end(), start, func);
}

/* Part 1 */
result_t part1(const data_t &map) {
	return run_trail(map, {3, 1});
}

result_t part2(const data_t &map) {
	const vector<point_t> moves = {{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}};
	const vector<size_t> path_trees = run_trails(map, moves);

	size_t trees = reduce<size_t>(path_trees, 1, [](size_t a, size_t b) { return a * b; });
	return trees;
}


int main(int argc, char *argv[]) {
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

	const char *input_file = argv[0];
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