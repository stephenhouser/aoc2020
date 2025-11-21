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
#include <regex>
#include <unordered_map>

#include "mrf.h"	// map, reduce, filter templates
#include "split.h"	// split strings

using namespace std;


struct tile_t {
	size_t id = 0;
	vector<size_t> sides = {};

	bool has_side(const size_t pattern) {
		// this is the any() function
		for (const auto side : sides) {
			if (pattern == side) {
				return true;
			}
		}

		return false;
	}
};

/* Update with data type and result types */
using data_t = vector<tile_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

void print_tile(const tile_t& tile) {
	print("Tile: {}: ", tile.id);
	for (const auto s : tile.sides) {
		print("{:010b}, ", s);
	}
	print("\n");
}

const string match_single(const string& text, const string& re) {
	smatch match;
	const regex rx(re);
	if (regex_search(text, match, rx)) {
		return match.str(1);
	}

	return "";
}

/* Matches possibly multiple instances of `re` in `text` and combines
 * the matched text into a single string that is returned.
 */
const string match_combine(const string& text, const string& re) {
	string result;
	smatch match;

	const regex rx(re);
	for (sregex_iterator it = sregex_iterator(text.begin(), text.end(), rx); it != sregex_iterator(); it++) {
		smatch match(*it);
		result.append(match.str(1));
	}

	return result;
}

size_t parse_code(const string& text) {
	size_t code = 0;
	for (const char ch : text) {
		code = code << 1;
		code |= (ch == '#') ? 0x1 : 0x0;
	}

	return code;
}

size_t reverse_code(const string& text) {
	string r(text);
	reverse(r.begin(), r.end());
	return parse_code(r);
}

const tile_t parse_tile(const string& raw_text) {
	tile_t tile;

	tile.id = stoul(match_combine(raw_text, "^Tile (\\d+):"));

	const vector<string> sides_rx = {
		":\\|([\\.#]+)\\|",		// top
		"([\\.#])\\|",			// right
		"\\|([\\.#]+)\\|$", 	// bottom
		"\\|([\\.#])",			// left
	};

	for (const auto& rx : sides_rx) {
		const string side_chars = match_combine(raw_text, rx);
		tile.sides.push_back(parse_code(side_chars));
		tile.sides.push_back(reverse_code(side_chars));
	}

	return tile;
}

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string raw_tile;

	string line;
	while (getline(ifs, line)) {
		if (line.empty()) {
			data.push_back(parse_tile(raw_tile));
			raw_tile.clear();
		} else {
			raw_tile.append(line);
			raw_tile.append("|");
		}
	}

	data.push_back(parse_tile(raw_tile));
	return data;
}

/* Part 1 */
result_t part1(const data_t& data) {
	// for (const auto& tile : data) {
	// 	print_tile(tile);
	// }

	// make map of edges -> tiles 
	unordered_map<size_t, vector<tile_t>> by_code;
	for (const auto& tile : data) {
		for (const auto code : tile.sides) {
			by_code[code].push_back(tile);
		}
	}

	// // show edges and number of tiles connected to edge
	// for (const auto& item : by_code) {
	// 	print("{:010b}: ", item.first);
	// 	for (const auto& tile : item.second) {
	// 		print("{}, ", tile.id);
	// 	}
	// 	print("\n");
	// }

	// a corner tile will only have two possible common edges
	vector<tile_t> corners;
	for (const auto& tile : data) {
		size_t neighbors = 0;
		for (const auto code : tile.sides) {
			neighbors += by_code[code].size() - 1;
		}

		if (neighbors == 4) {
			corners.push_back(tile);
		}
	}

	//
	size_t result = 1;
	for (const auto& tile : corners) {
		result *= tile.id;
		print_tile(tile);
	}


	return result;
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
