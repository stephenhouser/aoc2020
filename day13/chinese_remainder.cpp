
#include "chinese_remainder.h"
#include <vector>	// std::vector
#include <cassert>	// assert()

// Returns modulo inverse of a
// with respect to m using
// extended Euclid Algorithm.
// Refer below post for details:
// https://www.geeksforgeeks.org/
// multiplicative-inverse-under-modulo-m/
long int mod_inverse(long int a, long int m) {
	long int m0 = m, t, q;
	long int x0 = 0, x1 = 1;

	if (m == 1) {
		return 0;
	}

	// Apply extended Euclid Algorithm
	while (a > 1) {
		q = a / m;	// q is quotient
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

	return x1;
}

// Returns the smallest number x such that:
// x % num[0] = rem[0],
// x % num[1] = rem[1],
// ..................
// x % num[k-2] = rem[k-1]
// Assumption: Numbers in num[] are pairwise co-prime (gcd for every pair is 1)
long int chinese_remainder(std::vector<long int> remainders, std::vector<long int> numbers) {
	assert(numbers.size() == remainders.size());

	size_t k = numbers.size();

	// Compute product of all numbers
	long int prod = 1;
	for (size_t i = 0; i < k; i++) {
		prod *= numbers[i];
	}

	// Initialize result
	long int result = 0;
	for (size_t i = 0; i < k; i++) {
		long int pp = prod / numbers[i];
		result += remainders[i] * mod_inverse(pp, numbers[i]) * pp;
	}

	return result % prod;
}
