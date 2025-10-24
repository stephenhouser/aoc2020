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

#include "split.h"

using namespace std;

using data_t = vector<int>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back(stoi(line));
		}
	}

	return data;
}

/* Part 1 */
result_t part1([[maybe_unused]] const data_t &data) {
	auto numbers = data;
	std::sort(numbers.begin(), numbers.end());

	// brute force with binary search for the complement
	for (size_t i = 0; i < numbers.size(); i++) {
		int complement = 2020 - numbers[i];
		auto it = std::lower_bound(numbers.begin() + (long int)i + 1, numbers.end(), complement);
		if (it != numbers.end() && *it == complement) {
			return static_cast<result_t>(numbers[i] * complement);
		}
	}

	return 0;
}

result_t part2([[maybe_unused]] const data_t &data) {
	auto numbers = data;
	std::sort(numbers.begin(), numbers.end());

	// brute force with binary search for the complement
	for (size_t i = 0; i < numbers.size(); i++) {
		for (size_t j = i + 1; j < numbers.size(); j++) {
			int complement = 2020 - numbers[i] - numbers[j];
			auto it = std::lower_bound(numbers.begin() + (long int)j + 1, numbers.end(), complement);
			if (it != numbers.end() && *it == complement) {
				return static_cast<result_t>(numbers[i] * numbers[j] * complement);
			}
		}
	}

	return 0;
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