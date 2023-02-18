#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "./sio_http_codes.hpp"
#include "utility/sio_helpers.hpp"
#include "utility/sio_utils.hpp"

using namespace std;

#define REQ_INIT (1 << 0)
#define REQ_LINE (1 << 1)
#define REQ_HEADER (1 << 2)
#define REQ_BODY (1 << 3)
#define REQ_DONE (1 << 4)
#define REQ_INVALID (1 << 5)

#define BODY_INIT (1 << 0)
#define BODY_OPEN (1 << 1)
#define NORMAL_BODY (1 << 2)
#define CHUNKED_BODY (1 << 3)
#define LENGTHED_BODY (1 << 4)
#define BODY_READ (CHUNKED_BODY | NORMAL_BODY | LENGTHED_BODY)
#define BODY_DONE (1 << 5)

class Request;

class Body {
	FILE *_bodyFile;

	short  _bodyState;
	size_t _contentLength;
	size_t _content;
	string _filename;

   public:
	Body();
	Body(const Body &copy);
	Body &operator=(const Body &rhs);
	~Body();
	void chooseState(map<string, string, StringICaseCompare> &headers);
	void setState(const int &state);
	void consumeBody(istream &stream, Request &req);
};

class Request : public Body {
	short _state;
	short _statusCode;

	HttpMethod _method;
	string     _path;
	string     _query;
	string     _line;

	map<string, string, StringICaseCompare> _headers;
	Body                                    _body;

   public:
	typedef map<string, string, StringICaseCompare>::iterator headerIter;

   private:
	void parseFirstLine(string &line);
	void parseHeaders(string &line);
	void parseBody(stringstream &stream);

	void changeState(const int &state);

   public:
	Request();
	Request(const Request &copy);
	Request &operator=(const Request &rhs);
	void     consumeStream(stringstream &stream);

	// Getters
	string               getPath(void) const;
	string               getQuery(void) const;
	short                getState(void) const;
	map<string, string, StringICaseCompare> &getHeaders(void) const;

	bool valid() const;
};

#endif