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
#include <deque>
#include <unordered_set>

#include "mrf.h"	// map, reduce, filter templates
#include "split.h"	// split strings

using namespace std;

/* Update with data type and result types */
using data_t = vector<vector<size_t>>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	vector<size_t> cards;
	string line;
	while (getline(ifs, line)) {
		if (line.empty()) {
			data.push_back(cards);
			cards.clear();
		} else if (isdigit(line[0])) {
			cards.push_back(stoul(line));
		}
	}

	data.push_back(cards);
	return data;
}

unordered_set<string> cache;

void play_round(deque<size_t>& p1, deque<size_t>& p2) {
	assert(!p1.empty() && !p2.empty());

	size_t c1 = p1.front();
	p1.pop_front();

	size_t c2 = p2.front();
	p2.pop_front();

	if (c1 > c2) {
		p1.push_back(c1);
		p1.push_back(c2);
	} else {
		p2.push_back(c2);
		p2.push_back(c1);
	}
}

result_t score_deck(deque<size_t> deck) {
	result_t score = 0;

	size_t m = 1;
	for (auto it = deck.rbegin(); it != deck.rend(); it++) {
		score += m++ * *it;
	}

	return score;
}

/* Part 1 */
result_t part1(const data_t& data) {
	assert(data.size() == 2);

	deque<size_t> p1(data[0].begin(), data[0].end());
	deque<size_t> p2(data[1].begin(), data[1].end());

	while (!p1.empty() && !p2.empty()) {
		play_round(p1, p2);
	}

	auto &winner = p1.size() > p2.size() ? p1 : p2;
	print("{}\n", winner);
	return score_deck(winner);
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
