#include "sio_request_body.hpp"

Body::Body() {
	_bodyState = BODY_INIT;
	_readingState = START_BODY;
	_content = 0;
	_contentLength = 0;
	_chunkedLength = 0;
	_filename = "/tmp/.servio_" + to_string(getmstime()) + "_body.io";
	_bodyFile = fopen(_filename.c_str(), "w+");  // TODO: check if the file is well opened !
}

Body::Body(const Body &copy) {
	*this = copy;
}

Body &Body::operator=(const Body &rhs) {
	(void)rhs;
	return *this;
}

void Body::setState(const int &state) {
	_bodyState = state;
}

void Body::chooseState(Header &headers) {
	string value = headers.get("Transfer-Encoding");
	if (!value.empty()) {
		stringstream ss(value);
		string       part;

		while (getline(ss, part, ',')) {
			trim(part);
			if (iequalString(part, "Chunked")) {
				setState(CHUNKED_BODY);
				break;
			}
		}
	} else if (!(value = headers.get("Content-Length")).empty() && _bodyState != CHUNKED_BODY) {
		trim(value);
		if (value.length() > 0 && value[0] >= '1' && value[0] <= '9' && every(value, ::isdigit))
			_contentLength = atoi(value.c_str()), setState(LENGTHED_BODY);
	} else
		setState(NORMAL_BODY);
}

void Body::parseChunkedBody(istream &stream) {
	cerr << "Chunked Request !" << endl;
	string tmp = "";
	size_t size = 0;
	char   buff[1024] = {0};
	if (_readingState & ONGION_BODY) {
		stream.read(buff, _chunkedLength - _content);
		_content += stream.gcount();
		fwrite(buff, 1, stream.gcount(), _bodyFile);
		char c = stream.get();
		if (c == '\r') {
			c = stream.get();
			if (c != '\n') {
				goto invalid;
			}
		}
		if (_chunkedLength > _content)
			_readingState = ONGION_BODY;
		else {
			_readingState = START_BODY;
			_content = 0;
		}
	} else if (_readingState & START_BODY) {
		char c = stream.get();
		while (!stream.eof() && c != '\r' && c != '\n') {
			if (isxdigit(c))
				tmp += c;
			else
				goto invalid;
			c = stream.get();
		}
		if (c == '\r') {
			c = stream.get();
			if (c != '\n') {
				goto invalid;
			}
		}
		if (tmp.length() > 0 && _readingState != BODY_ERROR) {
			size = strtol(tmp.c_str(), nullptr, 16);
			_readingState = ONGION_BODY;
			if (size == 0) {
				_readingState = BODY_END;
				_bodyState |= BODY_DONE;
			} else {
				stream.read(buff, size);
				_chunkedLength = size;
				_content += stream.gcount();
				fwrite(buff, 1, stream.gcount(), _bodyFile);
				if (_chunkedLength > _content)
					_readingState = ONGION_BODY;
				else {
					_readingState = START_BODY;
					_content = 0;
				}
				char c = stream.get();
				if (c == '\r') {
					c = stream.get();
					if (c != '\n') {
						goto invalid;
					}
				}
			}
		}
	}
	return;
invalid:
	_readingState = BODY_ERROR;
}

void Body::parseLengthedBody(istream &stream) {
	char buff[1024] = {0};
	stream.read(buff, _contentLength - _content);
	_content += stream.gcount();
	fwrite(buff, 1, stream.gcount(), _bodyFile);
	if (_contentLength == _content)
		_bodyState |= BODY_DONE;
}

void Body::parseNormalBody(istream &stream) {
	char buff[1024] = {0};
	stream.read(buff, 1024);
	_content += stream.gcount();
	fwrite(buff, 1, stream.gcount(), _bodyFile);
}

void Body::consumeBody(istream &stream, Request *req) {
	if (_bodyState & BODY_INIT) {
		chooseState(req->getHeaders());
	}
	if (_bodyState & BODY_READ) {
		switch (_bodyState) {
		case CHUNKED_BODY:
			if (_readingState & BODY_END) {
				_bodyState |= BODY_DONE;
				break;
			}
			if (_readingState & BODY_ERROR) {
				_bodyState |= BODY_ERROR;
				break;
			}
			parseChunkedBody(stream);
			break;
		case LENGTHED_BODY:
			parseLengthedBody(stream);
			break;
		default:
			parseNormalBody(stream);
			break;
		}
		fflush(_bodyFile);
	}
}

int Body::getFileno() const {
	return _bodyFile ? fileno(_bodyFile) : -1;
}

Body::~Body() {
	fclose(_bodyFile);  // TODO: TBD !!
}
