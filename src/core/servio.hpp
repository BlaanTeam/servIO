#ifndef __SERVIO_H__
#define __SERVIO_H__

#include <map>
#include <set>

#include "./sio_cmdline_opts.hpp"
#include "./sio_config.hpp"
#include "http/sio_client.hpp"
#include "utility/sio_socket.hpp"

using namespace std;

void servio_init(const int &ac, char *const *av);
void handleSignals(void);

#endif