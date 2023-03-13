#ifndef __CGI_H__
#define __CGI_H__

#include <map>
#include <sstream>
#include <string>

#include "core/sio_ast.hpp"
#include "./sio_request.hpp"
#include "./sio_response.hpp"
#include "./sio_header.hpp"

using namespace std;

class CGI {
	string _scriptName;
	string _pathInfo;

	Header metaVariables;

	Request               *_req;
	Response              *_res;
	LocationContext<Type> *_location;

	bool _isCGI;

   public:
	string _scriptFileName;
	CGI(LocationContext<Type> *location, Request *req, Response *res);

	bool valid() const;

	void init();
	void setenv();

	pid_t spawn(int *fds, const int &fileno);
};

#endif