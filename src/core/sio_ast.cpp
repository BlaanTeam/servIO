#include "sio_ast.hpp"

ErrorPage::ErrorPage(string pattern, string page) : pattern(pattern), page(page) {}

bool ErrorPage::match(const int &errorCode) const {
	string error = to_string(errorCode);

	for (size_t idx = 0; idx < pattern.length(); idx++)
		if (error[idx] != pattern[idx] && ::tolower(pattern[idx]) != 'x')
			return false;
	return true;
}

bool ErrorPage::exists() const {
	return !access(page.c_str(), F_OK | R_OK);
}

Redirect::Redirect(int code, string path, bool isLocal) : code(code), path(path), isLocal(isLocal) {  //! TODO: change isLocal to _isLocal !!
	isRedirect = (code == 301 || code == 302 || code == 303 || code == 307 || code == 308);
};

void Redirect::prepare(MainContext<Type> *ctx) {
	if (isRedirect) {
		ServerName *serverName = ctx->directives()["server_name"].servName;

		if (path.length() > 0 && path[0] == '/' && !isLocal)
			path = joinPath((*serverName)[0], path);
	}
}

ServerName::ServerName(const vector<string> &vec) : vector<string>(vec) {}

bool ServerName::find(const string &name) {
	return ::find(begin(), end(), name) != end();
}

Type::Type() { bzero(this, sizeof(Type)); }

Type::Type(int type) {
	bzero(this, sizeof(Type));
	this->type = type;
}

Type::Type(const Type &cpy) {
	bzero(this, sizeof(Type));
	*this = cpy;
}

Type &Type::operator=(const Type &cpy) {
	switch (type) {
	case STR:
		delete str;
		break;
	case ADDR:
		delete addr;
		break;
	case ERRPG:
		delete errPage;
		break;
	case REDIR:
		delete redirect;
		break;
	case SERV_NAME:
		delete servName;
		break;
	}
	type = cpy.type;
	switch (type) {
	case BOOL:
		ok = cpy.ok;
		break;
	case INT:
		value = cpy.value;
		break;
	case STR:
		str = new string(*cpy.str);
		break;
	case ADDR:
		addr = new Address(*cpy.addr);
		break;
	case ERRPG:
		errPage = new ErrorPage(*cpy.errPage);
		break;
	case REDIR:
		redirect = new Redirect(*cpy.redirect);
		break;
	case SERV_NAME:
		servName = new ServerName(*cpy.servName);
		break;
	}
	return *this;
}

Type::~Type() {
	switch (type) {
	case STR:
		delete str;
		break;
	case ADDR:
		delete addr;
		break;
	case ERRPG:
		delete errPage;
		break;
	case REDIR:
		delete redirect;
		break;
	case SERV_NAME:
		delete servName;
		break;
	}
}