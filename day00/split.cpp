#include "split.h"

#include <cstring>	 // strtok, strdup
#include <fstream>	 // ifstream (reading file)
#include <iostream>	 // cout
#include <ranges>

using namespace std;

std::vector<std::string> split(const std::string& str, const std::string& delims) {
	std::vector<std::string> tokens;
	char* str_c{strdup(str.c_str())};
	char* token{NULL};

	token = strtok(str_c, delims.c_str());
	while (token != NULL) {
		tokens.push_back(token);
		token = strtok(NULL, delims.c_str());
	}

	free(str_c);
	return tokens;
}

std::vector<float> split_float(const std::string& str, const std::string& delims) {
	std::vector<float> numbers;

	for (const auto& s : split(str, delims)) {
		if (isdigit(s[0]) || s[0] == '-') {
			numbers.push_back(std::stof(s));
		}
	}

	return numbers;
}

std::vector<int> split_int(const std::string& str, const std::string& delims) {
	std::vector<int> numbers;

	for (const auto& s : split(str, delims)) {
		if (isdigit(s[0]) || s[0] == '-') {
			numbers.push_back(std::stoi(s));
		}
	}

	return numbers;
}

std::vector<size_t> split_size_t(const std::string& str, const std::string& delims) {
	std::vector<size_t> numbers;

	for (const auto& s : split(str, delims)) {
		if (isdigit(s[0])) {
			numbers.push_back(std::stoul(s));
		}
	}

	return numbers;
}

std::vector<int> split_int_2(const std::string& str, const std::string& delims) {
	std::vector<int> numbers;

	auto strings = split(str, delims);
	std::for_each(strings.begin(), strings.end(), [&numbers](const std::string& s) {
		if (isdigit(s[0]) || s[0] == '-') {
			numbers.push_back(std::stoi(s));
		}
	});

	return numbers;
}

std::vector<int> split_int_3(const std::string& str, const std::string& delims) {
	auto strings = split(str, delims);
	auto numbers = strings |
				   std::views::filter([](const std::string& s) {
					   return isdigit(s[0]) || s[0] == '-';
				   }) |
				   std::views::transform([](const std::string& s) {
					   return std::stoi(s);
				   }) |
				   std::ranges::to<std::vector<int>>();

	return numbers;
}

void test_split() {
	cout << "Testing split functions..." << endl;

	string test_str = "12, 34;56=78 abc -90";

	auto str_tokens = split(test_str);
	cout << "String tokens:" << endl;
	for (const auto& token : str_tokens) {
		cout << "[" << token << "]" << endl;
	}

	auto int_tokens = split_int(test_str);
	cout << "Integer tokens:" << endl;
	for (const auto& token : int_tokens) {
		cout << "[" << token << "]" << endl;
	}

	auto float_tokens = split_float(test_str, ", =;");
	cout << "Float tokens:" << endl;
	for (const auto& token : float_tokens) {
		cout << "[" << token << "]" << endl;
	}

	cout << "Split functions test completed." << endl;
}
