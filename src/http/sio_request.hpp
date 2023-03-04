#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "./sio_http_codes.hpp"
#include "./sio_request_body.hpp"
#include "utility/sio_helpers.hpp"
#include "utility/sio_utils.hpp"

using namespace std;

#define REQ_INIT (1 << 0)
#define REQ_LINE (1 << 1)
#define REQ_HEADER (1 << 2)
#define REQ_BODY (1 << 3)
#define REQ_DONE (1 << 4)
#define REQ_INVALID (1 << 5)

class Body;

class Request {
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
	string                                   getPath(void) const;
	string                                   getQuery(void) const;
	short                                    getState(void) const;
	int                                      getStatusCode() const;
	int                                      getFileno() const;
	HttpMethod                               getMethod(void) const;
	map<string, string, StringICaseCompare> &getHeaders(void) const;
	bool                                     match(const int &state) const;

	void reset(void);

	bool valid() const;
	bool isTooLarge(const int &clientMaxSize) ;
};

#endif