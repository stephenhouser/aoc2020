#include <getopt.h>	 // getopt

#include <algorithm>  // sort
#include <cassert>	  // assert macro
#include <chrono>	  // high resolution timer
#include <cstring>	  // strtok, strdup
#include <fstream>	  // ifstream (reading file)
#include <numeric>	  // max, reduce, etc.
#include <print>
#include <ranges>  // ranges and views
#include <regex>
#include <string>  // strings
#include <unordered_map>
#include <vector>  // collection
#include <unordered_set>

#include "mrf.h"	// map, reduce, filter templates
#include "split.h"	// split strings

using namespace std;

struct tile_t {
	size_t id = 0;
	vector<size_t> edges = {};
	string tilemap = "";
	size_t orientation = 0;

	bool has_edge(const size_t needle) {
		// this is the any() function
		for (const auto edge : edges) {
			if (needle == edge) {
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
	for (const auto edge : tile.edges) {
		print("{:010b}, ", edge);
	}
	print("\n");
}

/* Return the first match of regex in text */
const string match_first(const string& text, const string& re) {
	smatch match;
	const regex rx(re);
	if (regex_search(text, match, rx)) {
		return match.str(1);
	}

	return "";
}

/* Return a string with all the matches of regex in text concatenated to a string. */
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

size_t parse_edge(const string& text) {
	size_t code = 0;
	for (const char ch : text) {
		code = code << 1;
		code |= (ch == '#') ? 0x1 : 0x0;
	}

	return code;
}

size_t parse_edge_reverse(const string& text) {
	string r(text);
	reverse(r.begin(), r.end());
	return parse_edge(r);
}

// extract tile data from raw text
const string parse_tilemap(const string& text) {
	vector<string> rows;
	smatch match;

	const regex rx("\\|[\\\\.#]([\\\\.#]+)[\\\\.#]");
	auto begin = sregex_iterator(text.begin(), text.end(), rx);
	auto end = sregex_iterator();

	bool first_row = true;
	for (auto it = begin; it != end; ++it) {
		if (!first_row) {
			rows.push_back((*it).str(1));
		}

		first_row = false;
	}

	// remove last line
	rows.pop_back();

	string result = "";
	for (const auto& row : rows) {
		result.append(row);
		result.append("|");
	}

	return result;
}

/* Parse the raw text defineing a tile to a `tile_t` struct */
const tile_t parse_tile(const string& raw_text) {
	tile_t tile;

	tile.id = stoul(match_combine(raw_text, "^Tile (\\d+):"));

	const vector<string> edge_rxs = {
		":\\|([\\.#]+)\\|",		// top
		"([\\.#])\\|",			// right
		"\\|([\\.#]+)\\|$", 	// bottom
		"\\|([\\.#])",			// left
	};

	for (const auto& rx : edge_rxs) {
		const string edge_text = match_combine(raw_text, rx);
		tile.edges.push_back(parse_edge(edge_text));
		tile.edges.push_back(parse_edge_reverse(edge_text));
	}

	// gets middle tile data
	tile.tilemap = parse_tilemap(raw_text);
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
result_t part1(const data_t& tiles) {
	// make map of edges -> tiles 
	unordered_map<size_t, vector<size_t>> edges;
	for (const auto& tile : tiles) {
		for (const auto edge : tile.edges) {
			edges[edge].push_back(tile.id);
		}
	}

	auto count_neighbors = [&edges](const auto& tile) {
		size_t neighbors = 0;
		for (const auto edge : tile.edges) {
			neighbors += edges[edge].size() - 1;
		}

		return neighbors >> 1;	// divide by 2 for duplicates
	};

	auto is_corner = [&count_neighbors](const auto& tile) {
		return count_neighbors(tile) == 2;
	};

	auto corners = tiles 
		| views::filter(is_corner)
		| views::transform([](const auto& tile) { return tile.id; })
		| views::common;
		
	size_t result = std::accumulate(corners.begin(), corners.end(), 1u, multiplies());
	return result;
}

result_t part2(const data_t& tiles) {
	unordered_map<size_t, vector<size_t>> edges;
	for (const auto& tile : tiles) {
		for (const auto edge : tile.edges) {
			edges[edge].push_back(tile.id);
		}
	}

	// show edges and number of tiles connected to edge
	// for (const auto& [edge, neighbors] : edges) {
	// 	print("{:010b}: ", edge);
	// 	for (const auto& tile : neighbors) {
	// 		print("{}, ", tile.id);
	// 	}
	// 	print("\n");
	// }

	// tile_id,
	unordered_map<size_t, unordered_set<size_t>> neighbors;
	vector<size_t> corners;
	for (const auto& tile : tiles) {
		for (const auto edge : tile.edges) {
			for (const auto neighbor : edges[edge]) {
				if (neighbor != tile.id) {
					neighbors[tile.id].insert(neighbor);
				}
			}			
		}

		if (neighbors[tile.id].size() == 2) {
			corners.push_back(tile.id);
		}
	}

	print("Neighbors:\n");
	for (const auto& [tile_id, nbrs] : neighbors) {
		print("{}: ", tile_id);
		for (const auto n : nbrs) {
			print("{}, ", n);
		}
		print("\n");
	}

	assert(corners.size() == 4);
	size_t map[100][100] = { 0 };

	size_t col = 0;
	size_t row = 0;

	vector<size_t> queue;
	queue.push_back(corners[0]);	// 2 neighbors
	
	while (queue.size()) {
		size_t tile_id = queue.back();
		queue.pop_back();

		// place tile
		map[col][row] = tile_id;

		// remove this tile from neighbors
		for (const auto neighbor_id : neighbors[tile_id]) {
			neighbors[neighbor_id].erase(tile_id);
		}

		if (neighbors[tile_id].size() == 0) {
			print("End Map\n");
			break;
		}

		if (neighbors[tile_id].size() == 1) {
			print("End Row\n");
			row++;
			continue;
		}

		if (neighbors[tile_id].size() == 2) {

		}
	}	
	print("{}\n", map[0][0]);
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
