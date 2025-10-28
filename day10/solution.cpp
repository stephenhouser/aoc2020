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
#include <set>
#include <map>

#include "mrf.h"	// map, reduce, filter templates

using namespace std;

/* Update with data type and result types */
using result_t = size_t;
using data_t = vector<result_t>;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back(stoul(line));
		}
	}

	return data;
}

data_t map_differences(const data_t& data) {
	data_t differences;

	size_t prev = 0;
	for (const auto value : data) {
		differences.push_back(value - prev);
		prev = value;
	}

	return differences;
}

result_t count(const data_t& data, result_t target) {
	return (result_t)std::count(data.begin(), data.end(), target);
}

/* Part 1 */
result_t part1(const data_t& data) {
	data_t adapters = data;
	std::sort(adapters.begin(), adapters.end());

	// add our laptop to the list
	adapters.push_back(adapters[adapters.size()-1] + 3);

	auto diffs = map_differences(adapters);

	// print("1 -> {}\n", count(diffs, 1));
	// print("3 -> {}\n", count(diffs, 3) + 1);

	return count(diffs, 1) * count(diffs, 3);
}

/* returns the number of ways we can make*/
result_t arrangements(const data_t& adapters) {
	std::map<result_t, result_t> counts;

	// add the starting joltage
	counts[0] = 1;

	for (size_t pos = 0; pos < adapters.size(); pos++) {
		result_t jolts = adapters[pos];

		for (size_t offset = 1; offset < 4; offset++) {
			if (offset <= jolts) {
				counts[jolts] += counts[jolts - offset];
			}
		}
	}

	return counts[adapters[adapters.size()-1]];
}

result_t part2(const data_t& data) {
	data_t adapters = data;
	std::sort(adapters.begin(), adapters.end());

	auto result = arrangements(adapters);
	return result;
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
