# Advent of Code 2020

| Day | Part 1 | Part 2 | Title |
|---:|:-----:|:-----:|:-----|
| 1  | ⭐ | 🌟 | [Day 1: Report Repair](https://adventofcode.com/2020/day/1) |
| 2  | ⭐ | 🌟 | [Day 2: Password Philosophy](https://adventofcode.com/2020/day/2) |
| 3  | ⭐ | 🌟 | [Day 3: Toboggan Trajectory](https://adventofcode.com/2020/day/3) |
| 4  | ⭐ | 🌟 | [Day 4: Passport Processing](https://adventofcode.com/2020/day/4) |
| 5  | ⭐ | 🌟 | [Day 5: Binary Boarding](https://adventofcode.com/2020/day/5) |
| 6  | ⭐ | 🌟 | [Day 6: Custom Customs](https://adventofcode.com/2020/day/6) |
| 7  | ⭐ | 🌟 | [Day 7: Handy Haversacks](https://adventofcode.com/2020/day/7) |
| 8  | ⭐ | 🌟 | [Day 8: Handheld Halting](https://adventofcode.com/2020/day/8)   |
| 9  |    |    |    |
| 10 |    |    |    |
| 11 |    |    |    |
| 12 |    |    |    |
| 13 |    |    |    |
| 14 |    |    |    |
| 15 |    |    |    |
| 16 |    |    |    |
| 17 |    |    |    |
| 18 |    |    |    |
| 19 |    |    |    |
| 20 |    |    |    |
| 21 |    |    |    |
| 22 |    |    |    |
| 23 |    |    |    |
| 24 |    |    |    |
| 25 |    |    |    |

Contains my solutions to the [Advent of Code 2020](https://adventofcode.com/2020).

Tools:

- C++ 23 (STL only)
- Visual Studio Code
- macOS / Linux (primary)
- git

## Running

There is a top level makefile that can be used to make each day or a `summary`

- `make day5` will make day5 and test against the live `input.txt` in that directory.
- `make summary` will make all days and run against the `test.txt` and `input.txt` in each directory.

When developing, within each day's directory.

- `make` or `make test` will build and run `test.txt` which is sample input from the problem
- `make input` will build and run `input.txt` which is the live input for the problem

To debug, change the `Makefile`, remove `-O3` and replace with `-g`. Then use GDB
or modify the launch configuration in `.vscode/launch.json` for the appropriate day and input file.
