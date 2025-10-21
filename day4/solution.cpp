#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <vector>		// collectin
#include <string>		// strings
#include <ranges>		// ranges and views
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <unordered_map>
#include <functional>
#include <regex>

#include "split.h"

using namespace std;


struct passport_t {
	std::unordered_map<std::string, std::string> fields = {};

	bool validate_fields() const {
		bool required_fields = fields.find("byr") != fields.end() 
				&& fields.find("iyr") != fields.end() 
				&& fields.find("eyr") != fields.end()
				&& fields.find("hgt") != fields.end()
				&& fields.find("hcl") != fields.end() 
				&& fields.find("ecl") != fields.end()
				&& fields.find("pid") != fields.end();
		return required_fields && (fields.size() == 7 || (fields.contains("cid") && fields.size() == 8));
	}

	bool validate_number(const string &field_name, size_t digits = 0) const {
		auto field = fields.find(field_name);
		if (field != fields.end()) {
			std::regex rx("^[0-9]*$");
			std::smatch match;
			if (std::regex_match(field->second, match, rx)) {
				if (digits) {
					return field->second.size() == digits;
				}

				return true;
			}
		}

		return false;
	}

	bool validate_hex(const string &field_name) const {
		auto field = fields.find(field_name);
		if (field != fields.end() && field->second.size() == 7) {
			std::regex rx("^#[0-9a-f]{6}$");
			std::smatch match;
			if (std::regex_match(field->second, match, rx)) {
				return true;
			}
		}

		return false;
	}

	bool validate_year(const string &field_name, int min_value, int max_value) const {
		auto field = fields.find(field_name);
		if (field != fields.end() && field->second.size() == 4) {
			int value = stoi(field->second.c_str());
			if (min_value <= value && value <= max_value) {
				return true;
			}
		}

		return false;
	}

	bool validate_string(const string &field_name, const vector<string> &allowed) const {
		auto field = fields.find(field_name);
		if (field != fields.end() && field->second.size() == 3) {
			string value = field->second.c_str();
			return std::find(allowed.begin(), allowed.end(), value) != allowed.end();
		}

		return false;
	}

	bool validate_height(const string &field_name) const {
		auto field = fields.find(field_name);
		if (field != fields.end()) {
			std::regex rx("^([0-9]+)(cm|in)$");
			std::smatch match;
			if (std::regex_match(field->second, match, rx)) {
				int height = stoi(match[1].str());
				string unit = match[2].str();
				if (unit == "in") {
					return 59 <= height && height <= 76;
				} else if (unit == "cm") {
					return 150 <= height && height <= 193;
				}
			}
		}

		return false;
	}

	bool validate() const {
		return validate_fields()
			&& validate_year("byr", 1920, 2002)
			&& validate_year("iyr", 2010, 2020)
			&& validate_year("eyr", 2020, 2030)
			&& validate_height("hgt")
			&& validate_hex("hcl")
			&& validate_string("ecl", {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"})
			&& validate_number("pid", 9);
	}
};

void show_field(const passport_t& passport, const string& field_name) {
	cout << field_name << ": \"";
	auto field = passport.fields.find(field_name);
	if (field != passport.fields.end()) {
		cout << field->second;
	}
	cout << "\" ";
}

using data_t = vector<passport_t>;
using result_t = size_t;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

template <typename T, typename U>
T reduce(const std::vector<U> vec, const T start, std::function<T(T, U)> func) {
	return std::accumulate(vec.begin(), vec.end(), start, func);
}

/* Part 1 */
result_t part1(const data_t &data) {
	size_t valid = reduce<size_t, passport_t>(data, (size_t)0, [](size_t a, const passport_t& b) {
		return a + (b.validate_fields() ? 1 : 0);
	});

	return valid;
}

// 117 too high
result_t part2([[maybe_unused]] const data_t &data) {
	// for (auto &passport : data) {
	// 	cout << (passport.validate() ? "* " : "x ");
	// 	for (const auto& [key, value] : passport.fields) {
	// 		cout << key << ":" << value << " ";
	// 	}
	// 	cout << endl;
	// }

	size_t valid = reduce<size_t, passport_t>(data, (size_t)0, [](size_t a, const passport_t& b) {
		return a + (b.validate() ? 1 : 0);
	});

	return valid;
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	passport_t passport;
	while (getline(ifs, line)) {
		for (const auto& pair : split(line, " ")) {
			vector<string> field = split(pair, ":");
			passport.fields[field[0]] = field[1];
		}

		if (line.empty()) {
			data.push_back(passport);
			passport.fields.clear();
		}
	}
	data.push_back(passport);

	return data;
}

template <typename T>
void print_result(T result, chrono::duration<double, milli> duration) {
	const int time_width = 10;
	const int time_precision = 4;
	const int result_width = 15;

	cout << std::setw(result_width) << std::right << result;

	cout << " ("
		 << std::setw(time_width) << std::fixed << std::right 
		 << std::setprecision(time_precision)
		 << duration.count() << "ms)";
	cout << endl;
}

int main(int argc, char *argv[]) {
	const char *input_file = argv[1];
	if (argc < 2) {
		input_file = "test.txt";
	}

    auto start_time = chrono::high_resolution_clock::now();

	auto data = read_data(input_file);

	auto parse_time = chrono::high_resolution_clock::now();
	print_result("parse", (parse_time - start_time));

	result_t p1_result = part1(data);

	auto p1_time = chrono::high_resolution_clock::now();
	print_result(p1_result, (p1_time - parse_time));

	result_t p2_result = part2(data);

	auto p2_time = chrono::high_resolution_clock::now();
	print_result(p2_result, (p2_time - p1_time));

	print_result("total", (p2_time - start_time));
}
