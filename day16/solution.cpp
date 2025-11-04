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
#include <unordered_set>

#include "mrf.h"	// map, reduce, filter templates
#include "split.h"	// split strings

using namespace std;

bool verbose = false;

struct range_t {
	size_t min = 0;
	size_t max = 0;
};

struct field_t {
	string name = "";
	range_t low = {0, 0};
	range_t high = {0, 0};
};

using ticket_t = vector<size_t>;

struct data_t {
	vector<field_t> fields = {};
	vector<ticket_t> tickets = {};
};

/* Update with data type and result types */
// using data_t = vector<size_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
field_t parse_field(const string& line) {
	field_t f;

	auto parts = split(line, ":");

	f.name = parts[0];
	
	auto more_parts = split(parts[1], " ");
	auto low_nums = split_size_t(more_parts[0], "-");
	f.low.min = low_nums[0];
	f.low.max = low_nums[1];

	auto high_nums = split_size_t(more_parts[2], "-");
	f.high.min = high_nums[0];
	f.high.max = high_nums[1];

	return f;
}

const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	size_t reading = 0; 	// 0 == fields, 1 = my ticket, 2 = tickets

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			// print("{}\n", line);
			if (line.starts_with("your ticket")) {
				reading = 1;
			} else if (line.starts_with("nearby tickets")) {
				reading = 2;
			} else {
				switch (reading) {
					case 0:
						data.fields.emplace_back(parse_field(line));
						break;
					default:
						data.tickets.emplace_back(split_size_t(line));
						break;
				}
			}
		}
	}

	return data;
}

/* Part 1 */
bool number_in_field(const field_t& field, const size_t number) {
	return (field.low.min <= number && number <= field.low.max)
		|| (field.high.min <= number && number <= field.high.max);
}

vector<size_t> validate_ticket(const vector<field_t>& fields, const ticket_t& ticket) {
	vector<size_t> invalid;

	for (const auto t : ticket) {
		bool is_valid = false;
		for (const auto& field : fields) {
			if (number_in_field(field, t)) {
				is_valid = true;
				break;
			}
		}

		if (!is_valid) {
			invalid.push_back(t);
		}
	}

	return invalid;
}

// vector<bool> in_field(const vector<field_t>& fields, size_t n) {
// 	auto in_field = [n](const field_t& field) {
// 		return (field.low.min <= n && n <= field.low.max)
// 			|| (field.high.min <= n && n <= field.high.max);
// 	};

// 	return fields | std::views::transform(in_field) | std::ranges::to<vector<bool>>();
// }

result_t part1(const data_t& data) {
	const auto fields = data.fields;
	const auto tickets = data.tickets;

	// return true if field does not contain n
	auto field_contains = [](size_t n) {
		return [n](const field_t& field) {
			return (field.low.min <= n && n <= field.low.max)
				|| (field.high.min <= n && n <= field.high.max);
		};
	};

	// returns true if no fields contain n
	auto invalid_number = [&fields, field_contains](const size_t n) {
		return ranges::none_of(fields, field_contains(n));
	};

	// creates a 2d vector of invalid numbers
	auto invalid_ticket_numbers = [&fields, invalid_number](const ticket_t& ticket) {
		return ticket 
			| views::filter(invalid_number) 
			| ranges::to<vector<size_t>>();
	};

	auto invalid_numbers = tickets
		| views::transform(invalid_ticket_numbers)
		| views::join
		| ranges::to<vector<size_t>>();

	return reduce<size_t, size_t>(invalid_numbers, 0ul, std::plus<size_t>());
}

/* Part 2*/
result_t part2(const data_t& data) {
	auto valid_ticket = [&data](const ticket_t& ticket) {
		return validate_ticket(data.fields, ticket).size() == 0;
	};

	vector<ticket_t> valid = data.tickets 
		| std::views::filter(valid_ticket) 
		| std::ranges::to<vector<ticket_t>>();

	return valid.size();
}

int main(int argc, char* argv[]) {

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
