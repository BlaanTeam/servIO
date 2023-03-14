#ifndef __BODY_H__
#define __BODY_H__

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "./sio_header.hpp"
#include "utility/sio_helpers.hpp"
#include "utility/sio_utils.hpp"

using namespace std;

#define BODY_INIT (1 << 0)
#define BODY_OPEN (1 << 1)
#define NORMAL_BODY (1 << 2)
#define CHUNKED_BODY (1 << 3)
#define LENGTHED_BODY (1 << 4)
#define BODY_READ (CHUNKED_BODY | NORMAL_BODY | LENGTHED_BODY)
#define BODY_DONE (1 << 5)

#define START_BODY (1 << 0)
#define ONGION_BODY (1 << 1)
#define BODY_END (1 << 2)
#define BODY_ERROR (1 << 3)

class Request;

class Body {
	FILE *_bodyFile;

	short  _bodyState;
	short  _readingState;
	size_t _contentLength;
	size_t _chunkedLength;
	size_t _content;
	string _filename;

   private:
	void parseChunkedBody(istream &stream);
	void parseLengthedBody(istream &stream);
	void parseNormalBody(istream &stream);

   public:
	Body();
	Body(const Body &copy);
	Body &operator=(const Body &rhs);
	~Body();
	void chooseState(Header &headers);
	void setState(const int &state);
	void consumeBody(istream &stream, Request *req);

	int getFileno() const;
	short getState() const;
};

#include "./sio_request.hpp"

#endif
