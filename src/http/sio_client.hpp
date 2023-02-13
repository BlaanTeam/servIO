#ifndef __CLIENT_H__
#define __CLIENT_H__

#define TIMEOUT 7000  // TODO: change it to be reasonable

#include <unistd.h>

#include <map>
#include <sstream>

#include "sio_request.hpp"
#include "sio_response.hpp"
#include "utility/sio_barrel.hpp"

using namespace std;

class Client {
	pair<sockfd, Address> _connection;
	long long             _time;
	Request               _req;

   public:
	Client();
	Client(const pair<sockfd, Address> &connection);

	// Setters
	void setTime(const long long &time);

	// Getters
	bool timedOut() const;

	void handleRequest(istream &stream);
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