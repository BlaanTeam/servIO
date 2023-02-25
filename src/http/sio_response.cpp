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

bool Response::keepAlive(void) const {
	return _keepAlive;
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

	if (_state & (RES_INIT | RES_HEADER)) {
		if (_type == LENGTHED_RES && _stream)
			setupLengthedBody();
		else if (_type == CHUNKED_RES)
			setupChunkedBody();
		else if (_type == RANGED_RES)
			setupRangedBody();

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
			sendLengthedBody(fd);
			break;
		case CHUNKED_RES:
			sendChunkedBody(fd);
			break;
		case RANGED_RES:
			sendRangedBody(fd);
			break;
		}
	}
}

void Response::setupError(const int &statusCode) {
	setStatusCode(statusCode);
	setConnectionStatus(false);
	init();
	addHeader("Content-Type", mimeTypes["html"]);
	setStream(buildResponseBody(statusCode));
}

bool Response::match(const int &state) const {
	return _state & state;
}

void Response::setupDirectoryListing(const string &path, const string &title) {
	setStatusCode(200);
	setConnectionStatus(true);
	init();
	addHeader("Content-Type", mimeTypes["html"]);
	setStream(buildDirectoryListing(path, title));
}

// private functions

void Response::setupLengthedBody() {
	size_t seek = _stream->tellg();
	_stream->seekg(0, _stream->end);
	addHeader("Content-Length", to_string(_stream->tellg()));
	_stream->seekg(seek);
}

void Response::sendLengthedBody(const sockfd &fd) {
	char buff[(1 << 10)];
	_stream->read(buff, (1 << 10));
	::send(fd, buff, _stream->gcount(), 0);
	setState(_stream->eof() ? RES_DONE : _state);
}

void Response::setupChunkedBody() {
	addHeader("Transfer-Encoding", "Chunked");
}

void Response::sendChunkedBody(const sockfd &fd) {
	(void)fd;
}

void Response::setupRangedBody() {
}

void Response::sendRangedBody(const sockfd &fd) {
	(void)fd;
}
