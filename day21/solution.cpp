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
#include <unordered_map>
#include <map>
#include <unordered_set>

#include "split.h"	// split strings

using namespace std;

				// ingredients..., allergens...
using food_t = pair<unordered_set<string>, unordered_set<string>>;

/* Update with data type and result types */
using data_t = vector<food_t>;
using result_t = string;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			unordered_set<string> ingredients;
			unordered_set<string> allergens;

			bool is_ingredient = true;
			for (const auto& item : split(line, " (),")) {
				if (item == "contains") {
					is_ingredient = false;
				} else if (is_ingredient) {
					ingredients.insert(item);
				} else {
					allergens.insert(item);
				}
			}

			data.push_back({ingredients, allergens});
		}
	}

	return data;
}

unordered_set<string> intersection(const unordered_set<string> A, const unordered_set<string> B) {
	const auto& smaller_set = (A.size() <= B.size()) ? A : B;
	const auto& larger_set = (A.size() > B.size()) ? A : B;

	unordered_set<string> C;
	for (const auto & elem : smaller_set) {
		if (larger_set.contains(elem)) {
			C.emplace(elem);
		}
	}

	return C;
}

/* Return a mapping of ingredient to allergen,
 * That is which ingredient must be the named allergen
 */
unordered_map<string, string> allergen_items(const data_t& foods) {
	// map allergens -> possible ingredients that contain them
	unordered_map<string, unordered_set<string>> allergen_map;

	for (const auto& [ingredients, allergens] : foods) {

		for (const auto & allergen : allergens) {
			if (!allergen_map.contains(allergen)) {
				// if empty, set as possible items
				allergen_map[allergen] = ingredients;
			} else {
				// else set as intersection of current possible and new information
				allergen_map[allergen] = intersection(allergen_map[allergen], ingredients);
			}
		}
	}

	// iterate removing single allergen options
	// map of ingredient -> allergen name
	unordered_map<string, string> confirmed;
	while (allergen_map.size()) {
		// move 1-entry items to confirmed
		for (const auto &[allergen, ingredients] : allergen_map) {
			// if size 1 eliminate it
			if (ingredients.size() == 1) {
				auto ingredient = *(ingredients.begin());
				// move to confirmed
				confirmed[ingredient] = allergen;
				allergen_map.erase(allergen);

				// remove from all others
				for (auto & [allergen_1, ingredients_1] : allergen_map) {
					if (ingredients_1.contains(ingredient)) {
						ingredients_1.erase(ingredient);
					}
				}

				break;
			}
		}
	}

	/* Confirmed is map of ingredient -> allergen it is in*/
	return confirmed;
}

/* Part 1 */
result_t part1(const data_t& data) {
	size_t result = 0;

	// all the ingredients that are allergens (and their allergen)
	const auto allergens = allergen_items(data);

	for (const auto& [ingredients, _] : data) {
		for (const auto &ingredient : ingredients) {
			if (!allergens.contains(ingredient)) {
				result++;
			}
		}
	}

	return to_string(result);
}

bool compare_allergen(const pair<string, string>& a, const pair<string, string>& b) {
	return a.second < b.second;
}

result_t part2([[maybe_unused]] const data_t& data) {
	// all the ingredients that are allergens (and their allergen)
	const auto allergens = allergen_items(data);

	vector<pair<string, string>> dangerous(allergens.begin(), allergens.end());
	sort(dangerous.begin(), dangerous.end(), compare_allergen);

	string result;
	for (size_t i = 0; i < dangerous.size(); i++) {
		result += dangerous[i].first;
		if (i < dangerous.size() - 1) {
			result += ",";
		}
	}

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
