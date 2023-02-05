#ifndef __PARSER_H__
#define __PARSER_H__

#include "sio_lexer.hpp"
#include "sio_ast.hpp"

class Parser {
	string _serr;
	Lexer  _lex;

	typedef pair<string, vector<string> > Directive;

   private:
	const Token &current();
	bool         accept(int type, const string &value = "");
	bool         expect(int type, const string &value = "");

	Directive *parse_directive(Directive *_dir = nullptr);
	Directive *parse_http_dir(Directive *_dir = nullptr);
	Directive *parse_server_dir();
	Directive *parse_location_dir();


   public:
	Parser(ifstream &cfile);
	MainContext  *parse();
	const string &err() const;
};

#endif