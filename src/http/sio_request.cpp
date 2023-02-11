#include "sio_request.hpp"

#include "sio_http_codes.hpp"

Request::Request() {
	_state = REQ_INIT;
	_method = UNKNOWN;
}

void Request::parseFirstLine(string &line) {
	stringstream ss(line);
	string       buff;
	string       httpVer;

	ss >> buff >> _uri >> httpVer;
	int idx = 0;
	while (idx < httpMethodCount && httpMethods[idx] != buff)
		idx++;
	getline(ss, buff, '\0');
	_method = (HttpMethod)idx;
	if (_method == UNKNOWN || _uri[0] != '/' || httpVer != HTTP_VERSION ||
	    buff != CRLF)
		goto invalid;
	changeState(REQ_HEADER);
	return;

invalid:
	changeState(REQ_INVALID);
}

void Request::parseHeaders(string &line) {
	stringstream ss(line);
	string       key;
	string       value;

	if (line == CRLF)
		return changeState(REQ_BODY);
	getline(ss, key, ':');
	getline(ss, value, '\0');
	if (value.length() < 3 || value.substr(value.length() - 2, 2) != CRLF)
		goto invalid;
	value = value.substr(0, value.length() - 2);
	_headers[key] = value;
	return;
invalid:
	changeState(REQ_INVALID);
}

void Request::changeState(const int &state) {
	_state = state;
}

void Request::consumeStream(istream &stream) {
	char chr;

	string tmp(_line);

	stream.get(chr);
	tmp += string(1, chr);

	while (!stream.eof()) {
		if (tmp.find(CRLF) != string::npos) {
			switch (_state) {
			case REQ_INIT:
				parseFirstLine(tmp);
				break;
			case REQ_HEADER:
				parseHeaders(tmp);
				break;
			case REQ_BODY:
				// parseBody(tmp);
				break;
			}
			tmp = "";
		}
		stream.get(chr);
		if (!stream.eof())
			tmp += chr;
	}
	_line = tmp;
}

// Getters
bool Request::valid() const {
	return (_state & ~REQ_INVALID && _state & ~REQ_INIT);
}