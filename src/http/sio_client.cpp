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

bool Client::handleRequest(stringstream &stream) {
	_req.consumeStream(stream);
	string host = _req.getHeaders()["Host"];
	trim(host);
	VirtualServer *virtualServer = config.match(Address(_connection.first), host);

	if (!_req.valid()) {
		_res.setupErrorResponse(BAD_REQUEST);
		goto purgeConnection;
	}

	if (_req.match(REQ_BODY | REQ_DONE) && virtualServer) {
		if (_res.match(RES_INIT | RES_HEADER)) {
			Location *location = virtualServer->match(_req.getPath());
			string    path = _req.getPath();

			if (_req.getMethod() & UNKNOWN || (location && !location->isAllowedMethod(_req.getMethod()))) {
				_res.setupErrorResponse(METHOD_NOT_ALLOWED);
				goto sendResponse;
			}

			if (virtualServer->isRedirectable()) {
				// Handler Return logic !
				cerr << "Redicection !" << endl;
				return false;
			}

			struct stat fileStat;
			bzero(&fileStat, sizeof fileStat);
			if (!location || (!location->found(path, fileStat) && !location->isRedirectable())) {
				_res.setupErrorResponse(NOT_FOUND);
				goto sendResponse;
			}

			// TODO: check if the location configured as CGI or UPLOAD or REDIRECT !

			if (S_ISDIR(fileStat.st_mode)) {
				string tmp = path + location->getIndex();
				if (!access(tmp.c_str(), F_OK | R_OK))
					_res.setupNormalResponse(tmp, new fstream(tmp, ios::in));
				else if (location->isAutoIndexable())
					_res.setupDirectoryListing(path, _req.getPath());
				else
					_res.setupErrorResponse(FORBIDDEN);
			} else {
				if (!access(path.c_str(), F_OK | R_OK))
					_res.setupNormalResponse(path, new fstream(path, ios::in));
				else
					_res.setupErrorResponse(FORBIDDEN);
			}
		}
	sendResponse:
		_res.send(_connection.first);
		if (!_res.match(RES_DONE))
			setTime(getmstime());
	}

	if (_res.match(RES_BODY))
		_pfd->events |= POLLOUT;
	else if (_res.match(RES_DONE | RES_INIT))
		_pfd->events &= ~POLLOUT;

	if (_res.keepAlive())
		return false;

purgeConnection:
	return clients.purgeConnection(_connection.first);
}

void Client::handleResponse(const sockfd &fd) {
	_res.send(fd);

	if (_res.match(RES_BODY))
		_pfd->events |= POLLOUT;
	else if (_res.match(RES_DONE | RES_INIT))
		_pfd->events &= ~POLLOUT;
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