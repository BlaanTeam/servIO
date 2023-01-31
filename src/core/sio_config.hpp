#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <fstream>
#include <string>

#include "servio.hpp"
#include "sio_parser.hpp"

using namespace std;

#define CONF_DFL_PATH "conf/servio.conf"

class Config {
	string           _path;
	mutable ifstream _file_stream;

	MainContext *_asTree;

   private:
	bool parse();

   public:
	Config(const string &path = CONF_DFL_PATH);
	bool syntaxOnly();

	void displayContent(void) const;

	// Setters

	void setPath(const string &path);

	// getters
	string getPath(void) const;
	bool   good(void) const;
};

#endif