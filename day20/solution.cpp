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

size_t parse_edge_reverse(string text) {
	reverse(text.begin(), text.end());
	return parse_edge(text);
}


struct t_tile {
	size_t id = 0;
	vector<string> data = {};

	string _left = "";
	string _right = "";

	t_tile(size_t id, const vector<string>& data) : id(id), data(data) {
		for (const auto& row : data) {
			_left += row[0];
		}

		for (const auto& row : data) {
			_right += row[size()-1];
		}
	}

	const string map_data(const size_t row) {
		return data[row].substr(1, size()-2);
	}

	size_t size() const {
		return data[0].size();
	}

	const string top() const {
		return data[0];
	}

	const string bottom() const {
		return data[size()-1];
	}

	const string left() const {
		return _left;
	}

	const string right() const {
		return _right;
	}

	void print() const {
		std::print("{}:\n", this->id);
		for (const auto &l : this->data) {
			std::print("{}\n", l);
		}
	}
};

t_tile rotate(const t_tile& tile) {
	vector<string> rotated;
	size_t dim = tile.data.size();

	for (size_t x = 0; x < dim; x++) {
		rotated.push_back("");
	}

	for (size_t y = 0; y < dim; y++) {
		for (size_t x = 0; x < dim; x++) {
			rotated[(dim-1)-x] += tile.data[y][x];
		}
	}

	return {tile.id, rotated};
}

t_tile flip(const t_tile& tile) {
	vector<string> flipped;

	for (size_t y = tile.data.size(); y != 0; y--) {
		flipped.push_back(tile.data[y-1]);
	}

	return {tile.id, flipped};
}

vector<t_tile> tile_transforms(const t_tile& tile) {
	t_tile work = tile;
	vector<t_tile> tiles = {work, flip(work)};

	for (size_t i =0; i < 3; i++) {
		work = rotate(work);
		tiles.push_back(work);
		tiles.push_back(flip(work));
	}

	return tiles;
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

	void print() const {
		std::print("{}: ", this->id);
		for (const auto &n : this->neighbors) {
			std::print("{}, ", n);
		}
		// std::print("\n");
		// std::print("{}", this->tile_data);
	}
};

t_tile from_tile(const tile_t& tile) {
	vector<string> data = split(tile.tile_data, "\n");
	return {tile.id, data};	
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
unordered_map<size_t, unordered_set<size_t>> get_edge_map(const data_t& tiles) {
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
	auto edges = get_edge_map(tiles);

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


size_t find_above(const size_t id, const vector<t_tile>& all) {
	const t_tile &tile = all[id];

	for (size_t i = 0; i < all.size(); i++) {
		const t_tile& other = all[i];
		if (tile.id != other.id && tile.top() == other.bottom()) {
			return i;
		}
	}

	return all.size();
}

size_t find_left(const size_t id, const vector<t_tile>& all) {
	const t_tile &tile = all[id];

	for (size_t i = 0; i < all.size(); i++) {
		const t_tile& other = all[i];
		if (tile.id != other.id && tile.left() == other.right()) {
			return i;
		}
	}

	return all.size();
}

size_t find_right(const size_t id, const vector<t_tile>& all) {
	const t_tile &tile = all[id];

	for (size_t i = 0; i < all.size(); i++) {
		const t_tile& other = all[i];
		if (tile.id != other.id && tile.right() == other.left()) {
			return i;
		}
	}

	return all.size();
}

size_t find_below(const size_t id, const vector<t_tile>& all) {
	const t_tile &tile = all[id];

	for (size_t i = 0; i < all.size(); i++) {
		const t_tile& other = all[i];
		if (tile.id != other.id && tile.bottom() == other.top()) {
			return i;
		}
	}

	return all.size();
}

size_t find_corner(const vector<t_tile>& all) {
	for (size_t id = 0; id < all.size(); id++) {
		if (find_above(id, all) == all.size() && find_left(id, all) == all.size()) {
			return id;
		}
	}

	return all.size();
}

result_t part2(const data_t& tiles) {
	auto corners = tiles 
		| views::filter([](const auto& tile) { return tile.second.neighbors.size() == 2; })
		| views::transform([](const auto& tile) { return tile.first; })
		| ranges::to<vector<size_t>>();

	// convert all tiles to t_tiles
	vector<t_tile> t_tiles;
	for (const auto& tile : tiles) {
		t_tiles.push_back(from_tile(tile.second));
	}

	// generate vector of all tile rotations and flips
	vector<t_tile> all_tiles;
	for (const auto& tile : t_tiles) {
		const auto tf = tile_transforms(tile);
		all_tiles.insert(all_tiles.end(), tf.begin(), tf.end());
	}

	print("tiles={}, all_tiles={}", t_tiles.size(), all_tiles.size());

	// for (const auto& tile : all_tiles) {
	// 	tile.print();
	// 	print("\n");
	// }

	vector<vector<t_tile>> final;

	size_t id = find_corner(all_tiles);
	while (id < all_tiles.size()) {
		vector<t_tile> row;

		t_tile& tile = all_tiles[id];
		row.push_back(tile);
		// print("{} ", tile.id);
		
		size_t right = find_right(id, all_tiles);
		while (right < all_tiles.size()) {
			t_tile& right_tile = all_tiles[right];
			row.push_back(right_tile);	
			// print("{} ", tile.id);

			right = find_right(right, all_tiles);
		}

		// print("\n");

		final.push_back(row);
		id = find_below(id, all_tiles);
	}

	print("\n");
	for (const auto& t_tiles : final) {
		for (const t_tile& tile : t_tiles) {
			print("{} ", tile.id);
		}
		print("\n");
	}

	size_t final_dim = final.size() - 1;
	result_t result = final[0][0].id 
					* final[final_dim][0].id
					* final[0][final_dim].id
					* final[final_dim][final_dim].id;



	return result;
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
