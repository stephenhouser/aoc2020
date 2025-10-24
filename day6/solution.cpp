#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <vector>		// collectin
#include <string>		// strings
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <functional>
#include <print>
#include <unordered_map>

using namespace std;

using group_t = std::vector<std::string>;
using data_t = std::vector<group_t>;
using result_t = size_t;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

/* Reduce vector of U to single T */
template <typename T, typename R>
R reduce(const std::vector<T> vec, const R start, std::function<R(T, R)> func) {
	return std::accumulate(vec.begin(), vec.end(), start, func);
}

/* Map vector of S to vector of D */
template <typename T, typename R>
std::vector<R> map(const std::vector<T> &src,
                   const std::function<R(const T &)> func) {
  std::vector<R> dst;
  dst.reserve(src.size());
  std::transform(src.begin(), src.end(), std::back_inserter(dst), func);
  return dst;
}


void show_groups(const data_t &groups) {
  for (const auto &group : groups) {
    for (const auto &response : group) {
      std::print("{}\n", response);
    }
    std::print("\n");
  }
}

/* Part 1 */
unordered_map<char, result_t> unique_answers(const vector<string> &group) {
  unordered_map<char, result_t> responses;

  for (const auto &response : group) {
    for (const auto ch : response) {
      responses[ch]++;
    }
  }

  return responses;
}

result_t unique_answers_count(const vector<string> &group) {
  auto answers = unique_answers(group);
  return answers.size();
}

result_t common_answers_count(const vector<string> &group) {
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

  for (const auto &answer : answers) {
    if (answer.second == group.size()) {
      common++;
    }
  }

  return common;
}

result_t part1(const data_t &groups) {
  // map groups to unique answers in the group
  auto answers = map<vector<string>, result_t>(groups, unique_answers_count);
  // reduce to sum of all counts
  return reduce<result_t, result_t>(answers, 0, std::plus<result_t>());
}

result_t part2([[maybe_unused]] const data_t &groups) {
  // map groups to unique answers in the group
  auto answers = map<vector<string>, result_t>(groups, common_answers_count);
  // reduce to sum of all counts
  return reduce<result_t, result_t>(answers, 0, std::plus<result_t>());
}

const data_t read_data(const string &filename) {
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
