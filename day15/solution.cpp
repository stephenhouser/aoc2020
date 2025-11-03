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
#include <unordered_map>

#include "mrf.h"	// map, reduce, filter templates
#include "split.h"	// split strings

using namespace std;

bool verbose = false;

/* Update with data type and result types */
using data_t = vector<size_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			return split_size_t(line, ",");
		}
	}

	return data;
}

/* Part 1 */
size_t next_number(const data_t& numbers) {
	size_t turn = numbers.size();
	assert(turn > 1);

	size_t last_number = numbers[turn-1];

	// find previous position of last number
	size_t pos = turn - 2;
	for (; pos != 0; pos--) {
		if (numbers[pos] == last_number) {
			break;
		}
	}

	if (pos == 0 && numbers[pos] != last_number) {
		// number has not been "spoken", next number is 0
		return 0;
	}

	// print("({}: {},{})\n", last_number, turn-1, pos);
	return (turn-1) - pos;
}

void print_vec(const data_t& data) {
	for (const auto n : data) {
		print("{} ", n);
	}
	print("\n");
}

using spoken_map = unordered_map<size_t, pair<size_t, size_t>>;

bool spoken_before(const spoken_map& last_spoken, size_t number) {
	if (last_spoken.contains(number)) {
		auto [first, second] = last_spoken.at(number);
		return first != second;
	}

	return false;
}

void speak_number(spoken_map& last_spoken, size_t number, size_t turn) {
	if (last_spoken.contains(number)) {
		last_spoken[number].first = last_spoken[number].second;
		last_spoken[number].second = turn;
	} else {
		last_spoken[number] = {turn, turn};
	}
}

size_t spoken_number(const data_t& seed, const size_t turns) {
	spoken_map last_spoken;

	// seed the last_spoken map
	for (const auto [i, n] : enumerate<size_t, size_t>(seed)) {
		speak_number(last_spoken, n, i+1);
	}

	size_t last = seed[seed.size()-1];
	size_t turn = seed.size()+1;

	while (turn <= turns) {
		size_t next = 0;
		if (spoken_before(last_spoken, last)) {
			auto [first, second] = last_spoken.at(last);
			next = second - first;
		}

		// print("turn={} last={}, next={}\n", turn, last, next);
		speak_number(last_spoken, next, turn);
		last = next;
		turn++;
	}

	return last;
}

// tracking just the last time, about the same performance as a map
size_t spoken_number2(const data_t& seed, const size_t turns) {
	unordered_map<size_t, size_t> last_spoken;

	size_t next;
	size_t last = 0;
	for (size_t turn = 0; turn < turns; turn++) {
		if (turn < seed.size()) {
			next = seed[turn];
		} else {
			if (last_spoken.contains(last)) {
				next = turn - last_spoken[last];
			} else {
				next = 0;
			}
		}

		if (turn != 0) {
			last_spoken[last] = turn;
		}

		last = next;
	}

	return last;
}

// using a big vector, much faster
// this one from the reddit solution thread
size_t spoken_number3(const data_t& seed, const size_t turns) {
	vector<size_t> last_spoken(turns);
	vector<bool> is_valid(turns);

	size_t last = 0;
	for (size_t turn = 0; turn < turns; turn++) {
		size_t next = 0;
		if (turn < seed.size()) {
			next = seed[turn];
		} else if (is_valid[last]) {
			next = turn - last_spoken[last];
		}

		if (turn != 0) {
			last_spoken[last] = turn;
			is_valid[last] = true;
		}

		last = next;
	}

	return last;
}

result_t part1(const data_t& data) {
	return spoken_number3(data, 2020);
}
/* Part 2*/
result_t part2(const data_t& data) {
	return spoken_number3(data, 30000000);
}

int main(int argc, char* argv[]) {

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
