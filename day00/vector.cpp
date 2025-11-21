#include "vector.h"

#include <iomanip>	 // setw and setprecision on output
#include <iostream>	 // cout
#include <sstream>	 // std::ostringstream
#include <string>	 // std::string

std::ostream& operator<<(std::ostream& os, const vector_t& p) {
	std::ostringstream buffer;
	os << p.p << "," << p.dir;
	return os;
}
