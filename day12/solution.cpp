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

const string direction_name(const point_t& dir) {
	// Can only work with unit vectors
	assert(dir.x == 0 || dir.x == 1 || dir.x == -1);
	assert(dir.y == 0 || dir.y == 1 || dir.y == -1);

	// change in direction when rotating left in 45 degree steps
	// East (1,0) -> North East (1,-1) -> North (0,1), etc...
	vector<point_t> directions = {
		{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
	};

	vector<string> names = {
		"east", "northeast", "north", "northwest", "west", "southwest", "south", "southeast"
	};

	// find offset of vector's direction in direction table
	size_t dir_n = 0;
	for ( ; dir_n < directions.size(); dir_n++) {
		if (directions[dir_n] == dir) {
			return names[dir_n];
		}
	}

	return "unknown";
}

vector_t rotate_left(const vector_t& v, long int angle) {
	// Can only work with unit vectors
	assert(v.dir.x == 0 || v.dir.x == 1 || v.dir.x == -1);
	assert(v.dir.y == 0 || v.dir.y == 1 || v.dir.y == -1);

	// Can only work with positive angles
	if (angle < 0) {
		angle = 360 - angle;
	}

	// change in direction when rotating left in 45 degree steps
	// East (1,0) -> North East (1,-1) -> North (0,1), etc...
	vector<point_t> directions = {
		{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
	};

	// find offset of vector's direction in direction table
	size_t dir_n = 0;
	for ( ; dir_n < directions.size(); dir_n++) {
		if (directions[dir_n] == v.dir) {
			break;
		}
	}

	assert(dir_n < directions.size());

	while (angle > 0) {
		dir_n = (dir_n + 1) % directions.size();
		angle -= 45;
	}

	assert(angle == 0);
	return {v.p, directions[dir_n]};
}

vector_t rotate_right(const vector_t& v, long int angle) {
	return rotate_left(v, 360 - angle);
}


/* Part 1 */
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
			assert(distance == 90 || distance == 180 || distance == 270);
			next = rotate_right(v, distance);
			break;
		case 'L':
			assert(distance == 90 || distance == 180 || distance == 270);
			next = rotate_left(v, distance);
			break;
	}

	return next;
}

result_t part1(const data_t& data) {
	vector_t v = {{0, 0}, {1, 0}}; 	// 0,0 facing east
	for (const auto& i : data) {
		v = move(i, v);
		print("{}{:3}: east {}, north {}; facing {}\n", i.op, i.distance, v.p.x, v.p.y, direction_name(v.dir));
	}

	return (result_t)manhattan_distance({0, 0}, v.p);
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
