#ifndef __CGI_H__
#define __CGI_H__

#include <map>
#include <sstream>
#include <string>

#include "core/sio_ast.hpp"
#include "sio_request.hpp"
#include "sio_response.hpp"

using namespace std;

extern char **environ;

class CGI {
	string _scriptName;
	string _pathInfo;

	map<string, string> metaVariables;

	Request               *_req;
	Response              *_res;
	LocationContext<Type> *_location;

	bool _isCGI;

   public:
	string _scriptFileName;
	CGI(const string &ext, LocationContext<Type> *location, Request *req, Response *res);

	bool valid() const;

	void init();
	void setenv();
};

#endif