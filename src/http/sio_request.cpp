#include "sio_request.hpp"

Request::Request() {
	_state = REQ_INIT;
	_method = UNKNOWN;
	_statusCode = BAD_REQUEST;
}

Request::Request(const Request &copy) {
	*this = copy;
}

Request::~Request() {
	while (!_streams.empty()) {
		delete _streams.front();
		_streams.pop();
	}
}

Request &Request::operator=(const Request &rhs) {
	(void)rhs;
	return *this;
}

void Request::parseFirstLine(string &line) {
	stringstream ss(line);
	string       buff;
	string       uri;
	string       httpVer;

	ss >> buff >> uri >> httpVer;

	string::size_type idx;
	idx = uri.find('?');
	_path = uri.substr(0, idx);
	if (idx != string::npos)
		_query = uri.substr(idx + 1);

	pair<bool, string> pr = normpath(_path);

	if (uri.size() > 2048 || !every(buff, ::isupper) || !pr.first || httpVer != HTTP_VERSION) {
		_statusCode = (uri.size() > 2048) ? REQUEST_URI_TOO_LONG : _statusCode;
		goto invalid;
	}
	_path = pr.second;
	{
		int idx = 0;
		while (idx < httpMethodCount && httpMethods[idx] != buff)
			idx++;
		_method = (HttpMethod)pow(2, idx);
	}
	getline(ss, buff, '\0');
	if ((buff != CRLF && buff != LF))
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

	if (line == CRLF || line == LF) {
		_body.chooseState(_headers);
		_body.openFile();
		return changeState(_method & (GET | TRACE | OPTIONS | HEAD) ? REQ_DONE : REQ_BODY);
	}
	getline(ss, key, ':');
	getline(ss, value, '\0');
	size_t n = 1;
	if (value.length() > 1 && value.substr(value.length() - 2) == CRLF)
		n = 2;
	if (!value.length())
		return;
	string tmp = value.substr(value.length() - n);
	if (value.length() < n + 1 || (tmp != LF && tmp != CRLF))
		goto invalid;
	value = value.substr(0, value.length() - n);
	_headers.add(key, value);
	return;
invalid:
	changeState(REQ_INVALID);
}

void Request::parseBody(stringstream &stream) {
	_body.consumeBody(stream);
}

void Request::changeState(const int &state) {
	_state = state;
}

void Request::consumeStream(stringstream &stream) {
	char chr;
	bool empty = false;

	bool n = 0;

	string tmp(_line);

	(!stream.get(chr).eof()) && (n += 1);

	tmp += strchr(CRLF, chr) && _state & REQ_INIT ? "" : string(1, chr);

	while (!stream.eof()) {
		if (_state & REQ_BODY) {
			if (n == 1) {
				int seek = stream.tellg();
				stream.seekg(seek - 1);
			}

			parseBody(stream);
			if (_body.getState() & BODY_DONE) {
				changeState(REQ_DONE);
				break;
			}
		}

		if (_state & REQ_INIT && !strchr(CRLF, chr)) {
			if (empty) tmp += chr;
			changeState(REQ_LINE);
		}
		if (_state & REQ_INIT && strchr(CRLF, chr)) {
			(!stream.get(chr).eof()) && (n += 1);
			empty = true;
			continue;
		}
		if (tmp.find(CRLF) != string::npos || tmp.find(LF) != string::npos) {
			switch (_state) {
			case REQ_LINE:
				parseFirstLine(tmp);
				break;
			case REQ_HEADER:
				parseHeaders(tmp);
				break;
			}
			tmp = "";
		}
		(!stream.get(chr).eof()) && (n += 1);
		if (!stream.eof())
			tmp += chr;
	}
	_line = tmp;
}

void Request::addStream(stringstream *stream) {
	_streams.push(stream);
}

stringstream *Request::getAvailableStream(void) {
	if (_streams.size() && _streams.front()->eof()) {
		delete _streams.front();
		_streams.pop();
	}

	return _streams.size() ? _streams.front() : nullptr;
}

// Getters

string Request::getPath(void) const {
	return _path;
}

string Request::getQuery(void) const {
	return _query;
}

short Request::getState(void) const {
	return _state;
}

int Request::getStatusCode() const {
	return _statusCode;
}

int Request::getFileno() const {
	return _body.getFileno();
}

HttpMethod Request::getMethod(void) const {
	return _method;
}

Header &Request::getHeaders(void) {
	return _headers;
}
map<int, BodyFile> &Request::getBodyFiles() {
	return _body.getBodyFiles();
}
bool Request::valid() const {
	return (_state & ~REQ_INVALID);
}

bool Request::isTooLarge(const int &clientMaxSize) {
	string value = _headers.get("Content-Length");
	if (value.empty())
		return false;
	if (!every(value, ::isdigit))
		return false;
	return atoll(value.c_str()) > clientMaxSize;
}

bool Request::match(const int &state) const {
	return _state & state;
}
Range Request::getRange() {
	string value = _headers.get("Range");
	if (value.empty())
		return Range();
	return Range(value);
}

void Request::reset(void) {
	_state = REQ_INIT;
	_method = UNKNOWN;
	_statusCode = BAD_REQUEST;

	_path = "";
	_query = "";

	// clear the headers
	_headers.clear();
	_body.reset();
}

void Request::closeBodyFile() {
	if (match(REQ_DONE)) _body.closeFile();
}