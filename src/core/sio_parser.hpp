#ifndef __PARSER_H__
#define __PARSER_H__

#include "sio_lexer.hpp"
#include "sio_ast.hpp"

class Parser {
	string _serr;
	Lexer _lex;

   private:
	
   public:
	Parser(ifstream &cfile) {
		_lex.tokenizer(cfile);
	};
	MainContext *parse() {
		if (_lex.front().type() == _EOF) {
			_serr = "empty config";
			return nullptr;
		}
		return parse_main();
	};
	const string &err() const {
		return _serr;
	}
};

#endif