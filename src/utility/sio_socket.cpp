
#include "sio_socket.hpp"

static void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Address

Address::Address(const string &host, const in_port_t &port) : _host(host), _port(port), _ss_family(AF_INET) {}

Address::Address(sockaddr sa, const socklen_t &len) {
	char buff[INET6_ADDRSTRLEN];

	_ss_family = sa.sa_family;

	inet_ntop(sa.sa_family, (const sockaddr *)get_in_addr(&sa), buff, len);  // Todo: change it.

	_host = string(buff);
	_port = ntohs(((sockaddr_in *)&sa)->sin_port);
}

Address::~Address() {}

// Address Setters

void Address::setHost(const string &host) { _host = host; }
void Address::setPort(const short &port) { _port = port; }

// Address Getters
string    Address::getHost(void) const { return _host; }
in_port_t Address::getPort(void) const { return _port; }
sockaddr  Address::getSockAddr(void) const {
    sockaddr sa;

    bzero(&sa, sizeof(sockaddr));

    sa.sa_family = _ss_family;
    sa.sa_len = getSockLen();

    if (_ss_family == AF_INET)
        ((sockaddr_in *)&sa)->sin_port = htons(_port);
    else if (_ss_family == AF_INET6)
        ((sockaddr_in6 *)&sa)->sin6_port = htons(_port);

    inet_pton(_ss_family, _host.c_str(), get_in_addr(&sa)); // Todo: change it.

    return sa;
}

socklen_t Address::getSockLen(void) const { return _ss_family == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6); }

// Socket
Socket::Socket(int domain, int type, int protocol) {
	good = true;
	sock_fd = ::socket(domain, type, protocol);
	if (sock_fd == -1)
		good = false;

	int optval = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

bool   Socket::isGood() const { return good; }
sockfd Socket::getSockFd(void) const {
	return sock_fd;
}

Socket::~Socket() {}

void Socket::bind(const Address &addr) {
	sockaddr sa = addr.getSockAddr();

	good = true;

	if (::bind(sock_fd, &sa, addr.getSockLen()) != 0) {
		good = false;
	}
}

void Socket::listen(int backlog) {
	good = true;
	if (::listen(sock_fd, backlog) != 0)
		good = false;
}

pair<int, Address> Socket::accept(void) {
	sockaddr  sa;
	socklen_t sa_len;

	good = true;
	sockfd new_conn = ::accept(sock_fd, &sa, &sa_len);
	if (new_conn != 0)
		good = false;

	return make_pair(new_conn, Address(sa, sa_len));
}

PollFd::PollFd() {
}

class PollFd::IsExists {
	sockfd _fd;

   public:
	IsExists(const sockfd &fd) : _fd(fd){};
	bool operator()(const pollfd &pfd) {
		return pfd.fd == _fd;
	}
};

void PollFd::add(const sockfd &fd, const short &events) {
	pollfd new_pfd = {fd, events, 0};
	pfds.push_back(new_pfd);
}

void PollFd::remove(const sockfd &fd) {
	vector<pollfd>::iterator it = find_if(pfds.begin(), pfds.end(), IsExists(fd));

	if (it != pfds.end())
		pfds.erase(it);
}

int PollFd::size() const {
	return pfds.size();
}

int PollFd::poll(const int &timeout) {
	return ::poll(pfds.data(), pfds.size(), timeout);
}

vector<pollfd> &PollFd::getPfds(void) {
	return pfds;
}