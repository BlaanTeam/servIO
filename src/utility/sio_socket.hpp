#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

typedef int sockfd;

/*
    The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow.
*/
#define BACKLOG 10

using namespace std;

class Address {
	string    _host;
	in_port_t _port;
	short     _ss_family;

   public:
	Address(const string &host, const in_port_t &port);
	Address(sockaddr addr, const socklen_t &len);

	// setters
	void setHost(const string &host);
	void setPort(const short &port);

	// getters
	string    getHost(void) const;
	in_port_t getPort(void) const;
	sockaddr  getSockAddr(void) const;
	socklen_t getSockLen(void) const;

	~Address();
};

class Socket {
	sockfd sock_fd;
	bool   good;

   public:
	Socket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	void               bind(const Address &addr);
	void               listen(int backlog = BACKLOG);
	pair<int, Address> accept();

	// getters
	bool   isGood() const;
	sockfd getSockFd(void) const;

	~Socket();
};

class PollFd {
	vector<pollfd>                   pfds;
	typedef vector<pollfd>::iterator iterator;

	class IsExists;

   public:
	PollFd();
	void add(const sockfd &fd, const short &events);
	void remove(const sockfd &fd);

	iterator begin(void) { return pfds.begin(); }
	iterator end(void) { return pfds.end(); }

	int             size(void) const;
	int             poll(const int &timeout);
	vector<pollfd> &getPfds(void);
};

#endif