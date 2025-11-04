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

	// returns true if no field contains n
	auto invalid_number = [&fields, field_contains](const size_t n) {
		return ranges::none_of(fields, field_contains(n));
	};

	// transform a ticket to a vector of invalid numbers in the ticket
	auto invalid_ticket_numbers = [&fields, invalid_number](const ticket_t& ticket) {
		return ticket | views::filter(invalid_number);
	};

	auto invalid_numbers = tickets
		| views::transform(invalid_ticket_numbers)	// to invalid numbers
		| views::join								// flatten to 1D
		| ranges::to<vector<size_t>>();

	return reduce<size_t, size_t>(invalid_numbers, 0ul, std::plus<size_t>());
}

/* Part 2*/
bool number_in_field(const field_t& field, const size_t number) {
	return (field.low.min <= number && number <= field.low.max) 
		|| (field.high.min <= number && number <= field.high.max);
}


auto field_contains = [](size_t n) {
	return [n](const field_t& field) {
		return (field.low.min <= n && n <= field.low.max) 
			|| (field.high.min <= n && n <= field.high.max);
	};
};

auto valid_tickets(const vector<field_t>& fields, const vector<ticket_t>& tickets) {
	// return true if field does not contain n
	auto valid_ticket = [&fields](const ticket_t& ticket) {
		// all numbers on ticket are valid in a field
		return ranges::all_of(ticket, [&fields](const size_t n) {
			// number on ticket is valid in any field
			return ranges::any_of(fields, field_contains(n));
		});
	};

	return tickets | views::drop(1) | views::filter(valid_ticket);
}

unordered_set<size_t> intersection(const unordered_set<size_t>& a, const unordered_set<size_t>& b){
	unordered_set<size_t> i;

	for (const auto& e : a) {
		if (b.contains(e)) {
			i.insert(e);
		}
	}

	return i;
}

void print_set(const unordered_set<size_t>& s) {
	for (const auto n : s) {
		print("{},", n);
	}
}

result_t part2(const data_t& data) {
	const auto fields = data.fields;
	const auto tickets = data.tickets;

	const auto valid = valid_tickets(fields, tickets)
		| ranges::to<vector<ticket_t>>();

	// vector of ticket positions and the set if field indexes that
	// are possible for the position
	vector<unordered_set<size_t>> possible(fields.size());

	for (const auto& ticket : valid) {
		// for each ticket position
		for (size_t pos = 0; pos < ticket.size(); pos++) {
			const auto ticket_n = ticket[pos];	// number at position

			// find all possible fields for this ticket_n
			unordered_set<size_t> flds;
			for (size_t f = 0; f < fields.size(); f++) {
				const auto& field = fields[f];
				if (number_in_field(field, ticket_n)) {
					flds.insert(f);
				}
			}

			if (possible[pos].size() == 0) {
				// just starting out, add all possible fields
				possible[pos] = flds;
			} else {
				// replace with intersection of current possible fields
				// and existing possible fields
				possible[pos] = intersection(possible[pos], flds);
			}
		}
	}

	// Deduce the correct fileds for each position
	// Look for a position with only one possible field mapping
	// Add that to ticket_fields map, then remove it from all other
	// position possibilities.
	// Iterate until there are no more positions to check
	unordered_map<size_t, size_t> ticket_fields;

	while (ticket_fields.size() < fields.size()) {
		// for each ticket position...
		for (size_t pos = 0; pos < possible.size(); pos++) {
			// set of possible field mappings
			auto possible_fields = possible[pos];

			// if there is only 1 possible mapping (map it!)
			if (possible_fields.size() == 1) {
				// add the unique mapping from positon to field
				auto position_field = *(possible_fields.begin());
				ticket_fields[pos] = position_field;
				// remove this possible mapping from all other positions
				for (auto & ps : possible) {
					ps.extract(position_field);
				}
			}
		}
	}

	// Use the position to field mapping to find the departure positions
	// multiply the numbers in those positions together for the result
	auto my_ticket = tickets[0];
	result_t result = 1;
	for (const auto [pos, fld] : ticket_fields) {
		if (fields[fld].name.starts_with("departure")) {
			// print("pos={} field={}({}) -> {}\n", pos, fields[fld].name, fld, my_ticket[pos]);
			result *= my_ticket[pos];
		}
	}

	// 2564529489989
	// 11559770760143 too high
	return result;
}

// result_t part2(const data_t& data) {
// 	const auto fields = data.fields;
// 	const auto tickets = data.tickets;

// 	// return true if field does not contain n
// 	auto field_contains = [](size_t n) {
// 		return [n](const field_t& field) {
// 			return (field.low.min <= n && n <= field.low.max) 
// 				|| (field.high.min <= n && n <= field.high.max);
// 		};
// 	};

// 	// returns fields that contain the number
// 	auto fields_for_number = [&fields, field_contains](const size_t n) {
// 		return fields | views::transform(field_contains(n));
// 	};

// 	// returns if ticket is valid
// 	auto valid_ticket = [fields_for_number](const ticket_t& ticket) {
// 		auto valid_numbers = ticket 
// 			| views::transform(fields_for_number)		// [] of fields number appears in
// 			| views::transform([](const auto &in_fields) {	// number appears in any field
// 				return ranges::any_of(in_fields, [](const auto b) { return b; });
// 			});

// 		// all numbers in ticket are valid;
// 		return ranges::all_of(valid_numbers, [](const auto b) { return b; });
// 	};

// 	vector<ticket_t> valid = data.tickets 
// 		| std::views::filter(valid_ticket) 
// 		| std::ranges::to<vector<ticket_t>>();

// 	return valid.size();
// }

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
