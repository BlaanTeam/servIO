#ifndef __BODY_H__
#define __BODY_H__

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "./sio_boundary.hpp"
#include "./sio_header.hpp"
#include "utility/sio_helpers.hpp"
#include "utility/sio_utils.hpp"

using namespace std;

#define BODY_INIT (1 << 0)
#define BODY_OPEN (1 << 1)
#define NORMAL_BODY (1 << 2)
#define CHUNKED_BODY (1 << 3)
#define LENGTHED_BODY (1 << 4)
#define MULTIPARTED_BODY (1 << 5)
#define BODY_READ (CHUNKED_BODY | NORMAL_BODY | LENGTHED_BODY | MULTIPARTED_BODY)
#define BODY_DONE (1 << 6)

#define START_BODY (1 << 0)
#define ONGION_BODY (1 << 1)
#define BODY_END (1 << 2)
#define BODY_ERROR (1 << 3)

#define MULTIPART_INIT_BOUNDARY (1 << 0)
#define MULTIPART_INIT_CRLF (1 << 1)
#define MULTIPART_INIT (MULTIPART_INIT_BOUNDARY | MULTIPART_INIT_CRLF)
#define MULTIPART_HEADER (1 << 2)
#define MULTIPART_BODY_INIT (1 << 3)
#define MULTIPART_BODY_BOUNDARY (1 << 4)
#define MULTIPART_BODY_CRLF (1 << 5)
#define MULTIPART_BODY_READ (1 << 6)
#define MULTIPART_BODY_D (1 << 7)
#define MULTIPART_BODY_DD (1 << 8)
#define MULTIPART_BODY (MULTIPART_BODY_INIT | MULTIPART_BODY_BOUNDARY | MULTIPART_BODY_CRLF | MULTIPART_BODY_READ | MULTIPART_BODY_D | MULTIPART_BODY_DD)
#define MULTIPART_DONE (1 << 9)

class Request;
#include <stdio.h>

#include "./sio_header.hpp"

class BodyFile {
	FILE *_file;

   public:
	Header _headers;
	BodyFile() {}

	void addFile(FILE *file) {
		_file = file;
	}
	void write(stringstream &ss) {
		char chr;
		while (!ss.eof()) {
			ss.get(chr);
			fwrite(&chr, 1, 1, _file);
		}
		fflush(_file);
	}
	void write(const string &buff) {
		fwrite(buff.c_str(), 1, buff.size(), _file);
		fflush(_file);
	}
	void addHeader(const string &key, const string &value) {
		_headers.add(key, value);
	}
};

class Body {
	FILE *_bodyFile;

	short  _bodyState;
	short  _readingState;
	size_t _contentLength;
	size_t _chunkedLength;
	size_t _content;
	string _filename;

	Boundary _boundary;

	short        _multipartState;
	string       _line;
	stringstream _lost;

   private:
	void parseChunkedBody(istream &stream);
	void parseLengthedBody(istream &stream);
	void parseNormalBody(istream &stream);
	void parseMultipartBody(istream &stream);

	map<int, BodyFile> _bodyFiles;
	int                _fileIndex;

	void parseHeaders(stringstream &ss);

   public:
	Body();
	Body(const Body &copy);
	Body &operator=(const Body &rhs);
	~Body();
	void chooseState(Header &headers);
	void setState(const int &state);
	void consumeBody(istream &stream, Request *req);

	int   getFileno() const;
	short getState() const;
};

#include "./sio_request.hpp"

#endif
