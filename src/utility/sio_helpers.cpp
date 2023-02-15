#include "sio_helpers.hpp"

pair<bool, string> normpath(const string &path, const char sep) {
	deque<string> components;
	stringstream  ss(path);
	string        component;

	if (path[0] != sep)
		return make_pair(false, "");

	getline(ss, component, sep);  // skip empty component !!
	while (getline(ss, component, sep)) {
		if (component == ".")
			continue;
		else if (component == "..") {
			if (components.empty())
				return make_pair(false, "");
			components.pop_back();
		} else
			components.push_back(component);
	}
	string tmp = "/";
	for (size_t idx = 0; idx < components.size(); idx++) {
		if (idx != 0)
			tmp += "/";
		tmp += components[idx];
	}
	return make_pair(true, tmp);
}
