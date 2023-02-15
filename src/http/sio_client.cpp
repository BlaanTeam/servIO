#include "sio_client.hpp"

Client::Client() {
	_time = getmstime();
}

Client::Client(const pair<sockfd, Address> &connection) {
	_time = getmstime();
	_connection = connection;
}

// Setters

void Client::setTime(const long long &time) {
	_time = time;
}

// Getters

bool Client::timedOut(void) const {
	return getmstime() - _time > TIMEOUT;
}

#include <fstream>

void Client::handleRequest(istream &stream) {
	_req.consumeStream(stream);

	if (!_req.valid()) {
		_res.setStatusCode(BAD_REQUEST);
		_res.setConnectionStatus(false);
		_res.addHeader("Content-Type", mimeTypes["html"]);

		stringstream ss;
		buildResponseBody(BAD_REQUEST, ss);

		_res.send(_connection.first, ss);

		clients.purgeConnection(_connection.first);
	} else if (_req.getState() & REQ_DONE) {
		// Example Start !
		fstream file("html/index.html");

		_res.setStatusCode(200);
		_res.setConnectionStatus(false);
		_res.addHeader("Content-Type", mimeTypes["html"]);

		_res.send(_connection.first, file);
		clients.purgeConnection(_connection.first);
		// Example End !
	}
}

ClientMap::ClientMap() {
	_pfds = nullptr;
}

void ClientMap::changePollFds(PollFd *pfds) {
	_pfds = pfds;
}

int ClientMap::purgeConnection(const sockfd &fd) {
	iterator it = find(fd);
	if (it != end()) {
		_pfds->remove(fd);
		erase(it);
		close(fd);
		return 1;
	}
	return 0;
}

const vector<sockfd> ClientMap::getInactiveClients(void) {
	vector<sockfd> inactiveClients;
	for (iterator it = begin(); it != end(); it++)
		if (it->second.timedOut())
			inactiveClients.push_back(it->first);
	return inactiveClients;
}

int ClientMap::purgeInactiveClients() {
	int            count = 0;
	vector<sockfd> inactiveClients = getInactiveClients();

	vector<sockfd>::iterator it = inactiveClients.begin();

	while (it != inactiveClients.end())
		if (clients[*it].timedOut())
			count += clients.purgeConnection(*it++);
	return count;
}

ClientMap clients;