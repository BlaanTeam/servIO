#include "sio_parser.hpp"

// Public member functions

Parser::Parser(ifstream &cfile) {
	_lex.tokenizer(cfile);
};

MainContext<Type> *Parser::parse() {
	MainContext<> *tree = parse_main();

	if (!updateDirectives(tree))
		return delete tree, nullptr;

	pair<bool, MainContext<Type> *> ret = transfer(tree);

	delete tree;

	if (!ret.first)
		return delete ret.second, nullptr;

	return ret.second;
};

const string &Parser::err() const {
	return _serr;
}

// Parsing utils

const Token &Parser::current() {
	return _lex.front();
}

bool Parser::accept(int type, const string &value) {
	if ((current().type() & type) && (value.empty() || current().value() == value))
		return _lex.pop_front(), true;
	return false;
}

bool Parser::expect(int type, const string &value) {
	if (!accept(type, value)) {
		if (_serr.empty())
			_serr = "LINE " + to_string(current().line()) + ": parse error!";
		return false;
	}
	return true;
}

// Top-Down Parser

Parser::Directive *Parser::parse_directive(Parser::Directive *_dir) {
	if (_dir)
		return _dir;

	Parser::Directive *dir = new Directive();

	dir->first = current().value();

	if (!expect(WORD))
		goto failed;

	while (current().type() & WORD) {
		dir->second.push_back(current().value());
		_lex.pop_front();
	}

	if (!expect(SEMICOLON))
		goto failed;

	return dir;

failed:
	return delete dir, nullptr;
}

Parser::Directive *Parser::parse_http_dir(Parser::Directive *_dir) {
	Parser::Directive *dir = parse_directive(_dir);

	if (!dir)
		return nullptr;

	if (dir->first == "root" || dir->first == "index") {
		if (dir->second.size() != 1) {
			_serr = dir->first + " directive: invalid arguments!";
			goto failed;
		}
		return dir;
	}

	if (dir->first == "autoindex") {
		if (dir->second.size() != 1 || !(dir->second[0] == "on" || dir->second[0] == "off")) {
			_serr = "autoindex directive: invalid arguments!";
			goto failed;
		}
		return dir;
	}

	if (dir->first == "client_max_body_size") {
		if (dir->second.size() != 1) {
			_serr = "client_max_body_size directive: invalid arguments!";
			goto failed;
		}
		int    value = 0;
		size_t last;
		stoi(dir->second[0], &last);
		if (!isdigit(dir->second[0][0]) || value < 0 || last < (dir->second[0].size() - 1)) {
			_serr = "client_max_body_size directive: invalid arguments!";
			goto failed;
		} else if (last == (dir->second[0].size() - 1) && !strchr("gmkb", dir->second[0][last])) {
			_serr = "client_max_body_size directive: invalid arguments!";
			goto failed;
		}
		return dir;
	}

	if (dir->first == "error_page") {
		if (dir->second.size() != 2) {
			_serr = "error_page directive: invalid arguments!";
			goto failed;
		}
		for (size_t i = 0; i < dir->second[0].size(); i++) {
			if (!isdigit(dir->second[0][i]) && (dir->second[0][i] != 'x')) {
				_serr = "error_page directive: invalid arguments!";
				goto failed;
			}
		}
		dir->first += "_" + dir->second[0];  // to differentiate between multiple error_page keys
		return dir;
	}

	if (dir->first == "allowed_methods") {
		if (dir->second.empty()) {
			_serr = "allowed_methods directive: invalid arguments!";
			goto failed;
		}
		for (size_t i = 0; i < dir->second.size(); i++) {
			if (dir->second[i] != "GET" && dir->second[i] != "POST" && dir->second[i] != "DELETE") {
				_serr = "allowed_methods directive: invalid arguments!";
				goto failed;
			}
		}
		return dir;
	}

	_serr = "http context: " + dir->first + ": invalid directive!";

failed:
	return delete dir, nullptr;
}

