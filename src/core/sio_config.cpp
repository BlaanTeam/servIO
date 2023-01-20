#include "sio_config.hpp"

Config::Config(const string &path) : _path(path){};

bool Config::syntaxOnly() const {
	// TODO: implement syntax checking !
	return true;
}

void Config::displayContent(void) const {
	// TODO: implement display content !
	return;
}

// Setters

void Config::setPath(const string &path) {
	_path = path;
}

// Getters

string Config::getPath(void) const {
	return _path;
}