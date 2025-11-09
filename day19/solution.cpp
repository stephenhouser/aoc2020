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
#include <cctype>

#include "mrf.h"	// map, reduce, filter templates
#include "split.h"	// split strings

using namespace std;

struct rule_t {
	size_t id = 0;
	char ch = ' ';
	vector<size_t> left = {};
	vector<size_t> right = {};

	rule_t(const string& rule) {
		auto parts = split(rule, ":|\"");
		id = stoul(parts[0]);
		if (isdigit(parts[1][1])) {
			left = split_size_t(parts[1]);
			if (parts.size() > 2) {
				right = split_size_t(parts[2]);
			}
		} else {
			ch = parts[2][0];
		}
	}
};

using rules_t = unordered_map<size_t, rule_t>;
using message_t = string;
using messages_t = vector<message_t>;

/* Update with data type and result types */
using data_t = pair<rules_t, messages_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	rules_t rules;
	messages_t messages;
	
	std::ifstream ifs(filename);

	bool reading_rules = true;

	string line;
	while (getline(ifs, line)) {
		if (line.empty()) {
			reading_rules = false;
		} else if (reading_rules) {
			rule_t rule(line);
			rules.emplace(rule.id, rule);
		} else {
			messages.push_back(line);
		}
	}

	return {rules, messages};
}

// trys to match all rules in vector
// return 0 if failed, or number of tokens consumed from message
size_t match_rule(const rules_t& rules, const size_t rule_n, const string& message);

size_t match_rules(const rules_t& rules, const vector<size_t> match_r, const string& message) {
	size_t consumed = 0;
	string msg = message;

	// all the rules in the rule set given must match
	for (const auto rule_n : match_r) {
		size_t local = match_rule(rules, rule_n, msg);
		if (local == 0) {
			return 0;
		}

		consumed += local;
		msg = msg.substr(local);
	}

	return consumed;
}

size_t match_rule(const rules_t& rules, const size_t rule_n, const string& message) {
	rule_t rule = rules.at(rule_n);

	// rule is a leaf node, it it matches it consumes a token, if not, 0
	if (rule.ch != ' ') {
		return rule.ch == message[0] ? 1 : 0;
	}

	// rule is not a leaf node, see if the left or right match
	size_t matches = match_rules(rules, rule.left, message);
	if (matches == 0) {
		matches = match_rules(rules, rule.right, message);
	}

	return matches;
}

/* Part 1 */
result_t part1(const data_t& data) {
	const auto& rules = data.first;
	const auto& messages = data.second;

	result_t matches = 0;
	for (const auto& message : messages) {
		result_t local = match_rule(rules, 0, message);

		// print("{} -> {}\n", message, local);
		matches += (local == message.size()) ? 1 : 0ul;
	}

	return matches;
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
