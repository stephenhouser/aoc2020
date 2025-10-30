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
#include "split.h"	// split strings
#include "chinese_remainder.h"

using namespace std;

/* Update with data type and result types */
struct data_t {
	size_t time = 0;
	vector<size_t> busses = {};
};

// using data_t = vector<string>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	result_t line_n = 0;
	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			if (line_n % 2 == 0) {
				data.time = stoul(line);
			} else {
				for (const auto& bus : split(line)) {
					if (isdigit(bus[0])) {
						data.busses.push_back(std::stoul(bus));
					} else {
						data.busses.push_back(0);
					}
				}
			}

			line_n++;
		}
	}

	return data;
}

/* Part 1 */
result_t part1(const data_t& data) {

	auto target_time = data.time;

	using bus_time = pair<size_t, size_t>;
	auto non_zero = [](size_t n) { return n != 0; };
	auto next_time = [target_time](size_t bus) -> bus_time { return {bus, bus - (target_time % bus)}; };
	auto time_compare = [](const bus_time a, const bus_time b) { return a.second < b.second; };

	auto waiting_times = data.busses | std::views::filter(non_zero) | std::views::transform(next_time);
	auto shortest_wait = std::ranges::min_element(waiting_times, time_compare);

	auto bus_id = (*shortest_wait).first;
	auto wait_time = (*shortest_wait).second;
	print("bus {} in {} minutes\n", bus_id, wait_time);

	return bus_id * wait_time;
}

vector<pair<long int, long int>> index_pairs(const vector<size_t>& v){
	vector<pair<long int, long int>> pairs;

	for (size_t i = 0; i < v.size(); i++) {
		if (v[i] != 0) {
			pairs.push_back({(long int)i, (long int)v[i]});
		}
	}

	return pairs;
}

result_t part2(const data_t& data) {
	// vector<long int> numbers = {67, 7, 59, 61};
	// vector<long int> remainders = {0, 6, 57, 58};

	// long int l = chinese_remainder(remainders, numbers);

	// auto non_zero = [](size_t n) { return n != 0; };

	auto pos_bus = index_pairs(data.busses);

	auto to_remainder = [](const pair<long int, long int>& pair) {
		long int remainder = (pair.second - pair.first) % pair.second;
		std::pair<long int, long int> r = {remainder, pair.second};
		return r;
	};

	auto rem_mod = pos_bus | std::views::transform(to_remainder);

	auto pair_first = [](const pair<long int, long int>& p) {
		return p.first;
	};

	auto pair_second = [](const pair<long int, long int>& p) {
		return p.second;
	};

	auto remainders = rem_mod | std::views::transform(pair_first) | std::ranges::to<vector<long int>>();

	for (const auto &i : remainders) {
		print("{}, ", i);
	}
	print("\n");


	auto busses     = rem_mod | std::views::transform(pair_second) | std::ranges::to<vector<long int>>();

	for (const auto& i : busses) {
		print("{}, ", i);
	}
	print("\n");

	long int l = chinese_remainder(remainders, busses);

	// vector<result_t> v;
	// for (const auto &i : data.busses) {
	// 	if (i != 0) {
	// 		v.push_back(i);
	// 	}
	// }

	// auto l = lcm(v);
	print("lcm={}\n", l);

	return data.busses.size();
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
