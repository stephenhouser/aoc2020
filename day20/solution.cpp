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

	const regex rx(re, std::regex::ECMAScript);
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


struct tile_t {
	size_t id = 0;
	vector<size_t> neighbors = {};
	string tile_data = "";

	string tile_map = "";

	const unordered_set<size_t> get_edges() const {
		unordered_set<size_t> edges;

		const vector<string> edge_rxs = {
			"^([\\.#]+)\n",		// top
			"[\\.#]+([\\.#])",	// right
			"([\\.#]+)\n+$", 	// bottom
			"([\\.#])[\\.#]+",	// left
		};
	
		for (const auto& rx : edge_rxs) {
			const string edge_text = match_combine(tile_data, rx);
			edges.insert(parse_edge(edge_text));
			edges.insert(parse_edge_reverse(edge_text));
		}
	
		return edges;
	}

	const string get_tile_map() const {
		vector<string> rows;
		smatch match;
	
		const regex rx("\n[\\\\.#]([\\\\.#]+)[\\\\.#]");
		auto begin = sregex_iterator(tile_data.begin(), tile_data.end(), rx);
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
			result.append("\n");
		}
	
		return result;
	}
};

void print_tile(const tile_t& tile) {
	print("{}: ", tile.id);
	for (const auto &n : tile.neighbors) {
		print("{}, ", n);
	}
	print("\n");
	print("{}", tile.tile_data);
}

/* Update with data type and result types */
using data_t = unordered_map<size_t, tile_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Parse the raw text defineing a tile to a `tile_t` struct */
const tile_t parse_tile(const string& raw_text) {
	tile_t tile;

	tile.id = stoul(match_combine(raw_text, "^Tile (\\d+):\n"));
	tile.tile_data = raw_text.substr(raw_text.find_first_of("\n")+1);
	return tile;
}

/* Return a map of all edges to their corresponding tiles */
unordered_map<size_t, unordered_set<size_t>> get_edges(const data_t& tiles) {
	unordered_map<size_t, unordered_set<size_t>> edges;

	for (const auto& [id, tile] : tiles) {
		for (const auto edge : tile.get_edges()) {
			edges[edge].insert(tile.id);
		}
	}

	return edges;
}

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t tiles;

	std::ifstream ifs(filename);

	string raw_tile;
	string line;
	while (getline(ifs, line)) {
		if (line.empty()) {
			tile_t tile = parse_tile(raw_tile);
			tiles[tile.id] = tile;
			raw_tile.clear();
		} else {
			raw_tile.append(line);
			raw_tile.append("\n");
		}
	}

	tile_t tile = parse_tile(raw_tile);
	tiles[tile.id] = tile;

	// calculate neighbors based on edges
	unordered_map<size_t, unordered_set<size_t>> neighbors_of;
	auto edges = get_edges(tiles);

	for (const auto& [edge, neighbor_set] : edges) {
		assert(neighbor_set.size() <= 2);

		if (neighbor_set.size() == 2) {
			const vector<size_t> neighbors(neighbor_set.begin(), neighbor_set.end());
			neighbors_of[neighbors[0]].insert(neighbors[1]);
			neighbors_of[neighbors[1]].insert(neighbors[0]);
		}
	}

	// add neighbors to tiles
	for (auto& [id, tile] : tiles) {
		// tile.neighbors = neighbors_of[tile.id];
		vector<size_t> neighbors(neighbors_of[tile.id].begin(), neighbors_of[tile.id].end());
		tile.neighbors = neighbors;
	}

	return tiles;
}

size_t hashit(size_t x, size_t y) {
	return (x & 0xFFFFul << 16) | (y & 0xFFFFul);
}

/* Part 1 */
result_t part1(const data_t& tiles) {
	auto corners = tiles 
		| views::filter([](const auto& tile) { return tile.second.neighbors.size() == 2; })
		| views::transform([](const auto& tile) { return tile.first; })
		| views::common;
		
	size_t result = std::accumulate(corners.begin(), corners.end(), 1ul, multiplies());
	return result;
}

vector<size_t> intersection(const vector<size_t>& a, const vector<size_t>& b) {
	vector<size_t> isect;
	for (const auto& a_elem : a) {
		for (const auto b_elem : b) {
			if (a_elem == b_elem) {
				isect.push_back(a_elem);

			}
		}
	}

	return isect;
}

vector<size_t> unplaced_neighbors(const data_t& tiles, unordered_set<size_t>& unplaced, size_t tid) {
	vector<size_t> neighbors;

	const auto& tile = tiles.at(tid);
	for (const auto n : tile.neighbors) {
		if (unplaced.contains(n)) {
			neighbors.push_back(n);
		}
	}

	return neighbors;
}

result_t part2(const data_t& tiles) {
	auto corners = tiles 
		| views::filter([](const auto& tile) { return tile.second.neighbors.size() == 2; })
		| views::transform([](const auto& tile) { return tile.first; })
		| ranges::to<vector<size_t>>();

	// map of tile location hashit(x,y) -> tile_id
	unordered_map<size_t, size_t> tile_map;

	unordered_set<size_t> unplaced;
	for (const auto& [id, tile] : tiles) {
		unplaced.insert(id);
	}

	size_t y = 0;
	size_t x = 0;

	const tile_t tile = tiles.at(corners[0]);
	tile_map[hashit(x,y)] = tile.id;
	unplaced.erase(tile.id);

	print("Top  : {}\n", tile.id);

	auto top_n = unplaced_neighbors(tiles, unplaced, tile.id);
	if (top_n.size() == 2) {
		auto right = top_n.back();
		top_n.pop_back();

		tile_map[hashit(x+1,y)] = right;
		unplaced.erase(right);
		print("right: {}\n", right);	
	}

	if (top_n.size() == 1) {
		auto down = top_n.back();
		top_n.pop_back();

		tile_map[hashit(x,y+1)] = down;
		unplaced.erase(down);
		print("down : {}\n", down);
	}

	auto common = intersection(right_tile.neighbors, down_tile.neighbors);
	for (const auto diag : common) {
		if (unplaced.contains(diag)) {
			print("diag : {}\n", diag);
			break;
		}
	}

	// find tile with 


	// vector<size_t> queue;
	// queue.push_back(corners[0]);	// 2 neighbors
	
	// while (queue.size()) {
	// 	size_t tile_id = queue.back();
	// 	queue.pop_back();

	// 	// place tile
	// 	map[col][row] = tile_id;

	// 	// remove this tile from neighbors
	// 	for (const auto neighbor_id : neighbors[tile_id]) {
	// 		neighbors[neighbor_id].erase(tile_id);
	// 	}

	// 	if (neighbors[tile_id].size() == 0) {
	// 		print("End Map\n");
	// 		break;
	// 	}

	// 	if (neighbors[tile_id].size() == 1) {
	// 		print("End Row\n");
	// 		row++;
	// 		continue;
	// 	}

	// 	if (neighbors[tile_id].size() == 2) {

	// 	}
	// }	
	// print("{}\n", map[0][0]);
	return tiles.size();
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
