#include <unistd.h>	 // getopt

#include <algorithm>  // sort
#include <cassert>	  // assert macro
#include <chrono>	  // high resolution timer
#include <cstring>	  // strtok, strdup
#include <fstream>	  // ifstream (reading file)
#include <functional>
#include <iomanip>	 // setw and setprecision on output
#include <iostream>	 // cout
#include <numeric>	 // max, reduce, etc.
#include <print>
#include <string>  // strings
#include <unordered_map>
#include <vector>  // collectin

using namespace std;

using group_t = std::vector<std::string>;
using data_t = std::vector<group_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Reduce vector of U to single T */
template <typename T, typename R>
R reduce(const std::vector<T> vec, const R start, std::function<R(T, R)> func) {
	return std::accumulate(vec.begin(), vec.end(), start, func);
}

/* Map vector of S to vector of D */
template <typename T, typename R>
std::vector<R> map(const std::vector<T>& src,
				   const std::function<R(const T&)> func) {
	std::vector<R> dst;
	dst.reserve(src.size());
	std::transform(src.begin(), src.end(), std::back_inserter(dst), func);
	return dst;
}

const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	std::vector<string> group;
	string line;

	while (getline(ifs, line)) {
		if (!line.empty()) {
			group.push_back(line);
		} else {
			data.push_back(group);
			group.clear();
		}
	}

	data.push_back(group);
	return data;
}

void show_groups(const data_t& groups) {
	for (const auto& group : groups) {
		for (const auto& response : group) {
			std::print("{}\n", response);
		}
		std::print("\n");
	}
}

/* Part 1 */
unordered_map<char, result_t> unique_answers(const vector<string>& group) {
	unordered_map<char, result_t> responses;

	for (const auto& response : group) {
		for (const auto ch : response) {
			responses[ch]++;
		}
	}

	return responses;
}

result_t unique_answers_count(const vector<string>& group) {
	auto answers = unique_answers(group);
	return answers.size();
}

result_t common_answers_count(const vector<string>& group) {
	auto answers = unique_answers(group);

	// a.
	// filter items from map that match the group size
	// then count the number of itesm
	// b.
	// transform from map<char, int> to vector<int> where is 1 if char occurs group.size() times
	// then reduce (sum) the 1s
	// c.
	// reduce from a map<char, int> to result_t of items that int matches group size
	result_t common = 0;

	for (const auto& answer : answers) {
		if (answer.second == group.size()) {
			common++;
		}
	}

	return common;
}

result_t part1(const data_t& groups) {
	// map groups to unique answers in the group
	auto answers = map<vector<string>, result_t>(groups, unique_answers_count);
	// reduce to sum of all counts
	return reduce<result_t, result_t>(answers, 0, std::plus<result_t>());
}

result_t part2([[maybe_unused]] const data_t& groups) {
	// map groups to unique answers in the group
	auto answers = map<vector<string>, result_t>(groups, common_answers_count);
	// reduce to sum of all counts
	return reduce<result_t, result_t>(answers, 0, std::plus<result_t>());
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