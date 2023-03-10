#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#define RES_INIT (1 << 0)
#define RES_HEADER (1 << 1)
#define RES_BODY (1 << 2)
#define RES_DONE (1 << 3)

#define LENGTHED_RES (1 << 0)
#define CHUNKED_RES (1 << 1)
#define RANGED_RES (1 << 2)
#define CGI_RES (1 << 3)

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "./sio_http_codes.hpp"
#include "./sio_mime_types.hpp"
#include "./sio_request.hpp"
#include "utility/sio_helpers.hpp"
#include "utility/sio_socket.hpp"
#include "utility/sio_utils.hpp"

#define TIMEOUT 15000  // TODO: change it to be reasonable
#define CHUNK_SIZE 1024

using namespace std;

class Response {
	stringstream _ss;

	short _statusCode;
	short _type;
	short _state;

	iostream *_stream;
	int       _fd;

	map<string, string, StringICaseCompare> _headers;
	bool                                    _keepAlive;

   public:
	Response();
	Response(const short &statusCode, const string &contentType = DEFAULT_MIME_TYPE, bool keepAlive = true);
	Response(const Response &copy);
	Response &operator=(const Response &rhs);
	~Response();

	void init(const string &contentType = DEFAULT_MIME_TYPE);
	void prepare(void);

	// Setters

	void setStatusCode(const short &statusCode);
	void setState(const int &state);
	void setStream(iostream *stream);
	void addHeader(const string &name, const string &value);

	void setConnectionStatus(bool keepAlive = true);

	bool keepAlive(void) const;

	void send(const sockfd &fd);

	void setupErrorResponse(const int &statusCode, MainContext<Type> *ctx, bool isBuiltIn = true);
	void setupRedirectResponse(Redirect *redir, MainContext<Type> *ctx);
	void setupDirectoryListing(const string &path, const string &title);
	void setupNormalResponse(const string &path, iostream *file);
	void setupCGIResponse(const int &fd);

	bool match(const int &state) const;

	void reset(void);

   private:
	void sendLengthedBody(const sockfd &fd);
	void setupLengthedBody(void);

	void sendChunkedBody(const sockfd &fd);
	void setupChunkedBody(void);

	void sendRangedBody(const sockfd &fd);
	void setupRangedBody(void);

	void parseHeaders(stringstream &ss);
	void changeState(const int &state);

	void sendCGIBody(const sockfd &fd);
	bool setupCGIBody();
};

#endif