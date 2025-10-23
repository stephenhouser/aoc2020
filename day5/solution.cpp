#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <vector>		// collectin
#include <string>		// strings
#include <ranges>		// ranges and views
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <functional>
#include <print>

#include "split.h"

using namespace std;

using data_t = vector<string>;
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

  // this seems slightly faster than the loop below.
  std::transform(src.begin(), src.end(), std::back_inserter(dst), func);

  //for (const auto &e : src) {
  //  dst.emplace_back(func(e));
  //}

  return dst;
}

/* Part 1 */
result_t decode_pass(const string &pass) {
  result_t row_min = 0;
  result_t row_max = 127;
  result_t col_min = 0;
  result_t col_max = 7;

  char last_row = 0;
  char last_col = 0;

  for (const char ch : pass) {
    switch (ch) {
      case 'F':
        row_max -= ((row_max - row_min) >> 1) + 1;
        last_row = ch;
        break;
      case 'B':
        row_min += ((row_max - row_min) >> 1) + 1;
        last_row = ch;
        break;
      case 'L':
        col_max -= ((col_max - col_min) >> 1) + 1;
        last_col = ch;   
        break;
      case 'R':
        col_min += ((col_max - col_min) >> 1) + 1;
        last_col = ch;
        break;
    }
  }

  result_t row = (last_row == 'F') ? row_min : row_max;
  result_t col = (last_col == 'L') ? col_min : col_max;
  return (row * 8) + col;
}

result_t part1(const data_t &passes) {
  /* map passes to seat ids */
  auto seats = map<string, result_t>(passes, decode_pass);

  /* find the largest one */
  auto largest_seat = *std::max_element(seats.begin(), seats.end());

	return largest_seat;
}

result_t part2([[maybe_unused]] const data_t &passes) {
  /* map passes to seat ids */
  auto seats = map<string, result_t>(passes, decode_pass);

  /* sort the seats */
  std::sort(seats.begin(), seats.end(), std::less<result_t>());

  /* loop looking for first gap in numbers */
  result_t pos = 1;
  while (seats[pos] == seats[pos - 1] + 1) {
    pos++;
  }

  /* our seat number is the number past the gap - 1 */
	return seats[pos] - 1;
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back(line);
		}
	}

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
