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

vector<point_t> parse_line(const string &line) {
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
			data.push_back(parse_line(line));
		}
	}

	return data;
}

/* Part 1 */
result_t part1(const data_t& data) {
	unordered_set<point_t> tiles;

	for (const auto & dirs : data) {
		point_t p {0, 0};

		for (const auto & dir : dirs) {
			p += dir;
		}

		if (tiles.contains(p)) {
			tiles.erase(p);
		} else {
			tiles.insert(p);
		}
	}

	return tiles.size();
}

result_t part2([[maybe_unused]] const data_t& data) {
	return 0;
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
