#include "sio_config.hpp"

Config::Config(const string &path) {
	setPath(path);
}

bool Config::syntaxOnly() const {
	// TODO: implement syntax checking !
	return true;
}

void Config::displayContent(void) const {
	char buff[1 << 10];
	while (good() && !_file_stream.eof()) {
		_file_stream.read(buff, (1 << 10) - 1);
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