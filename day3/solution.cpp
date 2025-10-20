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

#include "charmap.h"

using namespace std;

using data_t = charmap_t;
using result_t = size_t;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

result_t run_trail(const data_t &map, const point_t &move) {
	size_t trees = 0;
	point_t pos = {0, 0};
	while (pos.y < map.size_y) {
		if (map.is_char(pos, '#')) {
			trees++;
		}

		pos.x = (pos.x + move.x) % map.size_x;
		pos.y = (pos.y + move.y);
	}

	return trees;
}

vector<result_t> run_trails(const data_t &map, const vector<point_t> &moves) {
	vector<result_t> trees;

	for (const auto &move : moves) {
		trees.push_back(run_trail(map, move));
	}

	return trees;
}

template <typename T>
T reduce(std::vector<T> vec, const T &start, std::function<T(T, T)> func) {
	return std::accumulate(vec.begin(), vec.end(), start, func);
}

/* Part 1 */
result_t part1(const data_t &map) {
	return run_trail(map, {3, 1});
}

result_t part2(const data_t &map) {
	const vector<point_t> moves = {{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}};
	const vector<size_t> path_trees = run_trails(map, moves);

	size_t trees = reduce<size_t>(path_trees, 1, [](size_t a, size_t b) { return a * b; });
	return trees;
}

const data_t read_data(const string &filename) {
	return charmap_t::from_file(filename);
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
