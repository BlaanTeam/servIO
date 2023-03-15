#include "sio_request_body.hpp"

BodyFile::BodyFile() {}
BodyFile::~BodyFile() {
	(_file) && (fclose(_file));
}

void BodyFile::addFile(FILE *file, const string &filename) {
	_file = file;
	_filename = filename;
}
void BodyFile::addHeader(const string &key, const string &value) {
	_headers.add(key, value);
}
void BodyFile::write(stringstream &ss) {
	fwrite(ss.str().c_str(), 1, ss.str().size(), _file);
	ss.clear();
	ss.str("");
	fflush(_file);
}
FILE *BodyFile::getFile() {
	return _file;
}

string BodyFile::extractFilename() {
	string value = _headers.get("Content-Disposition");
	if (value.empty())
		return value;
	size_t idx = value.find("filename=");
	if (idx == string::npos)
		return "";
	value = value.substr(idx + 9);
	trim(value, " \"");
	return value;
}

string BodyFile::getFilename() const {
	return _filename;
}

Body::Body() {
	_bodyState = BODY_INIT;
	_readingState = START_BODY;
	_content = 0;
	_contentLength = 0;
	_chunkedLength = 0;
	_filename = "/tmp/.servio_" + to_string(getmstime()) + "_body.io";
	_bodyFile = fopen(_filename.c_str(), "w+");  // TODO: check if the file is well opened !

	_multipartState = MULTIPART_INIT_BOUNDARY;
	_fileIndex = 0;
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
			if (iequalString(part, "Chunked"))
				return setState(CHUNKED_BODY);
		}
	}
	if (!(value = headers.get("Content-Length")).empty()) {
		trim(value);
		if (value.length() > 0 && value[0] >= '1' && value[0] <= '9' && every(value, ::isdigit))
			_contentLength = atoi(value.c_str());
	}
	value = headers.get("Content-Type");
	_boundary = Boundary(value);
	if (_boundary.valid())
		return setState(MULTIPARTED_BODY);
	else if (_contentLength > 0)
		setState(LENGTHED_BODY);
	else
		setState(BODY_DONE);
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
		case MULTIPARTED_BODY:
			parseMultipartBody(stream);
			break;
		}
		fflush(_bodyFile);
	}
}

map<int, BodyFile> &Body::getBodyFiles() {
	return _bodyFiles;
}

int Body::getFileno() const {
	return _bodyFile ? fileno(_bodyFile) : -1;
}

short Body::getState() const {
	return _bodyState;
}

Body::~Body() {
	fclose(_bodyFile);
}

