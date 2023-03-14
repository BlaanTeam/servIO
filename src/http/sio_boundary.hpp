#ifndef __BOUNDARY_H__
#define __BOUNDARY_H__

#define MULTIPART_FORM_DATA_STRING "multipart/form-data"
#define BOUNDARY_STRING "boundary"

#include <sstream>
#include <string>

#include "../utility/sio_helpers.hpp"

using namespace std;

class Boundary {
	bool   _valid;
	string _value;

   public:
	Boundary();
	Boundary(const string &value);

	int consumeBoundary(istream &stream);
	int consumeCRLF(istream &stream);

	bool valid() const;
	bool operator==(const string &value);
};

#endif