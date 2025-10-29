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
#include <vector>  // collectin

#include "mrf.h"	// map, reduce, filter templates
#include "point.h"
#include "vector.h"

using namespace std;

/* Update with data type and result types */
using result_t = size_t;

struct instruction_t {
	char op;
	long int distance;
};

using data_t = vector<instruction_t>;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			char instruction = line[0];
			long int distance = stol(line.substr(1));
			data.push_back({instruction, distance});
		}
	}

	return data;
}

vector_t move(const instruction_t& instr, const vector_t& v) {
	vector_t next = v;
	auto distance = instr.distance;
	switch (instr.op) {
		case 'N':
			next.p += {0, 1 * distance};
			break;
		case 'S':
			next.p += {0, -1 * distance};
			break;
		case 'E':
			next.p += {1 * distance, 0};
			break;
		case 'W':
			next.p += {-1 * distance, 0};
			break;
		case 'F':
			next.p += {(v.dir.x * distance), (v.dir.y * distance)};
			break;
		case 'R':
			next.turn_right(distance);
			break;
		case 'L':
			next.turn_left(distance);
			break;
	}

	return next;
}

/* Part 1 */
result_t part1(const data_t& data) {
	vector_t ship = {{0, 0}, {1, 0}}; 	// 0,0 facing east
	for (const auto& i : data) {
		ship = move(i, ship);
		// print("{}{:3}: east {}, north {}; facing {}\n", i.op, i.distance, ship.p.x, ship.p.y, ship.direction_name());
	}

	// return distance of ship from origin
	return (result_t)manhattan_distance({0, 0}, ship.p);
}

/* Part 2 */

result_t part2(const data_t& data) {
	vector_t ship = {{0, 0}, {1, 0}};
	vector_t waypoint = {{10, 1}, {0, 0}};	// waypoint is relative to the ship

	for (const auto& i : data) {
		switch (i.op) {
			case 'F': 	// move ship to waypoint
				ship.p += {waypoint.p.x * i.distance, waypoint.p.y * i.distance};
				break;
			case 'L':
				waypoint.p.rotate_left(i.distance);
				break;
			case 'R':
				waypoint.p.rotate_right(i.distance);
				break;
			default:	// move the waypoint
				waypoint = move(i, waypoint);
				break;
		}

		// print("{}{:3}: ", i.op, i.distance);
		// print("ship: east {}, north {}; facing {} ", ship.p.x, ship.p.y, direction_name(ship.dir));
		// print("waypoint: east {}, north {}\n", waypoint.p.x, waypoint.p.y);
	}

	// return distance of ship from origin
	return (result_t)manhattan_distance({0, 0}, ship.p);
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
