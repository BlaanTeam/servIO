#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <deque>
#include <sstream>
#include <string>
#include <functional>

using namespace std;

#include "sio_helpers.tpp"

pair<bool, string> normpath(const string &path, const char sep = '/');

#endif