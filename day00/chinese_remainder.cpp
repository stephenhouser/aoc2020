
#include "chinese_remainder.h"
#include <vector>	// std::vector
#include <cassert>	// assert()

// Returns modulo inverse of a
// with respect to m using
// extended Euclid Algorithm.
// Refer below post for details:
// https://www.geeksforgeeks.org/
// multiplicative-inverse-under-modulo-m/
size_t mod_inverse(size_t p_a, size_t p_m) {
	long m = (long)p_m;
	long a = (long)p_a;

	long x0 = 0;
	long x1 = 1;
	long m0 = (long)p_m;
	long t;
	long q;

	if (m == 1) {
		return 0;
	}

	// Apply extended Euclid Algorithm
	while (a > 1) {
		q = (long)a / m;	// q is quotient
		t = m;

		// m is remainder now, process same as euclid's algo
		m = a % m, a = t;
		t = x0;
		x0 = x1 - q * x0;
		x1 = t;
	}

	// make x1 positive
	if (x1 < 0) {
		x1 += m0;
	}

	return (size_t)x1;
}

// Returns the smallest number x such that:
// x % num[0] = rem[0],
// x % num[1] = rem[1],
// ..................
// x % num[k-2] = rem[k-1]
// Assumption: Numbers in num[] are pairwise co-prime (gcd for every pair is 1)
size_t chinese_remainder(std::vector<size_t> remainders, std::vector<size_t> numbers) {
	assert(numbers.size() == remainders.size());

	// Compute product of all numbers
	size_t prod = 1;
	for (size_t n : numbers) {
		prod *= n;
	}

	// Initialize result
	size_t result = 0;
	for (size_t i = 0; i < numbers.size(); i++) {
		size_t pp = prod / numbers[i];
		result += remainders[i] * mod_inverse(pp, numbers[i]) * pp;
	}

	return result % prod;
}
