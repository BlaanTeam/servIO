#include "sio_boundary.hpp"

#include <iostream>  // TODO: remove
#include <sstream>

Boundary::Boundary() {
	_valid = false;
}

Boundary::Boundary(const string &value) {
	_valid = true;

	stringstream ss(value);

	string part;
	getline(ss, part, ';');
	trim(part);
	if (!iequalString(part, MULTIPART_FORM_DATA_STRING))
		goto invalid;

	getline(ss, part, '\0');
	trim(part);
	ss.clear();
	ss.str(part);

	getline(ss, part, '=');
	if (!iequalString(part, BOUNDARY_STRING))
		goto invalid;

	getline(ss, _value, '\0');
	trim(_value);
	trim(_value, "\"");
	return;
invalid:
	_valid = false;
}

bool Boundary::operator==(const string &value) {
	return _value == value;
}

bool Boundary::valid() const {
	return _valid;
}

int Boundary::consumeBoundary(istream &stream, stringstream &ss) {
	static size_t idx = 0;

	char chr;
	while (!stream.eof() && idx < _value.length()) {
		stream.get(chr);
		ss << chr;
		if (_value[idx] != chr && !stream.eof())
			return idx = 0, -1;
		if (!stream.eof())
			idx++;
	}
	if (idx == _value.length())
		return idx = 0, 1;

	return 0;
}

#define CRLF "\r\n"

int Boundary::consumeCRLF(istream &stream) {
	static size_t idx = 0;

	char chr;
	while (!stream.eof() && idx < 2) {
		stream.get(chr);
		if (CRLF[idx] != chr && !stream.eof())  // ? INFO: preferable to skip \r
			return idx = 0, -1;
		if (!stream.eof())
			idx++;
	}
	if (idx == 2)
		return idx = 0, 1;

	return 0;
}
