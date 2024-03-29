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
#define CHUNKED_BODY (1 << 2)
#define LENGTHED_BODY (1 << 3)
#define MULTIPARTED_BODY (1 << 4)
#define BODY_READ (CHUNKED_BODY | LENGTHED_BODY | MULTIPARTED_BODY)
#define BODY_DONE (1 << 5)

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
#define MULTIPART_BODY_LR (1 << 7)
#define MULTIPART_BODY_LF (1 << 8)
#define MULTIPART_BODY_D (1 << 9)
#define MULTIPART_BODY_DD (1 << 10)
#define MULTIPART_BODY (MULTIPART_BODY_INIT | MULTIPART_BODY_BOUNDARY | MULTIPART_BODY_CRLF | MULTIPART_BODY_READ | MULTIPART_BODY_D | MULTIPART_BODY_DD | MULTIPART_BODY_LR | MULTIPART_BODY_LF)
#define MULTIPART_DONE (1 << 11)

class Request;
#include <stdio.h>

#include "./sio_header.hpp"

class BodyFile {
	FILE  *_file;
	string _filename;

   public:
	Header _headers;
	BodyFile();
	~BodyFile();

	void addFile(FILE *file, const string &filename);
	void addHeader(const string &key, const string &value);
	void write(stringstream &ss);

	FILE  *getFile();
	string getFilename() const;

	string extractFilename();
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
	void parseChunkedBody(stringstream &stream);
	void parseLengthedBody(stringstream &stream);
	void parseMultipartBody(stringstream &stream);

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
	void consumeBody(stringstream &stream);

	map<int, BodyFile> &getBodyFiles();

	int   getFileno() const;
	short getState() const;

	void openFile();
	void closeFile();
	void reset();
};

#include "./sio_request.hpp"

#endif
