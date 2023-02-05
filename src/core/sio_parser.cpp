#include "sio_parser.hpp"

// Public member functions

Parser::Parser(ifstream &cfile) {
	_lex.tokenizer(cfile);
};

MainContext *Parser::parse() {
	if (current().type() == _EOF) {
		_serr = "empty config";
		return nullptr;
	}
	return updateDirectives(parse_main());
};

const string &Parser::err() const {
	return _serr;
}

// Parsing utils

const Token &Parser::current() {
	return _lex.front();
}

bool Parser::accept(int type, const string &value) {
	if ((current().type() & type) && (value.empty() || current().value() == value))
		return _lex.pop_front(), true;
	return false;
}

bool Parser::expect(int type, const string &value) {
	if (!accept(type, value)) {
		if (_serr.empty())
			_serr = "LINE " + to_string(current().line()) + ": parse error!";
		return false;
	}
	return true;
}
