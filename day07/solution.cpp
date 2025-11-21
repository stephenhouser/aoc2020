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
#include <unordered_map>
#include <unordered_set>
#include <vector>  // collection

#include "split.h"	// split strings

using namespace std;

/* Update with data type and result types */
using contents_t = vector<pair<size_t, string>>;
using data_t = unordered_map<string, contents_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t rules;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			vector<string> parts = split(line, " ");
			string color = parts[0] + " " + parts[1];
			contents_t contains;

			if (parts[4] != "no") {
				for (size_t pos = 4; parts.size() >= pos + 4; pos += 4) {
					size_t count_1 = stoul(parts[pos]);
					string color_1 = parts[pos + 1] + " " + parts[pos + 2];
					contains.push_back({count_1, color_1});
				}
			}

			rules[color] = contains;
		}
	}

	return rules;
}

/* Print the rules... */
void print_rules(const data_t& rules) {
	for (const auto& [color, contents] : rules) {
		print("{}: ", color);
		for (const auto& bag : contents) {
			print("{}({}) ", bag.second, bag.first);
		}
		print("\n");
	}
}

/* Inverts the rules map to show contained by relationship instead of contains.
 * bag color -> bag colors that can contain it.
 *              the number is the number of bags that can be contained.
 */
data_t invert_rules(const data_t& rules) {
	data_t irules;

	for (const auto& [color, contents] : rules) {
		// ensure all colors exist in the inverted rules
		if (!irules.contains(color)) {
			irules[color] = {};
		}

		for (const auto& bag : contents) {
			auto bag_color = bag.second;
			irules[bag_color].push_back({bag.first, color});
		}
	}

	return irules;
}

/* Return bag colors that can contain the start color bag with the given rules.
 * Recursively descends through the rules, collecting unique bag colors, for part 1.
 */
unordered_set<string> can_contain(const data_t& rules, const string& start) {
	unordered_set<string> bags;

	for (const auto& [number, color] : rules.at(start)) {
		bags.emplace(color);
		bags.merge(can_contain(rules, color));
	}

	return bags;
}

/* Part 1 */
result_t part1(const data_t& rules) {
	// print_rules(rules);
	auto irules = invert_rules(rules);
	auto bags = can_contain(irules, "shiny gold");
	return bags.size();
}

/* Return the number of bags the start bag must contain.
 * Recursively descend through the rules counting bags, for part 2
 */
result_t contains_bags(const data_t& rules, const string& start) {
	result_t contains = 0;

	for (const auto& [number, color] : rules.at(start)) {
		contains += number + (number * contains_bags(rules, color));
	}

	return contains;
}

result_t part2(const data_t& rules) {
	// print("faded blue  : {}\n", contains_bags(rules, "faded blue"));
	// print("vibrant plum: {}\n", contains_bags(rules, "vibrant plum"));
	// print("dark olive  : {}\n", contains_bags(rules, "dark olive"));
	auto bags = contains_bags(rules, "shiny gold");
	return bags;
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
