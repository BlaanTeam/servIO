#ifndef __CLIENT_H__
#define __CLIENT_H__

#define TIMEOUT 100000  // TODO: change it to be reasonable

#include <unistd.h>

#include <iostream>
#include <map>

#include "./sio_request.hpp"
#include "./sio_response.hpp"

using namespace std;

class Client {
	pollfd               *_pfd;
	pair<sockfd, Address> _connection;
	long long             _time;
	Request               _req;
	Response              _res;

   public:
	Client();
	Client(const pair<sockfd, Address> &connection);

	// Setters
	void setTime(const long long &time);
	void setPollFd(const sockfd &fd, PollFd &pfd);

	// Getters
	bool timedOut() const;

	void handleRequest(stringstream &stream);
};

class ClientMap : public map<sockfd, Client> {
	PollFd *_pfds;

	const vector<sockfd> getInactiveClients(void);

   public:
	ClientMap();

	void changePollFds(PollFd *pfds);

	int purgeConnection(const sockfd &fd);

	int purgeInactiveClients(void);
};

extern ClientMap clients;

#endif