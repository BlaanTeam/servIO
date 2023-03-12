#include "sio_header.hpp"

bool Header::found(const string &key) const {
	return find(key) != end();
}