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

#include "mrf.h"	// map, reduce, filter templates

using namespace std;

/* Update with data type and result types */
using data_t = vector<long int>;
using result_t = size_t;

/* 5 for test, 25 for input set in main()
 * this is the preamble size and the window of data we look at
 * for finding the matching numbers to determine validity.
 */
size_t	window_size = 25;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back(stol(line));
		}
	}

	return data;
}

set<long int> get_window(const data_t &data, size_t pos) {
	set<long int> window;

	if (data.size() >= window_size) {
		for (size_t i = pos - window_size; i < pos; i++) {
			window.insert(data[i]);
		}
	}

	return window;
}

bool is_valid(const data_t& data, size_t pos) {
	// get all the unique numbers in the window to evaluate
	auto window = get_window(data, pos);

	// check the set for numbers that add to our number
	for (auto candidate : window) {
		if (window.contains(data[pos] - candidate)) {
			return true;
		}
	}

	return false;
}

long int find_invalid(const data_t& data) {
	for (size_t pos = window_size; pos < data.size(); pos++) {
		if (!is_valid(data, pos)) {
			return data[pos];
		}
	}

	return 0;
}

/* Part 1 */
result_t part1(const data_t& data) {
	auto result = find_invalid(data);
	return (result_t)result;
}

/* return 0 or length of string that sums to target */
size_t find_sum(const data_t& data, size_t start, long int target) {
	long int total = 0;
	size_t length = 0;

	auto pos = start;
	while (total < target) {
		length++;
		total += data[pos++];
	}

	return (total == target) ? length : 0;
}

/* Return the min and max number in data[start], data[start + length - 1]*/
pair<long int, long int> minmax_range(const data_t& data, size_t start, size_t length) {
	long int min = data[start];
	long int max = data[start];

	for (size_t pos = start + 1; pos < start + length; pos++) {
		if (data[pos] < min) {
			min = data[pos];
		}

		if (data[pos] > max) {
			max = data[pos];
		}
	}

	return {min, max};
}

result_t part2(const data_t& data) {
	auto target = find_invalid(data);

	// loop until we see the target (invalid) number
	for (size_t pos = 0; data[pos] != target; pos++) {
		auto length = find_sum(data, pos, target);
		if (length >= 2) {
			auto mm = minmax_range(data, pos, length);
			return (result_t)(mm.first + mm.second);
		}
	}

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

	/* set window size based on input file; 25 for live data, 5 otherwise */
	window_size = (strcmp("input.txt", input_file)) ? 5 : 25;

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
