#include "gridpath.hpp"
#include <cstdio>
#include <limits>

GridPath::GridPath(const char *file, unsigned int x0, unsigned int y0,
		unsigned int x1, unsigned int y1) : map(file) {
	start = map.loc(x0, y0);
	finish = map.loc(x1, y1);
}

GridPath::State GridPath::initialstate(void) {
	State s;
	s.loc = start;
	return s;
}