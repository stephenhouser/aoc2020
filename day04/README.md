# Day 4: Passport Processing

## Part 1

Just brute force "business logic" validation

## Part 2

Same thing as part 1, just more validation. Another instance of needed to read
the directions closely, `byr` could be max `2002` and I had coded in `2020`.

Using a template for `reduce()` and it refactored well. I should include it in my base template somewhere. Perhaps a `map`, `filter`, and `reduce`. This one allows a function to be passed and the output (reduced) type to be different than the accumulation type.

```

template <typename T, typename U>
T reduce(const std::vector<U> vec, const T start, std::function<T(T, U)> func) {
    return std::accumulate(vec.begin(), vec.end(), start, func);
}
```

and used

```
size_t valid = reduce<size_t, passport_t>(data, (size_t)0, [](size_t a, const passport_t& b) {
    return a + (b.validate() ? 1 : 0);
});
```
