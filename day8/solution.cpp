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

#include "split.h"	// split strings

using namespace std;

struct instruction_t {
	string op;
	long int operand;
};

struct cpu_t {
	long int a = 0;
	size_t pc = 0;

	size_t clock = 0;
};

/* Update with data type and result types */
using data_t = vector<instruction_t>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;

/* Read the data file... */
const data_t read_data(const string& filename) {
	data_t program;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			auto parts = split(line);

			if (parts.size() == 2) {
				auto op = parts[0];
				int operand = stoi(parts[1]);
				program.push_back({op, operand});
			}
		}
	}

	return program;
}

void print_program(const data_t& program) {
	for (const auto& [op, operand] : program) {
		print("{} {}\n", op, operand);
	}
}

void print_cpu(const cpu_t& cpu) {
	print("{}, pc={}, a={}\n", cpu.clock, cpu.pc, cpu.a);
}

void execute_instruction(const instruction_t& instr, cpu_t& cpu) {
	cpu.clock++;

	if (instr.op == "jmp") {
		cpu.pc = (size_t)((long int)cpu.pc + instr.operand);
		return;
	}

	if (instr.op == "acc") {
		cpu.a += instr.operand;
	} else if (instr.op == "nop") {
		;
	}

	cpu.pc++;
}

/* Runs until an instruction
 *  - is executed a second time (e.g. a loop)
 *  - is beyond (outside) of the program instructions
 */
void run_program(const data_t& program, cpu_t& cpu) {
	vector<bool> executed(program.size());

	while (cpu.pc < program.size() && !executed[cpu.pc]) {
		// print_cpu(cpu);

		executed[cpu.pc] = true;
		execute_instruction(program[cpu.pc], cpu);
	}
}

/* Part 1 - find where the program executes an instruction twice */
result_t part1(const data_t& program) {
	cpu_t cpu;

	run_program(program, cpu);
	return (result_t)cpu.a;
}

/* Part 2 - replace nop/jmp instructions to make it run to end of program */
result_t part2([[maybe_unused]] const data_t& program) {
	/* Replace each nop/jmp starting from the first one and execute.
	 * repeat until we get one that runs past the end of the program.
	 */
	for (size_t pos = 0; pos < program.size(); pos++) {
		auto op = program[pos].op;

		if (op != "acc") {
			// make a test program with this operation switched
			auto test = program;
			test[pos].op = op == "nop" ? "jmp" : "nop";

			// run the test program
			cpu_t cpu;
			run_program(test, cpu);

			// if it  ran to the end of the program, it is correct
			if (cpu.pc >= program.size()) {
				return (result_t)cpu.a;
			}
		}
	}

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
