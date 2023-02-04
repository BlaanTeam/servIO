#include "sio_ast.hpp"

// MainContext functions

MainContext::MainContext() {}

MainContext::MainContext(const MainContext *copy) {
	_directives = copy->_directives;
}

CtxType MainContext::type() {
	return _type;
}

map<string, vector<string> > &MainContext::directives() {
	return _directives;
}

vector<MainContext *> &MainContext::contexts() {
	return _contexts;
}

void MainContext::addContext(MainContext *ctx) {
	_contexts.push_back(ctx);
}

void MainContext::addDirective(const Directive &dir) {
	_directives[dir.first] = dir.second;
}

void MainContext::rmDirective(const string &dir) {
	dirIter it = _directives.find(dir);
	if (it != _directives.end()) _directives.erase(it);
}

vector<string> &MainContext::operator[](const string &dir) {
	return _directives[dir];
}

MainContext::~MainContext() {
	for (size_t idx = 0; idx < _contexts.size(); idx++)
		delete _contexts[idx];
};

// HttpContext functions

HttpContext::HttpContext() { _type = httpCtx; };

HttpContext::HttpContext(const MainContext *copy)
    : MainContext(copy) { _type = httpCtx; }

// ServerContext functions

ServerContext::ServerContext() { _type = serverCtx; };

ServerContext::ServerContext(const MainContext *copy)
    : MainContext(copy) { _type = serverCtx; }

// LocationContext functions

LocationContext::LocationContext(const string &loc)
    : _loc(loc) { _type = locationCtx; };

LocationContext::LocationContext(const MainContext *copy)
    : MainContext(copy) { _type = locationCtx; }

const string &LocationContext::location() const {
	return _loc;
}

void LocationContext::setLocation(const string &loc) {
	_loc = loc;
}
