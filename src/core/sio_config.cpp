#include "sio_config.hpp"

Config config;

Config::Config(const string &path) {
	setPath(path);
}

bool Config::parse() {
	Parser parser(_file_stream);
	if (!(_asTree = parser.parse())) {
		cerr << parser.err() << endl;
		return false;
	}
	return true;
}

bool Config::syntaxOnly() {
	return parse();
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

MainContext<Type> *Config::ast() {
	return _asTree;
}

string Config::getPath(void) const {
	return _path;
}

bool Config::good(void) const {
	return _file_stream.good();
}