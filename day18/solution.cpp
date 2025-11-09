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

#include "split.h"	// split strings

using namespace std;

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

/* Nodes that make up the parse tree */
struct node_t {
	string value = "";	// number, +, *, ( or )
	node_t* left = NULL;
	node_t* right = NULL;
};

class parser_t {
	public:

	parser_t(const string& line) : src(line), pos(0), token("") {
		// initialize first token
		next();
	}

	virtual ~parser_t() {
	}

	virtual node_t* parse_expr() {
		return NULL;
	}

	protected:
		const string src;
		size_t pos;
		string token;

	// Here lies the lexer, the token giver
	const string& next() {
		// (, ), +, *, and digits
		const regex rx("^ *(\\(|\\)|\\*|\\+|\\d+) *", regex::ECMAScript);

		token.clear();

		smatch match;
		string remainder = src.substr(pos);
		if (regex_search(remainder, match, rx)) {
			pos += match.str().size();
			token = match[1].str();
		}

		return token;
	}

	node_t* parse_number() {
		// print("parse_number -> {}\n", token());
		node_t* node = new node_t();
		node->value = token;
		next();

		return node;
	}

	node_t* parse_factor() {
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
};

/* Evaluate an expression represented by the parse tree, return the result */
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

/* Free the nodes in the parse tree */
void free_tree(node_t* node) {
	node_t* left = node->left;
	node_t* right = node->right;

	if (node->left) {
		free_tree(left);
	}

	if (node->right) {
		free_tree(right);
	}

	delete node;
}

/* Part 1 parser, + and * have same precedence 
	expr : term ((+ | *) term)*
	term : number | OPEN expr CLOSE
*/
class parser1_t : parser_t {
   public:

	parser1_t(const string& line) : parser_t(line) {
	}

	node_t* parse_expr() {
		// print("parse_expr -> {}\n", token());
		node_t* node = parse_factor();
		while (token == "+" || token == "*") {
			node_t* parent = new node_t();
			parent->left = node;

			parent->value = token;
			next();

			parent->right = parse_factor();
			node = parent;
		}

		return node;
	}
};

result_t part1(const data_t& equations) {
	size_t result = 0;

	for (const auto& equation : equations) {
		parser1_t parser(equation);
		auto eq_tree = parser.parse_expr();

		size_t local_result = evaluate(eq_tree);
		// print("local={}\n", local_result);
		result += local_result;
		
		free_tree(eq_tree);
	}

	return result;
}

/* Part 2 parser, + has higher precedence than *
	expr : term * term)*
	factor : (term + term)*
	term : number | OPEN expr CLOSE
*/
class parser2_t : parser_t {
   public:
	parser2_t(const string& line) : parser_t(line) {
	}

	node_t* parse_term() {
		// print("parse_term -> {}\n", token());
		node_t* node = parse_factor();
		while (token == "+") {
			node_t* parent = new node_t();
			parent->left = node;

			parent->value = token;
			next();

			parent->right = parse_factor();
			node = parent;
		}

		return node;
	}

	node_t* parse_expr() {
		// print("parse_expr -> {}\n", token());
		node_t* node = parse_term();
		while (token == "*") {
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

result_t part2(const data_t& equations) {
	size_t result = 0;

	for (const auto& equation : equations) {
		parser2_t parser(equation);
		auto eq_tree = parser.parse_expr();

		size_t local_result = evaluate(eq_tree);
		// print("local={}\n", local_result);
		result += local_result;

		free_tree(eq_tree);
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
