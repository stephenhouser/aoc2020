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
#include "split.h"	// split strings

using namespace std;

struct node_t {
	string value = "";
	node_t* left = NULL;
	node_t* right = NULL;
};

/* Update with data type and result types */
using data_t = vector<string>;
using result_t = size_t;

/* for pretty printing durations */
using duration_t = chrono::duration<double, milli>;


/* Read the data file... */
const vector<string> tokenize(const string& line) {
	vector<string> tokens;

	auto parts = split(line);
	for (auto part : parts) {

		while (part.starts_with('(')) {
			tokens.push_back("(");
			part = part.substr(1);
		}

		size_t close = part.find(')');
		if (close != string::npos) {
			tokens.push_back(part.substr(0, close));
		} else {
			tokens.push_back(part);
		}

		while (part.ends_with(')')) {
			tokens.push_back(")");
			part = part.substr(0, part.size() - 1);
		}
	}
	return tokens;
}

/*
	expr : term ((+ | *) term)*
	term : number | OPEN expr CLOSE
*/
class parser_t {
	public:
		vector<string> tokens = {};
		size_t pos = 0;
		string empty = "";

	parser_t(const vector<string>& equation) {
		tokens = equation;
		pos = 0;
	}

	const string& token() {
		if (pos < tokens.size()) {
			return tokens[pos];
		} else {
			return empty;
		}
	}

	node_t* parse_number() {
		// print("parse_number -> {}\n", token());
		node_t* node = new node_t();
		node->value = token();
		pos++;

		return node;
	}

	node_t* parse_term() {
		assert(tokens.size() > 0);
		// print("parse_term -> {}\n", token());

		node_t* node = NULL;
		if (token() == "(") {
			assert(tokens.size() > 1);

			pos++;
			node = parse_expr();

			assert(token() == ")");
			pos++;
		} else {
			node = parse_number();
		}

		return node;
	}

	node_t* parse_expr() {
		// print("parse_expr -> {}\n", token());

		node_t* node = parse_term();
		while (token() == "+" || token() == "*") {
			node_t* parent = new node_t();
			parent->left = node;

			parent->value = token();
			pos++;

			parent->right = parse_term();
			node = parent;
		}

		return node;
	}
};

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

size_t evaluate(const node_t* node) {
	const node_t* left = node->left;
	const node_t* right = node->right;

	if (left != NULL && right != NULL) {
		if (node->value == "+") {
			return evaluate(left) + evaluate(right);
		}

		if (node->value == "*") {
			return evaluate(left) * evaluate(right);
		}
	}

	return stoul(node->value);
}

result_t part1(const data_t& equations) {
	size_t result = 0;

	for (const auto& equation : equations) {
		auto tokens = tokenize(equation);
		auto parser = new parser_t(tokens);
		auto eq_tree = parser->parse_expr();

		size_t local_result = evaluate(eq_tree);
		print("local={}\n", local_result);
		result += local_result;
	}

	return result;
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
