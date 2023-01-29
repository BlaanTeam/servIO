#ifndef __LEXER_H__
#define __LEXER_H__

#include <math.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

extern const char *TokenNames[7];

enum TokenType {
	STRING = 1 << 1,
	OCURLY = 1 << 2,
	CCURLY = 1 << 3,
	SEMICOLON = 1 << 4,
	DQOUTE = 1 << 5,
	SQOUTE = 1 << 6,
	_EOF = 1 << 7
};

class Token {
	TokenType _type;
	string    _value;

   public:
	Token();
	Token(const TokenType &type, const string &value);

	// Getters
	TokenType getType(void) const;
	string    getValue(void) const;
};

class Lexer {
	mutable vector<Token> _tokens;

   public:
	// Member Types
	typedef vector<Token>::iterator iterator;

   public:
	Lexer();

	void tokenizer(ifstream &file);

	// Iterator

	iterator begin() const;
	iterator end() const;

	// Getters
	vector<Token> &getTokens(void) const;
};

#endif