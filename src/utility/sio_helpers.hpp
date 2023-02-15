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

#endif