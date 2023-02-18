#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <deque>
#include <functional>
#include <sstream>
#include <string>

using namespace std;

#include "sio_helpers.tpp"

pair<bool, string> normpath(const string &path, const char sep = '/');
bool               iequalString(const string &s1, const string &s2);

void ltrim(string &value, const string &sep = " ");
void rtrim(string &value, const string &sep = " ");
void trim(string &value, const string &sep = " ");

class StringICaseCompare : binary_function<string, string, bool> {
	class CharICaseCompare : public binary_function<unsigned char, unsigned char, bool> {
	   public:
		bool operator()(const unsigned char &c1, const unsigned char &c2) const {
			return ::tolower(c1) < ::tolower(c2);
		}
	};

   public:
	bool operator()(const std::string &s1, const std::string &s2) const {
		return lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), CharICaseCompare());
	}
};

#endif