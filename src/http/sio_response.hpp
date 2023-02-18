#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#define RES_INIT (1 << 0)
#define RES_HEADER (1 << 1)
#define RES_BODY (1 << 2)
#define RES_DONE (1 << 3)

#define LENGTHED_RES (1 << 0)
#define CHUNKED_RES (1 << 1)
#define RANGED_RES (1 << 2)

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "./sio_http_codes.hpp"
#include "./sio_mime_types.hpp"
#include "utility/sio_socket.hpp"
#include "utility/sio_utils.hpp"
#include "utility/sio_helpers.hpp"

using namespace std;

class Response {
	stringstream _ss;

	short _state;
	short _statusCode;
	short _type;

	iostream *_stream;

	map<string, string, StringICaseCompare> _headers;
	bool                _keepAlive;

   public:
	Response();
	Response(const short &statusCode, const string &contentType = DEFAULT_MIME_TYPE, bool keepAlive = true);
	Response(const Response &copy);
	Response &operator=(const Response &rhs);

	void init(const string &contentType = DEFAULT_MIME_TYPE);
	void prepare(void);

	// Setters

	void setStatusCode(const short &statusCode);
	void setState(const int &state);
	void setStream(iostream *stream);
	void addHeader(const string &name, const string &value);

	void setConnectionStatus(bool keepAlive = true);

	void send(const sockfd &fd);
};

#endif