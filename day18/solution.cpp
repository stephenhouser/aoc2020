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
#include <regex>

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

/*
	expr : term ((+ | *) term)*
	term : number | OPEN expr CLOSE
*/
class parser_t {
	public:
		const string src;
		size_t pos;
		string token;

	 parser_t(const string& line) : src(line), pos(0), token("") {
		// initialize first token
		next();
	 }

	 // Here lies the lexer, the token giver
	 const string next() {
		 // (, ), +, *, and digits
		 const regex rx("^ *(\\(|\\)|\\*|\\+|\\d+) *", regex::ECMAScript);

		 string remainder = src.substr(pos);

		 smatch match;
		 if (regex_search(remainder, match, rx)) {
			 pos += match.str().size();
			 token = match[1].str();
			 return token;
		 }

		 return "";
	 }

	node_t* parse_number() {
		// print("parse_number -> {}\n", token());
		node_t* node = new node_t();
		node->value = token;
		next();

		return node;
	}

	node_t* parse_term() {
		// print("parse_term -> {}\n", token());
		node_t* node = NULL;
		if (token == "(") {
			next();
			node = parse_expr();

			assert(token == ")");
			next();
		} else {
			node = parse_number();
		}

		return node;
	}

	node_t* parse_expr() {
		// print("parse_expr -> {}\n", token());
		node_t* node = parse_term();
		while (token == "+" || token == "*") {
			node_t* parent = new node_t();
			parent->left = node;

			parent->value = token;
			next();

			parent->right = parse_term();
			node = parent;
		}

		return node;
	}
};

/* Evaluate an expression, return the result */
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
		auto parser = new parser_t(equation);
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
