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

using namespace std;

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
			for (const char ch : line) {
				data.push_back((size_t)(ch-'0'));
			}	
		}
	}

	return data;
}

/* Print the cups... */
void print_cups(const vector<size_t>& cups, size_t current) {
	print("({}) ", current);
	for (size_t next = current; cups[next] != current; next = cups[next]) {
		print("{} ", cups[next]);
	}
}

/* Return the destination cup, given the `current` cup.
 * The next lowest value label or restart from the highest if none below.
 */
size_t find_destination(const vector<size_t> &cups, size_t current) {
	size_t label = (current == 1 ? cups.size() : current) - 1;

	while (cups[current] == label
			|| cups[cups[current]] == label 
			|| cups[cups[cups[current]]] == label) {

		label = (label == 1 ? cups.size() : label) - 1;
	}

	return label;
}

/* Return the cup organization afeter `moves`. 
 * Returns a vector of the first few cups excluding cup 1 (see below).
 */
vector<size_t> move_cups(const data_t& data, const size_t moves) {
	// Create vector where label is index and value is location of next cup
	vector<size_t> cups(data.size()+1);
	for (size_t i = 0; i < data.size(); i++) {
		cups[data[i]] = data[(i + 1) % data.size()];
	}

	// start with the cup in the first position of original data
	size_t current = data[0];

	// move...
	for (size_t m = 0; m < moves; m++) {
		auto destination = find_destination(cups, current);
		
		// Remove next 3 cups from current position
		auto splice_start = cups[current];
		auto splice_end   = cups[cups[cups[current]]];
		cups[current] = cups[splice_end];

		// splice in after destination cup
		cups[splice_end] = cups[destination];
		cups[destination] = splice_start;

		// advance to next cup
		current = cups[current];
	}

	// return a vector of the first 10 cups, skipping cup 1 as we don't use
	// it in calculating the answer
	vector<size_t> result;
	for (size_t i = 0, cup = cups[1]; i < 10 && cups[cup] != 1; i++, cup = cups[cup]) {
		result.push_back(cup);
	}
	return result;
}

/* Part 1 */
result_t part1(const data_t& data) {
	// part 1, 100 moves
	auto cups = move_cups(data, 100);

	// result is the integer formed by the cup organization
	// not including cup 1.
	size_t result = 0;
	for (const auto n : cups) {
		result = result * 10 + n;
	}

	return result;
}

result_t part2(const data_t data) {
	// pad out the data to be 1 million cups...
	data_t million_cups(data.begin(), data.end());
	for (size_t n = 10; n <= 1000000; n++) {
		million_cups.push_back(n);
	}

	// part 2, 10 million moves
	auto cups = move_cups(million_cups, 10000000);

	// result is the product of the two cups clockwise of cup 1.
	return cups[0] * cups[1];
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
