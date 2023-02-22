#include "sio_response.hpp"

Response::Response() {
	_keepAlive = true;
	_stream = nullptr;
	_type = LENGTHED_RES;
	setState(RES_INIT);
	init();
}

Response::Response(const short &statusCode, const string &contentType, bool keepAlive) {
	_statusCode = statusCode;
	_keepAlive = keepAlive;
	_stream = nullptr;
	_type = LENGTHED_RES;
	setState(RES_INIT);
	init(contentType);
}

Response::Response(const Response &copy) {
	_stream = nullptr;
	*this = copy;
}

Response &Response::operator=(const Response &rhs) {
	if (this != &rhs) {
		_keepAlive = rhs._keepAlive;
		_stream = rhs._stream;
		_state = rhs._state;
		_type = rhs._type;
		_headers = rhs._headers;
	}
	return *this;
}

Response::~Response() {
	delete _stream;
}

void Response::init(const string &contentType) {
	// !INFO: for security reason we should hide this header in some cases !
	addHeader("Server", NAME "/" VERSION);
	addHeader("Date", getUTCDate());
	addHeader("Connection", _keepAlive ? "keep-alive" : "close");
	if (_keepAlive)
		addHeader("Keep-Alive", "timeout=" + (to_string((int)(TIMEOUT / 1e3))));
	else
		_headers.erase("Keep-Alive");
	addHeader("Content-Type", contentType);
}

void Response::prepare(void) {
	_ss << HTTP_VERSION << " " << to_string(_statusCode) << " " << httpStatusCodes[_statusCode] << CRLF;

	map<string, string, StringICaseCompare>::iterator it = _headers.begin();

	while (it != _headers.end()) {
		_ss << it->first << ": " << it->second << CRLF;
		it++;
	}
}

// Setters

void Response::setStatusCode(const short &statusCode) {
	_statusCode = statusCode;
}

void Response::setState(const int &state) {
	_state = state;
}

void Response::setStream(iostream *stream) {
	_stream = stream;
}

void Response::setConnectionStatus(bool keepAlive) {
	_keepAlive = keepAlive;
}

void Response::addHeader(const string &name, const string &value) {
	if (_state & (RES_DONE | RES_BODY))
		return;
	_headers[name] = value;
	setState(RES_HEADER);
}

// TODO: return a boolean to check if the response end
void Response::send(const sockfd &fd) {
	(void)fd;

	if (_state & (RES_INIT | RES_HEADER) && _type == LENGTHED_RES && _stream) {
		int seek = _stream->tellg();
		_stream->seekg(0, _stream->end);
		addHeader("Content-Length", to_string(_stream->tellg()));
		_stream->seekg(seek);
		prepare();
		::send(fd, _ss.str().c_str(), _ss.str().size(), 0);
		::send(fd, CRLF, 2, 0);
		setState(RES_BODY);
	}

	if ((_statusCode / 100) == 1 || _statusCode == 204 || _statusCode == 301)
		return setState(RES_DONE);

	if (_state & RES_BODY && _stream) {
		switch (_type) {
		case LENGTHED_RES:
			char buff[(1 << 10)];
			_stream->read(buff, (1 << 10));
			::send(fd, buff, _stream->gcount(), 0);
			setState(_stream->eof() ? RES_DONE : _state);
			break;
		case CHUNKED_RES:
			break;
		case RANGED_RES:
			break;
		}
	}
}

void Response::sendError(const sockfd &fd, const int &statusCode) {
	setStatusCode(statusCode);
	setConnectionStatus(false);
	init();
	addHeader("Content-Type", mimeTypes["html"]);

	stringstream *ss = new stringstream;
	buildResponseBody(statusCode, *ss);
	setStream(ss);
	send(fd);
}

bool Response::match(const int &state) const {
	return _state & state;
}