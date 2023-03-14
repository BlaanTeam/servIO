#ifndef __BOUNDARY_H__
#define __BOUNDARY_H__

#define MULTIPART_FORM_DATA_STRING "multipart/form-data"
#define BOUNDARY_STRING "boundary"

#include <string>
#include <sstream>
#include "../utility/sio_helpers.hpp"

using namespace std;

class Boundary {
	bool   _valid;
	string _value;

   public:
	Boundary(const string &value);

    bool valid() const;
    bool operator==(const string &value);
};

#endif