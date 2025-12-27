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

struct tile_t {
	size_t id = 0;
	vector<string> data = {};

	string _left = "";
	string _right = "";

	tile_t(const string& raw) {
		const auto parts = split(raw, "\n");

		const size_t id_start = parts[0].find(' ');
		const size_t id_end = parts[0].find(':');
		id = stoul(parts[0].substr(id_start, id_end-id_start));

		data.insert(data.begin(), parts.begin()+1, parts.end());

		update_edges();
	}

	tile_t(size_t id, const vector<string>& data) : id(id), data(data) {
		update_edges();
	}

	void update_edges() {
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

/* Return a new tile (with the same id) rotated 90 degrees. */
tile_t rotate(const tile_t& tile) {
	size_t dim = tile.data.size();
	vector<string> rotated(dim);

	for (size_t y = 0; y < dim; y++) {
		for (size_t x = 0; x < dim; x++) {
			rotated[(dim-1)-x] += tile.data[y][x];
		}
	}

	return {tile.id, rotated};
}

/* Return a new tile (with the same id) with rows reversed (flipped) */
tile_t flip(const tile_t& tile) {
	vector<string> flipped(tile.data.rbegin(), tile.data.rend());

	return {tile.id, flipped};
}

/* Return a vector containing all possible variations(transformations)
 * of the original tile. All rotations and flips.
 */
vector<tile_t> tile_transforms(const tile_t& tile) {
	tile_t work = tile;
	vector<tile_t> tiles = {work, flip(work)};

	for (size_t i =0; i < 3; i++) {
		work = rotate(work);
		tiles.push_back(work);
		tiles.push_back(flip(work));
	}

	return tiles;
}

/* Update with data type and result types */
using data_t = vector<tile_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t tiles;

	std::ifstream ifs(filename);

	string raw_tile;
	string line;
	while (getline(ifs, line)) {
		if (line.empty()) {
			const tile_t &tile = tile_t(raw_tile);
			tiles.push_back(tile);

			raw_tile.clear();
		} else {
			raw_tile.append(line);
			raw_tile.append("\n");
		}
	}

	const tile_t &tile = tile_t(raw_tile);
	tiles.push_back(tile);

	return tiles;
}

/* Find tile index of tile that fits above of tile with index `idx` in `all` 
 * tiles or size of the `all` tile vector to signal not-found.
 */
size_t find_above(const size_t id, const vector<tile_t>& all) {
	const tile_t &tile = all[id];

	for (size_t i = 0; i < all.size(); i++) {
		const tile_t& other = all[i];
		if (tile.id != other.id && tile.top() == other.bottom()) {
			return i;
		}
	}

	return all.size();
}

/* Find tile index of tile that fits left of tile with index `idx` in `all`
 * tiles or size of the `all` tile vector to signal not-found.
 */
size_t find_left(const size_t id, const vector<tile_t>& all) {
	const tile_t &tile = all[id];

	for (size_t i = 0; i < all.size(); i++) {
		const tile_t& other = all[i];
		if (tile.id != other.id && tile.left() == other.right()) {
			return i;
		}
	}

	return all.size();
}

/* Find tile index of tile that fits right of tile with index `idx` in `all`
 * tiles or size of the `all` tile vector to signal not-found.
 */
size_t find_right(const size_t idx, const vector<tile_t>& all) {
	const tile_t &tile = all[idx];

	for (size_t i = 0; i < all.size(); i++) {
		const tile_t& other = all[i];
		if (tile.id != other.id && tile.right() == other.left()) {
			return i;
		}
	}

	return all.size();
}

/* Find tile index of tile that fits below tile with index `idx` in `all`
 * tiles or size of the `all` tile vector to signal not-found.
 */
size_t find_below(const size_t idx, const vector<tile_t>& all) {
	const tile_t &tile = all[idx];

	for (size_t i = 0; i < all.size(); i++) {
		const tile_t& other = all[i];
		if (tile.id != other.id && tile.bottom() == other.top()) {
			return i;
		}
	}

	return all.size();
}

size_t find_corner(const vector<tile_t>& all) {
	for (size_t idx = 0; idx < all.size(); idx++) {
		if (find_above(idx, all) == all.size() && find_left(idx, all) == all.size()) {
			return idx;
		}
	}

	return all.size();
}

vector<vector<tile_t>> arrange_tiles(const vector<tile_t> &tiles) {

	// All tile orientations (rotations and flips) for all tiles
	vector<tile_t> all;
	for (const auto& tile : tiles) {
		const auto tf = tile_transforms(tile);
		all.insert(all.end(), tf.begin(), tf.end());
	}

	// Start with top left (any corner that has nothing left or above)
	// left_idx is the tile index in all that is the leftmost tile for the row
	// Add the leftmost tile to the arranged matrix
	//	then search for the tile that goes to the right of it
	//	insert and iterate until we don't find a right tile.
	// Look for the tile that goes below the leftmost tile
	//	make it the new leftmost and repeat until we don't find any below tiles
	vector<vector<tile_t>> arranged;

	size_t left_idx = find_corner(all);
	while (left_idx < all.size()) {
		vector<tile_t> row;

		tile_t& left_tile = all[left_idx];
		row.push_back(left_tile);
		
		size_t right_idx = find_right(left_idx, all);
		while (right_idx < all.size()) {
			tile_t& right_tile = all[right_idx];
			row.push_back(right_tile);	

			right_idx = find_right(right_idx, all);
		}

		arranged.push_back(row);
		left_idx = find_below(left_idx, all);
	}

	return arranged;
}

/* Part 1 */
result_t part1(const data_t& tiles) {	
	const auto arranged = arrange_tiles(tiles);

	/* Product of the four corner tile ids */
	size_t size = arranged.size() - 1;
	result_t result = arranged[0][0].id 
					* arranged[size][0].id
					* arranged[0][size].id
					* arranged[size][size].id;

	return result;
}


result_t part2(const data_t& tiles) {
	const auto arranged = arrange_tiles(tiles);

	size_t size = arranged.size() - 1;
	result_t result = arranged[0][0].id 
					* arranged[size][0].id
					* arranged[0][size].id
					* arranged[size][size].id;

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
