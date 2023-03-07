#include "servio.hpp"

int main(int ac, char *const *av) {
	handleSignals();
	servio_init(ac, av);
}