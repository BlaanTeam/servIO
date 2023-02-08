#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <map>
#include <string>
#include <sstream>

#include "sio_http_codes.hpp"

using namespace std;

#define REQ_INIT (1 << 0)
#define REQ_HEADER (1 << 1)
#define REQ_BODY (1 << 2)
#define REQ_DONE (1 << 3)
#define REQ_INVALID -1

class Request {
	short _state;

	HttpMethod _method;
	string     _uri;

	map<string, string> _headers;
   
   public:
	Request();

	void consumeStream(istream &stream);

	bool valid() const;
};

#endif