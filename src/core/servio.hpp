#ifndef __SERVIO_H__
#define __SERVIO_H__

#define NAME "ServIo"
#define VERSION "1.0.0"

#define CR "\r"
#define LF "\n"
#define CRLF "\r\n"

#include <map>
#include <set>

#include "http/sio_barrel.hpp"
#include "sio_lexer.hpp"
#include "sio_utils.hpp"
#include "utility/sio_barrel.hpp"

using namespace std;

void servio_init(const int &ac, char *const *av);

#endif