#ifndef __CLIENT_H__
#define __CLIENT_H__

#define TIMEOUT 7000 // TODO: change it to be reasonable

#include <map>
#include <sstream>

#include "servio.hpp"
#include "sio_request.hpp"
#include "sio_socket.hpp"

using namespace std;

class Client {
	pair<sockfd, Address> _connection;
	long long             _time;
	Request               req;

   public:
	Client();
	Client(const pair<sockfd, Address> &connection);

	// Setters
	void setTime(const long long &time);

	// Getters
	bool timedOut() const;

	void handleRequest(istream &stream);
};

#endif