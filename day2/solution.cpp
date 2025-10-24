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

struct rule_t {
	std::pair<int, int> range;
	char required_char;

	bool range_validate(const string &password) {
		int matches = 0;
		for (auto c : password) {
			if (c == required_char) {
				matches++;
			}
		}

		return range.first <= matches && matches <= range.second;
	}

	bool position_validate(const string &password) {
		assert(range.first >= 1 && range.second >= 1);
		
		int matches = ((required_char == password[(size_t)range.first-1]) ? 1 : 0)
					+ ((required_char == password[(size_t)range.second-1]) ? 1 : 0);

		return matches == 1;
	}
};

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

using data_t = vector<pair<rule_t, string>>;
using result_t = size_t;

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			// 15-16 k: kkkkgkkkkkkkkkkh
			vector<string> parts = split(line, " -");
			int min = stoi(parts[0]);
			int max = stoi(parts[1]);
			char required_char = parts[2][0];
			string password = parts[3];
			data.push_back({{{min, max}, required_char}, password});
		}
	}

	return data;
}

/* Part 1 */
result_t part1(const data_t &data) {
	
	result_t valid = 0;
	for (const auto &check : data) {
		auto rule = check.first;
		auto password = check.second;
		if (rule.range_validate(password)) {
			valid++;
		}
	}

	return valid;
}

// 413 too high
// 400 too high
// 251 correct -- you need to read the rules better next time
result_t part2(const data_t &data) {
	result_t valid = 0;
	for (const auto &check : data) {
		auto rule = check.first;
		auto password = check.second;
		if (rule.position_validate(password)) {
			valid++;
		}
	}

	return valid;
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