#if !defined(SPLIT_H)
#define SPLIT_H

#include <cstring>	 // strtok, strdup
#include <iostream>	 // cout
#include <string>	 // std::string
#include <vector>	 // std::vector

extern std::vector<std::string> split(const std::string& str, const std::string& delims = ", =;");
extern std::vector<int> split_int(const std::string& str, const std::string& delims = ", =;");
extern std::vector<size_t> split_size_t(const std::string& str, const std::string& delims = ", =;");
extern std::vector<float> split_float(const std::string& str, const std::string& delims = ", =;");

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	for (auto it = v.begin(); it != v.end(); it++) {
		if (it != v.begin()) {
			os << ",";
		}
		os << *it;
	}
	return os;
}

extern void test_split();

#endif