void Body::parseHeaders(stringstream &ss) {
	string key;
	string value;

	if (ss.str() == CRLF || ss.str() == LF) {
		_multipartState = MULTIPART_BODY_INIT;
		return;
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
		return;
	value = value.substr(0, value.length() - n);
	_bodyFiles[_fileIndex].addHeader(key, value);
}

static bool match(const char &chr, const char &chr_, istream &stream) {
	return chr == chr_ && !stream.eof();
}

void Body::parseMultipartBody(istream &stream) {
	FILE  *file;
	string filename;

	char   chr;
	size_t currSeek;

	while (!stream.eof() && _multipartState != MULTIPART_DONE) {
		if (_multipartState & MULTIPART_INIT) {
			switch (_multipartState) {
			case MULTIPART_INIT_BOUNDARY:
				(_boundary.consumeCRLF(stream)) && (_multipartState = MULTIPART_INIT_CRLF);
				(_boundary.consumeBoundary(stream, _lost)) && (_multipartState = MULTIPART_INIT_CRLF);
				_lost.str("");
				break;

			case MULTIPART_INIT_CRLF:
				(_boundary.consumeCRLF(stream)) && (_multipartState = MULTIPART_HEADER);
				break;
			}
		} else if (_multipartState & MULTIPART_HEADER) {
			stream.get(chr);
			_line += chr;
			if (_line.find(CRLF) != string::npos || _line.find(LF) != string::npos) {
				stringstream ss(_line);
				parseHeaders(ss);
				_line = "";
				if (_multipartState & MULTIPART_BODY)
					break;
			}
		} else if (_multipartState & MULTIPART_BODY) {
			switch (_multipartState) {
			case MULTIPART_BODY_INIT:
				filename = "/tmp/.servio_" + to_string(getmstime()) + "_upload.io";
				file = fopen(filename.c_str(), "w+");  // TODO: check if the file not opened !
				_bodyFiles[_fileIndex].addFile(file, filename);
				_multipartState = MULTIPART_BODY_READ;
				break;
			case MULTIPART_BODY_D:
				stream.get(chr);
				if (match('-', chr, stream)) {
					_multipartState = MULTIPART_BODY_DD;
					(!stream.eof()) && (_lost << chr);
					break;
				}
				(!stream.eof()) && (_lost << chr);
				_bodyFiles[_fileIndex].write(_lost);
				_multipartState = MULTIPART_BODY_READ;
				break;
			case MULTIPART_BODY_DD:
				_multipartState = MULTIPART_BODY_BOUNDARY;
				break;
			case MULTIPART_BODY_BOUNDARY:
				int ret;
				if ((ret = _boundary.consumeBoundary(stream, _lost))) {
					if (ret == 1) {
						stream.get(chr);
						if (match('-', chr, stream)) {
							_multipartState = MULTIPART_DONE;
							_bodyState |= BODY_DONE;
							return;
						}
						_lost.clear();
						_lost.str("");
						currSeek = stream.tellg();
						stream.seekg(currSeek - 1);
						_multipartState = MULTIPART_BODY_CRLF;
					}
					if (ret == -1) {
						_bodyFiles[_fileIndex].write(_lost);
						_multipartState = MULTIPART_BODY_READ;
						break;
					}
				}
				break;
			case MULTIPART_BODY_CRLF:
				(_boundary.consumeCRLF(stream)) && (_multipartState = MULTIPART_HEADER);
				if (_multipartState & MULTIPART_HEADER)
					_fileIndex++;
				break;

			case MULTIPART_BODY_LR:
				stream.get(chr);
				if (match('\n', chr, stream)) {
					_multipartState = MULTIPART_BODY_LF;
					(!stream.eof()) && (_lost << chr);
					break;
				}
				(!stream.eof()) && (_lost << chr);
				_bodyFiles[_fileIndex].write(_lost);
				_multipartState = MULTIPART_BODY_READ;
				break;
			case MULTIPART_BODY_LF:
				stream.get(chr);
				if (match('-', chr, stream)) {
					_multipartState = MULTIPART_BODY_D;
					(!stream.eof()) && (_lost << chr);
					break;
				}
				_bodyFiles[_fileIndex].write(_lost);

				currSeek = stream.tellg();
				stream.seekg(currSeek - 1);

				_multipartState = MULTIPART_BODY_READ;
				break;
			case MULTIPART_BODY_READ:
				stream.get(chr);
				if (match('\r', chr, stream)) {
					_multipartState = MULTIPART_BODY_LR;
					(!stream.eof()) && (_lost << chr);
					break;
				} else if (match('-', chr, stream)) {
					_multipartState = MULTIPART_BODY_D;
					(!stream.eof()) && (_lost << chr);
					break;
				}
				(!stream.eof()) && (_lost << chr);
				_bodyFiles[_fileIndex].write(_lost);
				break;
			}
		}
	}
}

void Body::reset() {
	_bodyState = BODY_INIT;
	_readingState = START_BODY;
	_content = 0;
	_contentLength = 0;
	_chunkedLength = 0;
	_filename = "";
	(_bodyFile) && (fclose(_bodyFile));
	_bodyFile = NULL;

	_multipartState = MULTIPART_INIT_BOUNDARY;
	_line = "";
	_lost.clear();
	_lost.str("");
	_fileIndex = 0;

	map<int, BodyFile>::iterator it = _bodyFiles.begin();
	while (it != _bodyFiles.end()) {
		if (it->second.getFile())
			fclose(it->second.getFile());
		it++;
	}
	_bodyFiles.clear();
}
