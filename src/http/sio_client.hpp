#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>

#include "./sio_cgi.hpp"
#include "./sio_request.hpp"
#include "./sio_response.hpp"
#include "core/sio_config.hpp"

using namespace std;

class Client {
	pollfd               *_pfd;
	pair<sockfd, Address> _connection;
	long long             _time;
	Request               _req;
	Response              _res;

	MainContext<Type> *_ctx;

	int   _fds[2];
	pid_t _pid;

   public:
	Client();
	Client(const pair<sockfd, Address> &connection);

	// Setters
	void setTime(const long long &time);
	void setPollFd(const sockfd &fd, PollFd &pfd);

	// Getters
	bool timedOut() const;

	bool handleRequest(stringstream &stream);

	void handleResponse(const sockfd &fd);

	bool isInternalServerError();

	void reset(void);
	
	void togglePollOut(void);
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