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

using namespace std;

struct node_t {
	size_t label;
	node_t *next;

	node_t(size_t label, node_t * next) : label(label), next(next) {
	}
};


/* Update with data type and result types */
using data_t = vector<size_t>;
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
			for (const char ch : line) {
				data.push_back((size_t)(ch-'0'));
			}	
		}
	}

	return data;
}

node_t *to_list(const vector<size_t> & cups) {
	node_t * head = NULL;
	node_t * tail = NULL;

	for (const auto n : cups) {
		node_t * node = new node_t(n, head);
		if (head == NULL) {
			head = node;
		} else {
			tail->next = node;
		}

		tail = node;
	}

	return head;
}

void free_list(node_t * head) {
	node_t * prev = head;
	node_t * cur = prev->next;

	while (cur != head) {
		free(prev);
		
		prev = cur;
		cur = cur->next;
	}

	free(prev);
}

result_t score_cups(const node_t * head) {
	const node_t * first = head;
	while (first->label != 1) {
		first = first->next;
	}

	size_t result = 0;
	for (const node_t * current = first->next; current != first; current = current->next) {
		result = result * 10 + current->label;
	}

	return result;
}

void print_cups(const node_t * head) {
	const node_t * current = head;
	for (; current->next && current->next != head; current = current->next) {
		print("{}, ", current->label);
	}
	print("{}", current->label);
}

node_t *pick(node_t * head, size_t to_pick = 3) {
	node_t * picked = head->next;

	node_t * current = head;
	for (size_t n = 0; n < to_pick; n++) {
		current = current->next;
	}

	head->next = current->next;
	current->next = NULL;

	return picked;
}

node_t * find_cup(node_t * head, size_t label) {
	node_t * current = head;
	for (; current->next && current->next != head; current = current->next) {
		if (current->label == label) {
			return current;
		}
	}

	return current->label == label ? current : NULL;
}

node_t * find_destination(node_t * head, size_t label) {
	node_t * dest = NULL;

	do {
		label = (label == 1) ? 9 : (label - 1);
		dest = find_cup(head, label);
	} while (dest == NULL);

	return dest;
}

/* Splice list in after node */
void splice(node_t * node, node_t * list) {
	assert(node != NULL);
	assert(list != NULL);

	node_t * tail = node->next;
	node->next = list;

	while (list->next != NULL) {
		list = list->next;
	}

	list->next = tail;
}

node_t *move_cups(node_t * start, size_t moves) {
	node_t * current = start;

	for (size_t m = 0; m < moves; m++) {
		// print("\n-- move {} --\ncups: ", m+1);
		// print_cups(current);

		node_t * holding = pick(current);

		// print("\npick: ");
		// print_cups(holding);

		node_t * destination = find_destination(current, current->label);

		// print("\ndestination: {}\n", destination->label);

		splice(destination, holding);		

		// print("new: ");
		// print_cups(current);
		// print("\n");

		current = current->next;
	}

	return current;
}

size_t find_destination(const vector<size_t> &cups, size_t current) {
	size_t label = (current == 1 ? cups.size() : current) - 1;

	while (cups[current] == label
			|| cups[cups[current]] == label 
			|| cups[cups[cups[current]]] == label) {

		label = (label == 1 ? cups.size() : label) - 1;
	}

	return label;
}

void print_it(const vector<size_t>& cups, size_t current) {
	print("({}) ", current);
	for (size_t next = current; cups[next] != current; next = cups[next]) {
		print("{} ", cups[next]);
	}

	// for (size_t i = 1; i < cups.size(); i++) {
	// 	if (i == current) {
	// 		print("({}) ", cups[i]);
	// 	} else {
	// 		print("{} ", cups[i]);
	// 	}
	// }
}

result_t score_it(const vector<size_t> &cups) {
	result_t result = 0;
	for (size_t c = cups[1]; c != 1; c = cups[c]) {
		result = result * 10 + c;
	}

	return result;
}

/* Part 1 */
result_t part1(const data_t data) {

	print("data: {}\n", data);

	// vector where label is index and value is location of next cup
	vector<size_t> cups(data.size()+1);
	for (size_t i = 0; i < data.size(); i++) {
		size_t label = data[i];
		size_t next = data[(i + 1) % data.size()];
		cups[label] = next;
	}

	print("cups: {}\n", cups);

	size_t current = data[0];
	for (size_t m = 0; m < 100; m++) {
		print("-- move {} --\ncups: ", m+1);
		print_it(cups, current);
		print("\n");

		auto destination = find_destination(cups, current);
		
		print("destination: {}\n", destination);

		auto splice_start = cups[current];
		auto splice_end   = cups[cups[cups[current]]];

		// splice out next 3 from current position
		cups[current] = cups[splice_end];

		// add in at destination
		cups[splice_end] = cups[destination];
		cups[destination] = splice_start;

		current = cups[current];
	}

	return score_it(cups);

	// node_t *cups = to_list(data);

	// cups = move_cups(cups, 100);

	// result_t result = score_cups(cups);

	// free_list(cups);
	// return result;
}

result_t part2(const data_t data) {
	return data.size();
	// vector<size_t> vec(data.begin(), data.end());
	// for (size_t n = 10; n <= 1000000; n++) {
	// 	vec.push_back(n);
	// }

	// node_t *cups = to_list(vec);

	// // print_cups(cups);

	// cups = move_cups(cups, 10000000);

	// node_t * cup1 = find_cup(cups, 1);

	// print("a1 = {}, a2 = {}\n", cup1->next->label, cup1->next->next->label);

	// result_t result = cup1->next->label * cup1->next->next->label;

	// free_list(cups);
	// return result;
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
