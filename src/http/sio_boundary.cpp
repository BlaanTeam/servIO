#include "sio_boundary.hpp"

#include <iostream>

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
