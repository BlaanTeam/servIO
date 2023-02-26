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

void ErrorPage::setRoot(const string &root) {
	// ! TODO : this function will be removed !
	if (!_root)
	{
		page = joinPath(root, page);
		_root = true;
	}
}

Redirect::Redirect(int code, string path) : code(code), path(path){};

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
	if (type & STR)
		delete str;
	else if (type & ADDR)
		delete addr;
	else if (type & ERRPG)
		delete errPage;
	else if (type & REDIR)
		delete redirect;
	else if (type & SERV_NAME)
		delete servName;
	type = cpy.type;
	if (type & STR)
		str = new string(*cpy.str);
	else if (type & ADDR)
		addr = new Address(*cpy.addr);
	else if (type & ERRPG)
		errPage = new ErrorPage(*cpy.errPage);
	else if (type & REDIR)
		redirect = new Redirect(*cpy.redirect);
	else if (type & SERV_NAME)
		servName = new ServerName(*cpy.servName);
	else if (type & BOOL)
		ok = cpy.ok;
	else
		value = cpy.value;
	return *this;
}

Type::~Type() {
	if (type & STR)
		delete str;
	else if (type & ADDR)
		delete addr;
	else if (type & ERRPG)
		delete errPage;
	else if (type & REDIR)
		delete redirect;
	else if (type & SERV_NAME)
		delete servName;
}