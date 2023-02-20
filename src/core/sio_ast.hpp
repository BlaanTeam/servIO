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

struct Type {
	int type;
	union {
		long long  value;
		bool       ok;
		string    *str;
		Address   *addr;
		ErrorPage *errPage;
		Redirect  *redirect;
	};
	Type();
	Type(int type);;
	Type(Type &cpy);
	Type &operator = (Type &cpy);
	~Type();
};

class MainContext {
   protected:
	CtxType                                        _type;
	vector<MainContext *>                          _contexts;
	map<string, vector<string> >                   _directives;
	typedef map<string, vector<string> >::iterator dirIter;
	typedef pair<string, vector<string> >          Directive;

   public:
	MainContext();
	MainContext(const MainContext *copy);

	CtxType                       type();
	map<string, vector<string> > &directives();
	vector<MainContext *>        &contexts();

	void            addContext(MainContext *ctx);
	void            addDirective(const Directive &dir);
	void            rmDirective(const string &dir);
	vector<string> &operator[](const string &dir);

	virtual ~MainContext();
};

class HttpContext : public MainContext {
   public:
	HttpContext();
	HttpContext(const MainContext *copy);
	~HttpContext();
};

class ServerContext : public MainContext {
   public:
	ServerContext();
	ServerContext(const MainContext *copy);
	~ServerContext();
};

class LocationContext : public MainContext {
	string _loc;

   public:
	LocationContext(const string &loc = "");
	const string &location() const;
	void          setLocation(const string &loc);
	LocationContext(const MainContext *copy);
	~LocationContext();
};

#endif