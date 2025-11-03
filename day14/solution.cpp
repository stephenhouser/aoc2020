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
#include <unordered_map>

#include "mrf.h"	// map, reduce, filter templates
#include "split.h"	// split strings

using namespace std;

bool verbose = false;

/*
mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
mem[8] = 11
*/

struct cpu_t {
	string mask = "";
	unordered_map<size_t, size_t> memory = {};
};

enum operator_t {
	unknown, mask, mem
};

struct instruction_t {
	operator_t op;
	size_t address;	// address or set_bits
	size_t value;	// value or clear_bits;
	std::string mask;
};

/* Update with data type and result types */
using data_t = vector<std::string>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back(line);
		}
	}

	return data;
}

/* Part 1 */
size_t parse_mask(const string& mask, const char value) {
	size_t bits = 0x00;
	for (const char c : mask) {
		bits <<= 1;
		bits |= (c == value) ? 1 : 0;
	}

	return bits;
}

void set_mask(cpu_t& cpu, const std::string& mask) {
	cpu.mask = mask;
}

void set_memory(cpu_t& cpu, size_t address, size_t value) {
	size_t mask_set = parse_mask(cpu.mask, '1');
	size_t mask_clear = parse_mask(cpu.mask, '0');
	cpu.memory[address] = (value | mask_set) & ~mask_clear;
}

size_t memory_sum(const cpu_t& cpu) {
	size_t total = 0;
	for (const auto& [address, value] : cpu.memory) {
		total += value;
	}
	
	return total;
}

instruction_t decode_instruction(const std::string& line) {
	if (line.starts_with("mask")) {
		return {mask, 0, 0, line.substr(7)};
	}

	if (line.starts_with("mem")) {
		auto parts = split(line, "[] =");
		auto address = stoul(parts[1]);
		auto value = stoul(parts[2]);
		return {mem, address, value, ""};
	}

	return {unknown, 0x00, 0x00, ""};
}

result_t part1(const data_t& instructions) {
	cpu_t cpu;

	for (const auto& instr_raw : instructions) {
		auto instr = decode_instruction(instr_raw);
		switch (instr.op) {
			case mask:
				set_mask(cpu, instr.mask);
				break;
			case mem:
				set_memory(cpu, instr.address, instr.value);
				break;
			default:
				break;
		}
	}

	return memory_sum(cpu);
}
/* Part 2*/
size_t set_bit(size_t address, size_t bit_n) {
	size_t mask = 0x01ul << bit_n;
	return address | mask;
}

size_t clear_bit(size_t address, size_t bit_n) {
	size_t mask = ~(0x01ul << bit_n);
	return address & mask;
}

void set_memory(cpu_t& cpu, size_t address, const string& mask, size_t value) {
	size_t x_pos = mask.find("X");
	if (x_pos == string::npos) {
		size_t mask_set = parse_mask(cpu.mask, '1');
		cpu.memory[(address | mask_set)] = value;
		// print("{:036B} = {}\n", (address | mask_set), value);
	} else {
		size_t bit_pos = mask.size() - x_pos - 1;
		string new_mask(mask);
		new_mask[x_pos] = '0';	// set mask to pass this bit
		set_memory(cpu, set_bit(address, bit_pos), new_mask, value);
		set_memory(cpu, clear_bit(address, bit_pos), new_mask, value);
	}
}

bool mask_check(const string& mask) {
	auto floaters = std::ranges::count(mask, 'X');
	return floaters < 10;
}

result_t part2(const data_t& instructions) {
	cpu_t cpu;

	for (const auto& instr_raw : instructions) {
		auto instr = decode_instruction(instr_raw);
		switch (instr.op) {
			case mask:
				if (mask_check(instr.mask)) {
					set_mask(cpu, instr.mask);
				} else {
					if (verbose) {
						print("ERROR: Too many floating bits in mask\n");
					}
					break;
				}
				break;
			case mem:
				set_memory(cpu, instr.address, cpu.mask, instr.value);
				break;
			default:
				break;
		}
	}

	return memory_sum(cpu);
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