Parser::Directive *Parser::parse_server_dir() {
	Parser::Directive *dir = parse_directive();

	if (!dir)
		return nullptr;

	if (dir->first == "listen" || dir->first == "server_name") {
		if (dir->second.size() != 1) {
			_serr = dir->first + " directive: invalid arguments!";
			goto failed;
		}
		return dir;
	}

	if (parse_http_dir(dir)) {
		return dir;
	}

	if (_serr.substr(0, 4) == "http") {
		_serr = "server context: " + dir->first + ": invalid directive!";
	}

	return nullptr;

failed:
	return delete dir, nullptr;
}

Parser::Directive *Parser::parse_location_dir() {
	Parser::Directive *dir = parse_directive();

	if (!dir)
		return nullptr;

	if (dir->first == "return") {
		if (dir->second.empty() || dir->second.size() > 2) {
			_serr = "return directive: invalid arguments!";
			goto failed;
		}
		size_t sz;
		stoi(dir->second[0], &sz);
		if (sz != dir->second[0].size()) {  // is integer
			_serr = "return directive: invalid arguments!";
			goto failed;
		}
		return dir;
	}

	if (parse_http_dir(dir)) {
		return dir;
	}

	if (_serr.substr(0, 4) == "http") {
		_serr = "location context: " + dir->first + ": invalid directive!";
	}

	return nullptr;

failed:
	return delete dir, nullptr;
}

MainContext *Parser::parse_location() {
	MainContext *ret = new LocationContext();

	if (!expect(WORD, "location"))
		goto failed;

	((LocationContext *)ret)->setLocation(current().value());

	if (!expect(WORD) || !expect(OCURLY))
		goto failed;

	while (current().type() & ~CCURLY) {
		if (current().value() == "location") {
			MainContext *p = parse_location();
			if (!p)
				goto failed;
			ret->addContext(p);
		} else {
			Parser::Directive *dir = parse_location_dir();
			if (!dir)
				goto failed;
			ret->addDirective(*dir);
			delete dir;
		}
	}

	if (!expect(CCURLY))
		goto failed;

	return ret;

failed:
	return delete ret, nullptr;
}

MainContext *Parser::parse_server() {
	MainContext *ret = new ServerContext();

	if (!expect(WORD, "server") || !expect(OCURLY))
		goto failed;

	while (current().type() & ~CCURLY) {
		if (current().value() == "location") {
			MainContext *p = parse_location();
			if (!p)
				goto failed;
			ret->addContext(p);
		} else {
			Parser::Directive *dir = parse_server_dir();
			if (!dir)
				goto failed;
			ret->addDirective(*dir);
			delete dir;
		}
	}

	if (!expect(CCURLY))
		goto failed;

	return ret;

failed:
	return delete ret, nullptr;
}

MainContext *Parser::parse_main() {
	MainContext *ret = new HttpContext();

	if (!expect(WORD, "http") || !expect(OCURLY))
		goto failed;

	while (current().type() & ~CCURLY) {
		if (current().value() == "server") {
			MainContext *p = parse_server();
			if (!p)
				goto failed;
			ret->addContext(p);
		} else {
			Parser::Directive *dir = parse_http_dir();
			if (!dir)
				goto failed;
			ret->addDirective(*dir);
			delete dir;
		}
	}

	if (!expect(CCURLY) || !expect(_EOF))
		goto failed;

	return ret;

failed:
	return delete ret, nullptr;
}

MainContext *Parser::updateDirectives(MainContext *tree, MainContext *parent) {
	if (!tree)
		return nullptr;
	if (parent) {
		map<string, vector<string> > &directives = parent->directives();
		for (map<string, vector<string> >::iterator dir = directives.begin(); dir != directives.end(); dir++) {
			if (tree->directives().find(dir->first) == tree->directives().end()) {
				tree->directives().insert(*dir);
			}
		}
	}
	for (size_t i = 0; i < tree->contexts().size(); i++)
		updateDirectives(tree->contexts()[i], tree);
	return tree;
}