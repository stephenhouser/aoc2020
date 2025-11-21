
#if !defined(CHINESE_REMAINDER_H)
#define CHINESE_REMAINDER_H

#include <numeric>	//	gcd() and lcm()
#include <vector>	 // std::vector
#include <cstdint>

constexpr auto lcm(auto x, auto... xs) {
	return ((x = std::lcm(x, xs)), ...);
}

constexpr auto gcd(auto x, auto... xs) {
	return ((x = std::gcd(x, xs)), ...);
}

template <typename T>
T lcm(const std::vector<T>& vec) {
	T x = 1;

	for (T x_n : vec) {
		x = lcm(x, x_n);
	}

	return x;
}

extern size_t chinese_remainder(std::vector<size_t> numbers, std::vector<size_t> remainders);

#endif