#ifndef __AST_H__
#define __AST_H__

#include <map>
#include <string>
#include <vector>

using namespace std;

enum CtxType {
	httpCtx,
	serverCtx,
	locationCtx,
};

class MainContext {
   protected:
	CtxType                                        _type;
	vector<MainContext *>                          _contexts;
	map<string, vector<string> >                   _directives;
	typedef map<string, vector<string> >::iterator dirIter;

   public:
	MainContext();
	MainContext(const MainContext *copy);

	CtxType                       type();
	map<string, vector<string> > &directives();
	vector<MainContext *>        &contexts();

	void            addContext(MainContext *ctx);
	void            rmDirective(const string &dir);
	vector<string> &operator[](const string &dir);

	virtual ~MainContext();
};

class HttpContext : public MainContext {
   public:
	HttpContext();
	HttpContext(const MainContext *copy);
};

class ServerContext : public MainContext {
   public:
	ServerContext();
	ServerContext(const MainContext *copy);
};

class LocationContext : public MainContext {
   public:
	LocationContext();
	LocationContext(const MainContext *copy);
};

#endif