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


using data_t = vector<pair<rule_t, string>>;
using result_t = size_t;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


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

template <typename T>
void print_result(T result, chrono::duration<double, milli> duration) {
	const int time_width = 10;
	const int time_precision = 4;
	const int result_width = 15;

	cout << std::setw(result_width) << std::right << result;

	cout << " ("
		 << std::setw(time_width) << std::fixed << std::right 
		 << std::setprecision(time_precision)
		 << duration.count() << "ms)";
	cout << endl;
}

int main(int argc, char *argv[]) {
	const char *input_file = argv[1];
	if (argc < 2) {
		input_file = "test.txt";
	}

    auto start_time = chrono::high_resolution_clock::now();

	auto data = read_data(input_file);

	auto parse_time = chrono::high_resolution_clock::now();
	print_result("parse", (parse_time - start_time));

	result_t p1_result = part1(data);

	auto p1_time = chrono::high_resolution_clock::now();
	print_result(p1_result, (p1_time - parse_time));

	result_t p2_result = part2(data);

	auto p2_time = chrono::high_resolution_clock::now();
	print_result(p2_result, (p2_time - p1_time));

	print_result("total", (p2_time - start_time));
}
