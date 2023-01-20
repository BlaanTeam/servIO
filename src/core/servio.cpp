#include "servio.hpp"

#include "sio_cmdline_opts.hpp"

void servio_init(const int &ac, char *const *av) {
	Config config;

	if (!parse_options(ac, av, config))
		return;
	cout << "running !\n";
}