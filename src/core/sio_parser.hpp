#ifndef __PARSER_H__
#define __PARSER_H__

#include "sio_lexer.hpp"
#include "sio_ast.hpp"

class Parser {
	string _serr;
	Lexer  _lex;

   private:
	bool expect(TokenType type) {
		if (_lex.front().type() != type) {
			_serr = "LINE " + to_string(_lex.front().line()) + ": temp expect token";
			return false;
		}
		return _lex.pop_front(), true;
	}

	
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