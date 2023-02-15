#ifndef __HTTP_RANGE_H__
#define __HTTP_RANGE_H__

#include <string>
#include <vector>

#include "utility/sio_helpers.hpp"

using namespace std;

enum UnitType {
	NON = (1 << 0),
	NOF = (1 << 1),
	NOL = (1 << 2)
};

struct RangeSpecifier {
	int      rangeStart;
	int      rangeEnd;
	UnitType type;
};

class Range {
	bool _valid;

	vector<RangeSpecifier> rangeSpecifiers;

   private:
	RangeSpecifier parse(const string &value);

   public:
	Range();
	Range(const string &value);

	// Getters
	bool valid(void) const;
};

#endif