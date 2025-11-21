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

#include "split.h"	// split strings

using namespace std;

struct rule_t {
	size_t id = 0;
	char ch = '\0';
	vector<vector<size_t>> rules = {};

	rule_t(const string& rule) {
		auto parts = split(rule, ":|\"");
		id = stoul(parts[0]);
		if (isdigit(parts[1][1])) {
			rules.push_back(split_size_t(parts[1]));
			if (parts.size() > 2) {
				rules.push_back(split_size_t(parts[2]));
			}
		} else {
			ch = parts[2][0];
		}
	}

	rule_t(size_t id, const vector<vector<size_t>>& rules) :
		id(id), rules(rules) {
	}

	rule_t() : id(0), rules({}) {
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

/* match -- attempt to match message to a sequence of rules from the ruleset (rules)
 * message = abbbba
 * sequence = {0, 5, 6}	the rules we need to match for this to be a valid message
 * 
 * returns true if the seqence can match the message, false otherwise
*/
bool match(const rules_t& rules, const string& message, const vector<size_t>& sequence) {
	if (message.empty() || sequence.size() == 0) {
		// if both are at end we have matched!
		return message.empty() && sequence.size() == 0;
	}

	const rule_t& rule = rules.at(sequence[0]);
	const vector<size_t> remainder(sequence.begin()+1, sequence.end());

	if (rule.ch) {
		if (rule.ch == message[0]) {
			return match(rules, message.substr(1), remainder);
		} 

		return false;
	}

	for (const auto& child_rule : rule.rules) {
		// prepend the child rules to the remaining sequence
		vector<size_t> child_sequence(child_rule.begin(), child_rule.end());
		child_sequence.insert(child_sequence.end(), remainder.begin(), remainder.end());

		if (match(rules, message, child_sequence)) {
			return true;
		}
	}

	return false;
}

/* Part 1 */
result_t part1(const data_t& data) {
	const auto& rules = data.first;
	const auto& messages = data.second;

	result_t matches = 0;
	for (const auto& message : messages) {
		auto matched = match(rules, message, {0});
		// print("{} -> {}\n", message, matched);
		matches += matched ? 1 : 0ul;
	}

	return matches;
}

// 176 too low
// 265 too high
// 260 just right
result_t part2(const data_t& data) {
	rules_t rules = data.first;
	const messages_t& messages = data.second;

	// replace these two rules, as per the instructions
	// they create loops!
	rules[8] = {8, {{42}, {42, 8}}};
	rules[11] = {11, {{42, 31}, {42, 11, 31}}};

	result_t matches = 0;
	for (const auto& message : messages) {
		auto matched = match(rules, message, {0});
		// print("{} -> {}\n", message, matched);
		matches += matched ? 1 : 0ul;
	}

	return matches;
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
