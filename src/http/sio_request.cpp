#include "sio_request.hpp"

Request::Request() {
	_state = REQ_INIT;
	_method = UNKNOWN;
}

void Request::consumeStream(istream &stream) {
	(void)stream;
}

// Getters
bool Request::valid() const {
	return _state != REQ_INVALID;
}