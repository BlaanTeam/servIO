#include "sio_request.hpp"

Request::Request() {
	_state = REQ_INIT;
	_method = UNKNOWN;
}

void Request::consumeStream(const string &stream) {
	(void)stream;

headers:

	goto body;
body:

	goto done;
done:
	_state = REQ_DONE;
	return;

invalid:
	_state = REQ_INVALID;
}

// Getters
bool Request::valid() const {
	return _state != REQ_INVALID;
}