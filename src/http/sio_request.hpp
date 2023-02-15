#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "./sio_http_codes.hpp"
#include "utility/sio_helpers.hpp"

using namespace std;

#define REQ_INIT (1 << 0)
#define REQ_LINE (1 << 1)
#define REQ_HEADER (1 << 2)
#define REQ_BODY (1 << 3)
#define REQ_DONE (1 << 4)
#define REQ_INVALID (1 << 5)

class Request {
	short _state;

	HttpMethod _method;
	string     _path;
	string     _query;
	string     _line;
	// ofstream   _bodyFile;

	map<string, string> _headers;

   private:
	void parseFirstLine(string &line);
	void parseHeaders(string &line);
	void parseBody(istream &stream);

	void changeState(const int &state);

   public:
	Request();
	Request(const Request &copy);
	Request &operator=(const Request &rhs);
	void consumeStream(istream &stream);

	// Getters
	string getPath(void) const;
	string getQuery(void) const;
	short  getState(void) const;

	bool valid() const;
};

#endif