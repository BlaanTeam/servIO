#include "sio_response.hpp"

Response::Response() {
	init();
	_keepAlive = true;
}

Response::Response(const short &statusCode, const string &contentType, bool keepAlive) {
	init();
	addHeader("Content-Type", contentType);
	_statusCode = statusCode;
	_keepAlive = keepAlive;
}

Response::Response(const Response &copy) {
	(void)copy;
}

Response &Response::operator=(const Response &rhs) {
	(void)rhs;
	return *this;
}

void Response::init(void) {
	// !INFO: for security reason we should hide this header in some cases !
	addHeader("Server", NAME "/" VERSION);
	addHeader("Date", getUTCDate());
	addHeader("Connection", _keepAlive ? "keep-alive" : "close");
	addHeader("Content-Type", DEFAULT_MIME_TYPE);
}

void Response::prepare(void) {
	_ss << HTTP_VERSION << " " << to_string(_statusCode) << " " << httpStatusCodes[_statusCode] << CRLF;

	map<string, string>::iterator it = _headers.begin();

	while (it != _headers.end()) {
		_ss << it->first << ": " << it->second << CRLF;
		it++;
	}
}

// Setters

void Response::setStatusCode(const short &statusCode) {
	_statusCode = statusCode;
}

void Response::setConnectionStatus(bool keepAlive) {
	_keepAlive = keepAlive;
}

void Response::addHeader(const string &name, const string &value) {
	_headers[name] = value;
}

void Response::send(const sockfd &fd, iostream &stream, bool chunked) {
	(void)chunked;  // TODO: add logic for chunked responses !

	int seek = stream.tellg();
	stream.seekg(0, stream.end);
	addHeader("Content-Length", to_string(stream.tellg()));
	stream.seekg(seek);

	prepare();
	::send(fd, _ss.str().c_str(), _ss.str().size(), 0);
	::send(fd, CRLF, 2, 0);

	while (stream) {
		char buff[(1 << 10) + 1] = {0};
		stream.read(buff, (1 << 10));
		::send(fd, buff, stream.gcount(), 0);
	}
}