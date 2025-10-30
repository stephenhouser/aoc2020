/* map(), reduce(), and filter() meta-functions */
#include <algorithm>  // sort
#include <functional>
#include <ranges>  // ranges and views
#include <vector>  // collection

/* Reduce vector of U to single T using func */
template <typename T, typename R>
R reduce(const std::vector<T> vec, const R start, std::function<R(T, R)> func) {
	return std::accumulate(vec.begin(), vec.end(), start, func);
}

/* Example using reduce()
 * NOTE: it can do a mapping or filter along the way as the in and out types differ
 * and you define the function.
 *
 * Make a function that adds the value of pass to accum
 *
 *	size_t add_pass(const size_t accum, const string &pass) { return accum + ...; }
 *
 * The use the reduce passing the function, make sure to include the types!
 *
 *	auto total = reduce<string, size_t>(passes, 0, add_pass);
 */

/* Map vector of S to vector of D using func */
template <typename T, typename R>
std::vector<R> map(const std::vector<T>& src,
				   const std::function<R(const T&)> func) {
	std::vector<R> dst;
	dst.reserve(src.size());
	std::transform(src.begin(), src.end(), std::back_inserter(dst), func);
	return dst;
}

/* Example using map()
 *
 * Make a function that maps a single object; here string -> size_t
 *
 *	size_t decode_pass(const string &pass) { return 0; }
 *
 * The use the map passing the function, make sure to include the types!
 *
 *	auto seats = map<string, size_t>(passes, decode_pass);
 */

/* filter vector to a new vector for all that match func. */
template <typename T>
std::vector<T> filter(const std::vector<T> src,
					  const std::function<bool(const T&)> func) {
	std::vector<T> filtered;
	std::copy_if(src.begin(), src.end(), std::back_inserter(filtered), func);
	return filtered;
}

/* Example using filter()
 *
 *	auto possible = filter<instruction_t>(program, [](const instruction_t &instr) {
 *		return instr.op != "acc";
 *	});
 */

/* Enumerate the vector V resulting in a vector of pairs (index, value)
 * 	using I as index type and V as value type
 */
template <typename I, typename V>
std::vector<std::pair<I, V>> enumerate(const std::vector<V>& vec) {
	std::vector<std::pair<I, V>> pairs;

	for (size_t i = 0; i < vec.size(); i++) {
		pairs.push_back({static_cast<I>(i), vec[i]});
	}

	return pairs;
}

/* Example of using enumerate 
	std::vector<int> v = {1, 2, 3 };
	auto e = enumerate<size_t, int>(v);

	Now e will be a vector of pairs:
		[ {0, 1}, {1, 2}, {2, 3} ]
*/