#include "lexer.hpp"

/*

<main> = "http" "{" {<key-val> <server> <key-val>}+ "}"

<server> = "server" "{" {<key-val> | <location>} "}"

<location> = "location" <WORD> "{" {<location>} "}"

<key-val> = { {<WORD>}+ ";" }

<WORD> = config file literals

*/

// class MainContext {
// 	map<string, string> directives;
// 	vector<MainContext> contexts;
// };

// class HttpContext : public MainContext {};

// class ServerContext : public HttpContext {};

// class LocationContext: public ServerContext {
// 	LocationContext()
// };
const char *TokenNames[7] = {
    "WORD",
    "OCURLY",
    "CCURLY",
    "SEMICOLON",
    "DQOUTE",
    "SQOUTE",
    "_EOF"};

// Token Class

Token::Token() {}

Token::Token(const TokenType &type, const string &value) : _type(type), _value(value) {}

// Token Getters

TokenType Token::getType(void) const {
	return _type;
}

string Token::getValue(void) const {
	return _value;
}

// Lexer Class

Lexer::Lexer() {}

void Lexer::tokenizer(ifstream &file) {
	char chr;

	file.get(chr);
	while (!file.eof()) {
		switch (chr) {
		case '#':
			while (chr != '\n' && !file.eof())
				file.get(chr);
		case ' ':
		case '\n':
		case '\r':
		case '\f':
		case '\v':
		case '\t':
			break;
		case '{':
		case '}':
			push_back(Token(chr == '}' ? CCURLY : OCURLY, string(1, chr)));
			break;
		case ';':
			push_back(Token(SEMICOLON, string(1, chr)));
			break;
		default:
			string word;
			while (!strchr("{};", chr) && !isspace(chr) && !file.eof()) {
				if (chr == '\\')
					file.get(chr);
				word += string(1, chr);
				file.get(chr);
			}
			push_back(Token(STRING, word));
			continue;
		};
		file.get(chr);
	}
	push_back(Token(_EOF, "End Of File"));

	// reset seek of the file to the beginning !
	file.clear();
	file.seekg(0);
}

int Lexer::current(void) const {
	return front().getType();
}

bool Lexer::match(int tokenType) const {
	return current() & tokenType;
}

bool Lexer::accept(int tokenType) {
	if (match(tokenType)) {
		pop_front();
		return true;
	}
	return false;
}