#include "gridmap.hpp"
#include <cstdio>
#include <cmath>
#include <cstdlib>

class GridPath {
public:

	enum { UnitCost = false };

	typedef float Cost;
	static const float InfCost = -1.0;

	typedef int Oper;	// Index into the ops arrays.
	enum { Nop = -1 };

	GridPath(const char *file, unsigned int, unsigned int,
		unsigned int, unsigned int);

	class State {
		friend class GridPath;
		unsigned int loc;
		int nops;
		Oper ops[8];
	public:
		State(void) : nops(-1) { }
	};

	class PackedState {
		friend class GridPath;
		unsigned int loc;
	public:
		unsigned long hash(void) { return loc; }

		bool eq(const PackedState &other) const {
			return other.loc == loc;
		}
	};

	struct Undo {
		Undo(State &, Oper) { }
	};

	State initialstate(void);

	Cost h(State &s) {
		return octiledist(s.loc, finish);
	}

	bool isgoal(State &s) {
		return s.loc == finish;
	}

	unsigned int nops(State &s) {
		if (s.nops < 0) {
			s.nops = 0;

			bool up = map.terrainok(s.loc, map.up(s.loc));
			bool down = map.terrainok(s.loc, map.down(s.loc));
			bool left = map.terrainok(s.loc, map.left(s.loc));
			bool right = map.terrainok(s.loc, map.right(s.loc));

			if (up)
				s.ops[s.nops++] = map.up(s.loc);
			if (down)
				s.ops[s.nops++] = map.down(s.loc);
			if (left)
				s.ops[s.nops++] = map.left(s.loc);
			if (right)
				s.ops[s.nops++] = map.right(s.loc);
			if (map.terrainok(s.loc, map.up(map.left(s.loc))) && (up || left))
				s.ops[s.nops++] = map.up(map.left(s.loc));
			if (map.terrainok(s.loc, map.down(map.left(s.loc))) && (down || left))
				s.ops[s.nops++] = map.down(map.left(s.loc));
			if (map.terrainok(s.loc, map.up(map.right(s.loc))) && (up || right))
				s.ops[s.nops++] = map.up(map.right(s.loc));
			if (map.terrainok(s.loc, map.down(map.right(s.loc))) && (down || right))
				s.ops[s.nops++] = map.down(map.right(s.loc));
		}
		return s.nops;
	}

	Oper nthop(State &s, unsigned int n) {
		return s.ops[n];
	}

	Oper revop(State &s, Oper op) {
		return s.loc;
	}

	Cost opcost(State &s, Oper op) {
		if (map.x(s.loc) == map.x(op) || map.y(s.loc) == map.y(op))
			return 1;
		return sqrtf(2.0);
	}

	void undo(State &s, Undo &u) { }

	State &apply(State &buf, State &s, Oper op) {
		buf.loc = op;
		return buf;
	}

	void pack(PackedState &dst, State &src) {
		dst.loc = src.loc;
	}

	State &unpack(State &buf, PackedState &pkd) {
		buf.loc = pkd.loc;
		return buf;
	}

	void dumpstate(FILE *out, State &s) {
		fprintf(out, "%u, %u\n", map.x(s.loc), map.y(s.loc));
	}

	unsigned int width(void) { return map.width(); }

	unsigned int height(void) { return map.height(); }

private:

	float octiledist(unsigned int l0, unsigned int l1) {
		unsigned int dx = abs(map.x(l0) - map.x(l1));
		unsigned int dy = abs(map.y(l0) - map.y(l1));
		unsigned int diag = dx < dy ? dx : dy;
		unsigned int straight = dx < dy ? dy : dx;
		return (straight - diag) + sqrtf(2.0) * diag;
	}

	unsigned int start, finish;
	GridMap map;
};