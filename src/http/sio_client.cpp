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

void Client::setPollFd(const sockfd &fd, PollFd &pfd) {
	PollFd::iterator it = pfd.get(fd);
	if (it != pfd.end())
		_pfd = &(*it);
}

// Getters

bool Client::timedOut(void) const {
	return getmstime() - _time > TIMEOUT;
}

#include <fstream>

bool Client::handleRequest(stringstream &stream) {
	_req.consumeStream(stream);

	if (!_req.valid()) {
		_res.sendError(_connection.first, BAD_REQUEST);
		goto purgeConnection;
	}
	if (_req.match(REQ_BODY | REQ_DONE)) {
		if (_req.getMethod() & UNKNOWN) {
			_res.sendError(_connection.first, METHOD_NOT_ALLOWED);
			goto purgeConnection;
		}

		if (_res.match(RES_INIT | RES_HEADER)) {
			fstream *file = new fstream("html/index.html");
			_res.setStatusCode(200);
			_res.addHeader("Content-Type", mimeTypes["html"]);
			_res.setStream(file);
		}

		_res.send(_connection.first);
		if (!_res.match(RES_DONE))
			setTime(getmstime());
	}
	if (_res.match(RES_BODY))
		_pfd->events |= POLLOUT;
	else if (_res.match(RES_DONE | RES_INIT))
		_pfd->events &= ~POLLOUT;

	return false;
purgeConnection:
	return clients.purgeConnection(_connection.first);
}

void Client::send(const sockfd &fd) {
	return _res.send(fd);
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