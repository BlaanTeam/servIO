#include "sio_cgi.hpp"

CGI::CGI(LocationContext<Type> *location, Request *req, Response *res) : _isCGI(false) {
	_location = location;
	_req = req;
	_res = res;
	stringstream ss(req->getPath().substr(location->location().length() + 1)); // ! GOT ME : 1zx0
	string       tmp;
	string       scriptFileName;

	getline(ss, tmp, '/');
	getline(ss, _pathInfo, '\0');

	_scriptName = joinPath(location->location(), tmp);
	_scriptFileName = _scriptName;

	struct stat fileStat;
	bzero(&fileStat, sizeof fileStat);

	CgiExtension *cgiExt = location->getCGIExtensions();

	if (cgiExt->match(tmp) && location->found(_scriptFileName, fileStat)) {
		if (!access(_scriptFileName.c_str(), X_OK))
			_isCGI = true;
	}
}

bool CGI::valid() const {
	return _isCGI;
}

void CGI::init() {
	metaVariables["GATEWAY_INTERFACE"] = "CGI/1.1";
	metaVariables["DOCUMENT_ROOT"] = *_location->directives()["root"].str;
	metaVariables["QUERY_STRING"] = _req->getQuery();
	metaVariables["REQUEST_METHOD"] = httpMethods[(int)log2((int)_req->getMethod())];
	metaVariables["REQUEST_URI"] = _req->getPath();
	if (!_req->getQuery().empty())
		metaVariables["REQUEST_URI"] += "?" + _req->getQuery();
	metaVariables["SCRIPT_FILENAME"] = _scriptFileName;
	metaVariables["SCRIPT_NAME"] = _scriptName;
	metaVariables["PATH_INFO"] = _pathInfo;

	// TODO: add needed variables !

	Request::headerIter it = _req->getHeaders().begin();

	while (it != _req->getHeaders().end()) {
		metaVariables["HTTP_" + it->first] = it->second;
		it++;
	}
}

void CGI::setenv() {
	map<string, string>::iterator it = metaVariables.begin();
	while (it != metaVariables.end()) {
		::setenv(it->first.c_str(), it->second.c_str(), 1);
		it++;
	}
}

pid_t CGI::spawn(int *fds, const int &fileno) {
	pipe(fds);
	int pid = fork();
	if (!pid) {
		lseek(fileno, 0, SEEK_SET);

		dup2(fileno, STDIN_FILENO);
		close(fds[0]);
		close(fileno);

		dup2(fds[1], STDOUT_FILENO);
		close(fds[1]);

		init();
		setenv();
		execvp(_scriptFileName.c_str(), (char *[]){NULL});
		perror("execvp");
		exit(1);
	}
	close(fds[1]);
	return pid;
}