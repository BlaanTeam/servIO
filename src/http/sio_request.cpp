#include "sio_request.hpp"

Request::Request() {
	_state = REQ_INIT;
	_method = UNKNOWN;
	_bodyState = BODY_INIT;
	_statusCode = BAD_REQUEST;
	_content = 0;
	_bodyFile = nullptr;
}

Request::Request(const Request &copy) {
	*this = copy;
}

// TODO: add Request destructor to close _bodyFile

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

	if (!every(buff, ::isupper) || !pr.first || httpVer != HTTP_VERSION)
		goto invalid;
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
	_headers[key] = value;
	return;
invalid:
	changeState(REQ_INVALID);
}

void Request::parseBody(stringstream &stream) {
	if (_bodyState & BODY_INIT) {
		_bodyFile = fopen(("/tmp/.servio_" + to_string(getmstime()) + "_body.io").c_str(), "w+");
		// TODO: check if the file opened !
		_bodyState = BODY_OPEN;
	}
	if (_bodyState & BODY_OPEN) {
		headerIter it = _headers.find("Transfer-Encoding");
		if (it != _headers.end()) {
			stringstream ss(it->second);
			string       part;

			while (getline(ss, part, ',')) {
				trim(part);
				if (iequalString(part, "Chunked")) {
					_bodyState = CHUNKED_BODY;
					break;
				}
			}
		}
		it = _headers.find("Content-Length");
		if (it != _headers.end() && _bodyState != CHUNKED_BODY) {
			trim(it->second);
			if (it->second.length() > 0 && it->second[0] >= '1' && it->second[0] <= '9' && every(it->second, ::isdigit))
				_contentLength = stoi(it->second), _bodyState = LENGTHED_BODY;
		}
		_bodyState = _bodyState & BODY_OPEN ? NORMAL_BODY : _bodyState;
	}
	if (_bodyState & BODY_READ) {
		char buff[1024] = {0};
		switch (_bodyState) {
		case CHUNKED_BODY:
			cerr << "Chunked Request !" << endl;
			stream.read(buff, 1024);
			fwrite(buff, 1, stream.gcount(), _bodyFile);
			break;
		case LENGTHED_BODY:
			cerr << "Request With Content-Length: " << _contentLength << endl;
			stream.read(buff, _contentLength);
			fwrite(buff, 1, stream.gcount(), _bodyFile);
			break;
		default:
			cerr << "Request Without Content-Length !" << endl;
			stream.read(buff, 1024);
			_content += stream.gcount();
			fwrite(buff, 1, stream.gcount(), _bodyFile);
			break;
		}
		fflush(_bodyFile);
	}
}

void Request::changeState(const int &state) {
	_state = state;
}

void Request::consumeStream(stringstream &stream) {
	char chr;
	bool empty = false;

	string tmp(_line);

	stream.get(chr);
	tmp += strchr(CRLF, chr) && _state & REQ_INIT ? "" : string(1, chr);

	while (!stream.eof()) {
		if (_state & REQ_BODY) {
			int seek = stream.tellg();
			stream.seekg(seek - 1);
			parseBody(stream);
		}

		if (_state & REQ_INIT && !strchr(CRLF, chr)) {
			if (empty) tmp += chr;
			changeState(REQ_LINE);
		}
		if (_state & REQ_INIT && strchr(CRLF, chr)) {
			stream.get(chr);
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
		stream.get(chr);
		if (!stream.eof())
			tmp += chr;
	}
	_line = tmp;
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

bool Request::valid() const {
	return (_state & ~REQ_INVALID);
}