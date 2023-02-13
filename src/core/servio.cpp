#include "servio.hpp"

static int isInSockets(const sockfd &fd, const vector<Socket> &vec) {
	vector<Socket>::const_iterator it = vec.begin();

	while (it != vec.end()) {
		if (fd == it->getSockFd())
			return distance(vec.begin(), it);
		it++;
	}
	return -1;
}

static void initListeningSockets(const set<Address> &addrs, vector<Socket> &sockets, PollFd &pfds) {
	set<Address>::const_iterator it = addrs.begin();

	while (it != addrs.end()) {
		Socket socket;

		if (!socket.isGood())
			perror("socket");

		socket.bind(*it);
		if (!socket.isGood())
			perror("bind");

		socket.listen();
		if (!socket.isGood())
			perror("listen");

		pfds.add(socket.getSockFd(), POLLIN);
		sockets.push_back(socket);
		it++;
	}
}

void servio_init(const int &ac, char *const *av) {
	Config config;

	if (!parse_options(ac, av, config))
		return;

	// TODO: uncomment the above line!
	// config.syntaxOnly();

	// TODO: load the addresses from the config!
	set<Address> addrs;

	addrs.insert(Address("0.0.0.0", 80));
	addrs.insert(Address("0.0.0.0", 443));
	addrs.insert(Address("127.0.0.1", 4040));

	vector<Socket> sockets(addrs.size());
	PollFd         pfds;

	initListeningSockets(addrs, sockets, pfds);

	while (true) {
		int ret = pfds.poll(TIMEOUT);
		if (ret == -1)
			perror("poll");

		clients.changePollFds(&pfds);
		clients.purgeInactiveClients();

		PollFd::iterator it = pfds.begin();
		PollFd           tmp = pfds;

		while (it != pfds.end()) {
			clients.changePollFds(&tmp);
			const int idx = isInSockets(it->fd, sockets);
			if (idx != -1 && it->revents & POLLIN) {
				pair<int, Address> newConnection = sockets[idx].accept();
				cerr << "New Connection From " << newConnection.second << endl;
				tmp.add(newConnection.first, POLLIN);
				clients[newConnection.first] = Client(newConnection);
			} else if (it->revents & POLLIN) {
				char stream[(1 << 0xA) + 1];
				int  nbyte = recv(it->fd, stream, (1 << 0xA), 0);
				if (nbyte != -1) {
					stream[nbyte] = 0x0;
					istringstream iss(stream);
					clients[it->fd].handleRequest(iss);
					clients[it->fd].setTime(getmstime());
				}
			}
			if (it->revents & POLLHUP) {
				cerr << "Client Disconnected !" << endl;
				clients.purgeConnection(it->fd);
			}
			it++;
		}
		pfds = tmp;
	}
}