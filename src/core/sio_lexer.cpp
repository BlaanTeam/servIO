#include "sio_lexer.hpp"

const char *TokenNames[7] = {
    "WORD",
    "OPEN CURLY BRACE",
    "CLOSING CURLY BRACE",
    "SEMICOLON",
    "DOUBLE QOUTE",
    "SINGLE QOUTE",
    "END OF FILE"};

string name(int type) {
	if (type > (1 << 7))
		return "UNKNOWN";
	return TokenNames[(int)log2(int(type)) - 1];
}

// Token Class

Token::Token() {}

Token::Token(const TokenType &type, const string &value, const size_t &line) : _type(type), _value(value), _line(line) {}

// Token Getters

TokenType Token::type(void) const {
	return _type;
}

const string &Token::value(void) const {
	return _value;
}

size_t Token::line(void) const {
	return _line;
}

string Token::name() const {
	return ::name(_type);
}

// Lexer Class

Lexer::Lexer() {}

void Lexer::tokenizer(ifstream &file) {
	char   chr;
	size_t line = 1;

	file.get(chr);
	while (!file.eof()) {
		switch (chr) {
		case '#':
			while (chr != '\n' && !file.eof())
				file.get(chr);
		case ' ':
		case '\n':
			chr == '\n' ? line++ : line += 0;
		case '\r':
		case '\f':
		case '\v':
		case '\t':
			break;
		case '{':
		case '}':
			push_back(Token(chr == '}' ? CCURLY : OCURLY, string(1, chr), line));
			break;
		case ';':
			push_back(Token(SEMICOLON, string(1, chr), line));
			break;
		default:
			string word;
			while (!strchr("{};", chr) && !isspace(chr) && !file.eof()) {
				if (chr == '\\')
					file.get(chr);
				word += string(1, chr);
				file.get(chr);
			}
			push_back(Token(WORD, word, line));
			continue;
		};
		file.get(chr);
	}
	push_back(Token(_EOF, "End Of File", line));

	// reset seek of the file to the beginning !
	file.clear();
	file.seekg(0);
}
