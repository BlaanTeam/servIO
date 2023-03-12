#include "sio_header.hpp"

bool Header::found(const string &key) const {
	return find(key) != end();
}

string Header::get(const string &key) {
	string value = (*this)[key];
	trim(value);
	return value;
}