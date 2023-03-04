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

	// if ((_statusCode / 100) == 1 || _statusCode == 204 || _statusCode == 301)
	// 	return setState(RES_DONE);

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

void Response::setupErrorResponse(const int &statusCode, MainContext<Type> *ctx, bool isBuiltIn) {
	setStatusCode(statusCode);
	setConnectionStatus(false);
	init();
	addHeader("Content-Type", mimeTypes["html"]);

	if (ctx) {
		ErrorPage *errPage = ctx->getErrorPage(statusCode);
		if (errPage) {
			if (!errPage->exists() && isBuiltIn)
				setupErrorResponse(NOT_FOUND, ctx, false);
			else if (!errPage->exists()) {
				if (errno == EACCES && !isBuiltIn)
					return setupErrorResponse(FORBIDDEN, ctx, true);
				setStream(buildResponseBody(NOT_FOUND));
			} else {
				addHeader("Content-Type", mimeTypes.choiceMimeType(errPage->page));
				setStream(new fstream(errPage->page, ios::in));
			}
			return;
		}
	}

	setStream(buildResponseBody(statusCode));
}

void Response::setupRedirectResponse(Redirect *redir, MainContext<Type> *ctx) {
	redir->prepare(ctx);

	setStatusCode(redir->code);
	init();
	addHeader("Content-Type", mimeTypes["html"]);

	if (redir->isRedirect) {
		addHeader("Location", redir->path);
		setStream(buildResponseBody(redir->code));
		return;
	}
	addHeader("Content-Type", mimeTypes[""]);
	iostream *ss = new stringstream;
	(*ss) << redir->path;
	setStream(ss);
}

void Response::setupDirectoryListing(const string &path, const string &title) {
	setStatusCode(200);
	setConnectionStatus(true);
	init();
	addHeader("Content-Type", mimeTypes["html"]);
	setStream(buildDirectoryListing(path, title));
}

void Response::setupNormalResponse(const string &path, iostream *file) {
	// TODO: check if the file is openned !
	setStatusCode(200);
	setConnectionStatus(true);
	init();
	addHeader("Content-Type", mimeTypes.choiceMimeType(path));
	setStream(file);
}

bool Response::match(const int &state) const {
	return _state & state;
}

void Response::reset(void) {
	// clear the headers
	_headers.clear();

	// clear the stringstream
	_ss.str("");
	_ss.clear();

	delete _stream;
	_stream = nullptr;

	_keepAlive = true;
	_type = LENGTHED_RES;

	setState(RES_INIT);
	init();
}

// private functions

void Response::setupLengthedBody() {
	size_t seek = _stream->tellg();
	_stream->seekg(0, _stream->end);

	size_t contentLength = _stream->tellg() < 0 ? (streampos)0 : _stream->tellg();
	addHeader("Content-Length", to_string(contentLength));

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
	char buff[(1 << 10)];

	_stream->read(buff, (1 << 10));
	if (_stream->gcount() > 0) {
		_ss << hex << _stream->gcount() << CRLF;
		::send(fd, _ss.str().c_str(), _ss.str().size(), 0);
		::send(fd, buff, _stream->gcount(), 0);
		::send(fd, CRLF, 2, 0);
		_ss.str("");
		_ss.clear();
	}
	setState(_stream->eof() ? RES_DONE : _state);
}

void Response::setupRangedBody() {
}

void  Response::noLastRange(const sockfd &fd,RangeSpecifier &range) {
			char buff[(1 << 10)];
			if (_lengthState == INIT_LENGTH) {
				_stream->seekg(range.rangeStart);
				_length = _stream->end - range.rangeStart;
				_lengthState = ONGOING_LENGTH;
			}
			if (_length > (1 << 10)) {
				_stream->read(buff, (1 << 10));
				::send(fd, buff, _stream->gcount(), 0);
				::send(fd, CRLF, 2, 0);
				_length -= _stream->gcount();
			} else {
				_stream->read(buff, _length);
				::send(fd, buff, _stream->gcount(), 0);
				::send(fd, CRLF, 2, 0);
				_length -= _stream->gcount();
				_lengthState = DONE_LENGTH;
			}
}

void Response::noFirstRange(const sockfd &fd, RangeSpecifier &range) {
	char buff[(1 << 10)];
	if (_lengthState == INIT_LENGTH) {
		_stream->seekg(_stream->end - range.rangeEnd);
		_length = range.rangeEnd;
		_lengthState = ONGOING_LENGTH;
	}
	if (_length > (1 << 10)) {
		_stream->read(buff, (1 << 10));
		::send(fd, buff, _stream->gcount(), 0);
		::send(fd, CRLF, 2, 0);
		_length -= _stream->gcount();
	} else {
		_stream->read(buff, _length);
		::send(fd, buff, _stream->gcount(), 0);
		::send(fd, CRLF, 2, 0);
		_length -= _stream->gcount();
		_lengthState = DONE_LENGTH;
	}
}

void	Response::normalRange(const sockfd &fd, RangeSpecifier &range) {
	char buff[(1 << 10)];
	if (_lengthState == INIT_LENGTH) {
		_stream->seekg(range.rangeStart);
		_length = range.rangeEnd - range.rangeStart;
		_lengthState = ONGOING_LENGTH;
	}
	if (_length > (1 << 10)) {
		_stream->read(buff, (1 << 10));
		::send(fd, buff, _stream->gcount(), 0);
		::send(fd, CRLF, 2, 0);
		_length -= _stream->gcount();
	} else {
		_stream->read(buff, _length);
		::send(fd, buff, _stream->gcount(), 0);
		::send(fd, CRLF, 2, 0);
		_length -= _stream->gcount();
		_lengthState = DONE_LENGTH;
	}
}

void Response::sendRangedBody(const sockfd &fd) {
	vector<RangeSpecifier> ranges = _range.getRangeSpecifiers();
	for (size_t i = 0; i < ranges.size(); i++) {
		RangeSpecifier range = ranges[i];
		switch (range.type) {
		case NOL:
			noLastRange(fd, range);
			break;
		case NOF:
			noFirstRange(fd, range);
			break;
		default:
			normalRange(fd, range);
			break;
		}
	}
}
