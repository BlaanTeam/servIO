#ifndef __UTILS_H__
#define __UTILS_H__

#include <sys/time.h>

#include <iostream>


#include "core/sio_ast.hpp"

using namespace std;

void dumpConfigDot(MainContext *main, ostream &stream = cout);

string    getUTCDate(void);
long long getmstime(void);

#endif