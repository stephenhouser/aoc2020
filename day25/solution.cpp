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
			data.push_back(stoul(line));
		}
	}

	return data;
}

/* Return the transformation of subject, iterated loops times. */
result_t transform(const size_t subject, const size_t loops) {
	result_t value = 1;

	for (size_t n = 0; n < loops; n++) {
		value = (value * subject) % 20201227;
	}

	return value;
}

/* Return the number of loops needed to generate key from subject. */
result_t crack_loops(const size_t subject, const size_t key) {
	result_t loops = 0;
	result_t value = 1;

	do {
		value = (value * subject) % 20201227;
		loops++;
	} while (value != key);

	return loops;
}

/* Part 1 */
result_t part1(const data_t& data) {
	assert(data.size() == 2);

	// Known starting subject value
	const size_t subject = 7;

	// discover the number of loops for the card key
	auto card_public = data[0];
	result_t card_loops = crack_loops(subject, card_public);

	// discover the number of loops for the door key
	auto door_public = data[1];
	result_t door_loops = crack_loops(subject, data[1]);

	// Transform to create the encryption key, check they are the same
	auto card_private = transform(door_public, card_loops);
	auto door_private = transform(card_public, door_loops);
	assert(card_private == door_private);

	return card_private;
}

/* Part 2 */
result_t part2(const data_t& data) {
	/* Do all the other problems. */
	return data.size();
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
