#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <fstream>
#include <string>
using namespace std;

#define CONF_DFL_PATH "conf/servio.conf"

class Config {
	string   _path;
	ifstream _file_stream;

   public:
	Config(const string &path = CONF_DFL_PATH);
	bool syntaxOnly() const;

	void displayContent(void) const;

	// Setters

	void setPath(const string &path);

	// getters
	string getPath(void) const;
};

#endif