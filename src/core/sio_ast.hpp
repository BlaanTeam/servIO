#ifndef __AST_H__
#define __AST_H__

#include <map>
#include <string>
#include <vector>

#include "sio_socket.hpp"

using namespace std;

enum CtxType {
	httpCtx = 1 << 1,
	serverCtx = 1 << 2,
	locationCtx = 1 << 3,
};

enum TypeList {
	INT = 1 << 0,
	STR = 1 << 1,
	BOOL = 1 << 2,
	ADDR = 1 << 3,
	ERRPG = 1 << 4,
	REDIR = 1 << 5,
	SERV_NAME = 2 << 6,
};

struct ErrorPage {
	string pattern, page;
	ErrorPage(string pattern = "", string page = "");
};

struct Redirect {
	int    code;
	string path;
	Redirect(int code = 301, string path = "");
};

struct ServerName : public vector<string> {
	bool find(const string &name);
};

struct Type {
	int type;
	union {
		long long   value;
		bool        ok;
		string     *str;
		Address    *addr;
		ErrorPage  *errPage;
		Redirect   *redirect;
		ServerName *servName;
	};
	Type();
	Type(int type);
	Type(Type &cpy);
	Type &operator=(Type &cpy);
	~Type();
};

template <class T = vector<string> >
class MainContext {
   protected:
	CtxType                  _type;
	vector<MainContext<T> *> _contexts;
	map<string, T>           _directives;

   public:
	typedef typename map<string, T>::iterator dirIter;
	typedef pair<string, T>                   Directive;

	MainContext() {}

	MainContext(const MainContext<T> *copy) {
		_directives = copy->_directives;
	}

	CtxType type() {
		return _type;
	}

	map<string, T> &directives() {
		return _directives;
	}

	vector<MainContext<T> *> &contexts() {
		return _contexts;
	}

	void addContext(MainContext<T> *ctx) {
		_contexts.push_back(ctx);
	}

	void addDirective(const Directive &dir) {
		_directives[dir.first] = dir.second;
	}

	void rmDirective(const string &dir) {
		dirIter it = _directives.find(dir);
		if (it != _directives.end()) _directives.erase(it);
	}

	T &operator[](const string &dir) {
		return _directives[dir];
	}

	virtual ~MainContext() {
		for (size_t idx = 0; idx < _contexts.size(); idx++)
			delete _contexts[idx];
	};
};

template <class T = vector<string> >
class HttpContext : public MainContext<T> {
   public:
	HttpContext() {
		MainContext<T>::_type = httpCtx;
	};

	HttpContext(const MainContext<T> *copy)
	    : MainContext<T>(copy) { MainContext<T>::_type = httpCtx; }

	~HttpContext(){};
};

template <class T = vector<string> >
class ServerContext : public MainContext<T> {
   public:
	ServerContext() { MainContext<T>::_type = serverCtx; };

	ServerContext(const MainContext<T> *copy)
	    : MainContext<T>(copy) { MainContext<T>::_type = serverCtx; }

	~ServerContext(){};
};

template <class T = vector<string> >
class LocationContext : public MainContext<T> {
	string _loc;

   public:
	LocationContext(const string &loc = "")
	    : _loc(loc) { MainContext<T>::_type = locationCtx; };

	LocationContext(const MainContext<T> *copy)
	    : MainContext<T>(copy) { MainContext<T>::_type = locationCtx; }

	const string &location() const {
		return _loc;
	}

	void setLocation(const string &loc) {
		_loc = loc;
	}

	~LocationContext(){};
};

#endif