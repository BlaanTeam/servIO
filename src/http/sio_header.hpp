#ifndef __HEADER_H__
#define __HEADER_H__

#include <map>
#include <string>
#include <set>

#include "utility/sio_helpers.hpp"

using namespace std;

class Header : public map<string, set<string>, StringICaseCompare> {
   public:
	bool   found(const string& key) const;
	string get(const string& key);
	
	void add(const string &key, const string &value);
};

#endif