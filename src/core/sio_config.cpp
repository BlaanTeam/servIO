#include "sio_config.hpp"

Config::Config(const string &path) {
	setPath(path);
}

bool Config::syntaxOnly() const {
	// TODO: implement syntax checking !

	Lexer lexer;

	lexer.tokenizer(_file_stream);

	Lexer::iterator it = lexer.begin();

	while (it != lexer.end()) {
		cout << setw(10) << left << TokenNames[(int)log2((double)it->getType()) - 1] << " = " << it->getValue() << endl;
		it++;
	}

	return true;
}

void Config::displayContent(void) const {
	char buff[1 << 10];
	while (good() && !_file_stream.eof()) {
		buff[_file_stream.read(buff, (1 << 10) - 1).gcount()] = 0x0;
		cout << buff;
	}
}

// Setters

void Config::setPath(const string &path) {
	_file_stream.close();
	_file_stream.open(path, ios::in);
	if (!good())
		cerr << NAME << ": " << strerror(errno) << endl;
	_path = path;
}

// Getters

string Config::getPath(void) const {
	return _path;
}

bool Config::good(void) const {
	return _file_stream.good();
}