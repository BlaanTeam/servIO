#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>

#include "sio_ast.hpp"

using namespace std;

void dumpConfigDot(MainContext *main, ostream &stream = cout);


string getUTCDate(void);

#endif