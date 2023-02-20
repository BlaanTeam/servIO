#include "sio_ast.hpp"

// MainContext functions

MainContext::MainContext() {}

MainContext::MainContext(const MainContext *copy) {
	_directives = copy->_directives;
}

Type::Type() { bzero(this, sizeof(Type)); }

Type::Type(int type) {
	bzero(this, sizeof(Type));
	this->type = type;
}

Type::Type(Type &cpy) {
	bzero(this, sizeof(Type));
	*this = cpy;
}

Type &Type::operator = (Type &cpy) {
	if (type & STR) delete str;
	else if (type & ADDR) delete addr;
	else if (type & ERRPG) delete errPage;
	else if (type & REDIR) delete redirect;
	type = cpy.type;
	if (type & STR) str = new string(*cpy.str);
	else if (type & ADDR) addr = new Address(*cpy.addr);
	else if (type & ERRPG) errPage = new ErrorPage(*cpy.errPage);
	else if (type & REDIR) redirect = new Redirect(*cpy.redirect);
	else if (type & BOOL) ok = cpy.ok;
	else value = cpy.value;
	return *this;
}

Type::~Type() {
	if (type & STR) delete str;
	else if (type & ADDR) delete addr;
	else if (type & ERRPG) delete errPage;
	else if (type & REDIR) delete redirect;
}