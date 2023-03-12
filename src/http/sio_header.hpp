#ifndef __HEADER_H__
#define __HEADER_H__

#include <map>
#include <string>
#include "utility/sio_helpers.hpp"

using namespace std;

class Header : public map<string, string, StringICaseCompare> {
   public:
	bool found(const string &key) const;
};

#endif