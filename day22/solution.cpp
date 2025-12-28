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
#include <deque>
#include <unordered_set>


using namespace std;

/* Update with data type and result types */
using data_t = vector<vector<size_t>>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

bool recursive_combat(deque<size_t>& p1, deque<size_t>& p2);

/* Read the data file... */
const data_t read_data(const string& filename) {
	std::ifstream ifs(filename);

	data_t data;
	vector<size_t> deck;

	string line;
	while (getline(ifs, line)) {
		if (line.empty()) {
			data.push_back(deck);
			deck.clear();
		} else if (isdigit(line[0])) {
			deck.push_back(stoul(line));
		}
	}

	data.push_back(deck);
	return data;
}

/* Return the score for the given deck as per the instructions. */
result_t score_deck(deque<size_t> deck) {
	result_t score = 0;

	size_t m = 1;
	for (auto it = deck.rbegin(); it != deck.rend(); it++) {
		score += m++ * *it;
	}

	return score;
}

/* Play a round of combat, adjust the decks accordingly.
 * Recurse to recursive_combat() if recursive combat is enabled to play a sub-game.
 */
void play_round(deque<size_t>& p1, deque<size_t>& p2, bool recursive = false) {
	assert(!p1.empty() && !p2.empty());

	size_t c1 = p1.front();
	p1.pop_front();

	size_t c2 = p2.front();
	p2.pop_front();

	bool p1_wins = c1 > c2;
	if (recursive && p1.size() >= c1 && p2.size() >= c2) {
		// recurse when enabled and there are enough cards in each deck
		deque<size_t> r1(p1.begin(), p1.begin() + static_cast<long int>(c1));
		deque<size_t> r2(p2.begin(), p2.begin() + static_cast<long int>(c2));
		p1_wins = recursive_combat(r1, r2);
	}

	if (p1_wins) {
		p1.push_back(c1);
		p1.push_back(c2);
	} else {
		p2.push_back(c2);
		p2.push_back(c1);
	}
}

/* Return true if player 1 wins this game, false if player 2 wins.
 * Update decks accordingly.
 */
bool combat(deque<size_t>& p1, deque<size_t>& p2) {
	while (!p1.empty() && !p2.empty()) {
		play_round(p1, p2);
	}

	return p2.empty();
}

/* Part 1 */
result_t part1(const data_t& data) {
	assert(data.size() == 2);

	deque<size_t> p1(data[0].begin(), data[0].end());
	deque<size_t> p2(data[1].begin(), data[1].end());

	auto &winner = combat(p1, p2) ? p1 : p2;
	return score_deck(winner);
}

/* Return a hash of the two decks suitable to cache maintenance. */
size_t state_hash(const deque<size_t>& deck1, const deque<size_t>& deck2) {
	size_t hash = deck1.size();

	for(auto& i : deck1) {
		hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	hash ^= deck2.size() + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	for(auto& i : deck2) {
		hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	return hash;
}

/* Return true if player 1 wins this game, false if player 2 wins.
 * Enable recursive combat, update decks accordingly, and player 1 wins
 * if we encounter a repeated state (as in the instructions).
 */
bool recursive_combat(deque<size_t>& p1, deque<size_t>& p2) {
	unordered_set<size_t> cache;

	while (!p1.empty() && !p2.empty()) {
		size_t state = state_hash(p1, p2);
		if (cache.contains(state)) {
			return true;	// player 1 wins when we repeat states
		}

		cache.insert(state);
		play_round(p1, p2, true);
	}

	return p2.empty();
}

/* Part 2 */
result_t part2(const data_t& data) {
	assert(data.size() == 2);
	
	deque<size_t> p1(data[0].begin(), data[0].end());
	deque<size_t> p2(data[1].begin(), data[1].end());

	auto &winner = recursive_combat(p1, p2) ? p1 : p2;
	return score_deck(winner);
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
