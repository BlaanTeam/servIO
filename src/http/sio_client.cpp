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
	reset();
	_req.consumeStream(stream);

	string host = _req.getHeaders()["Host"];
	trim(host);
	VirtualServer *virtualServer = config.match(Address(_connection.first), host);

	if (!_req.valid()) {
		_res.setupErrorResponse(BAD_REQUEST, virtualServer, true);
		goto purgeConnection;
	}

	if (_req.match(REQ_BODY | REQ_DONE) && virtualServer) {
		if (_res.match(RES_INIT | RES_HEADER)) {
			Location *location = virtualServer->match(_req.getPath());
			string    path = _req.getPath();

			if (virtualServer->isRedirectable()) {
				_res.setupRedirectResponse(virtualServer->getRedir(), virtualServer);
				goto sendResponse;
			}

			if (!location) {  //! the location does not inherit the `return`
				_res.setupErrorResponse(NOT_FOUND, virtualServer, true);
				goto sendResponse;
			} else if (location->isRedirectable()) {
				_res.setupRedirectResponse(location->getRedir(), location);
				goto sendResponse;
			} else if (!location->isAllowedMethod(_req.getMethod())) {
				_res.setupErrorResponse(METHOD_NOT_ALLOWED, location, true);
				goto sendResponse;
			}

			// TODO: check if the location configured as CGI or UPLOAD

			struct stat fileStat;
			bzero(&fileStat, sizeof fileStat);
			if (!location->found(path, fileStat)) {
				_res.setupErrorResponse(NOT_FOUND, location, true);
			} else if (S_ISDIR(fileStat.st_mode)) {
				// ? INFO : redirect in case uri without `/` in the ending
				if (_req.getPath().length() > 1 && _req.getPath()[_req.getPath().length() - 1] != '/') {
					Redirect redir(MOVED_PERMANENTLY, joinPath(_req.getPath(), "/"), true);
					_res.setupRedirectResponse(&redir, location);
					goto sendResponse;
				}

				string tmp = joinPath(path, location->getIndex());
				if (!access(tmp.c_str(), F_OK | R_OK))
					_res.setupNormalResponse(tmp, new fstream(tmp, ios::in));
				else if (location->isAutoIndexable())
					_res.setupDirectoryListing(path, _req.getPath());
				else
					_res.setupErrorResponse(FORBIDDEN, location, true);
			} else {
				if (!access(path.c_str(), F_OK | R_OK))
					_res.setupNormalResponse(path, new fstream(path, ios::in));
				else
					_res.setupErrorResponse(FORBIDDEN, location, true);
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

purgeConnection:

	if (_res.keepAlive() || !_res.match(RES_DONE))
		return false;
	return clients.purgeConnection(_connection.first);
}

void Client::handleResponse(const sockfd &fd) {
	_res.send(fd);

	if (_res.match(RES_BODY))
		_pfd->events |= POLLOUT;
	else if (_res.match(RES_DONE | RES_INIT))
		_pfd->events &= ~POLLOUT;
	reset();
}

void Client::reset(void) {
	if (_req.match(REQ_DONE) && _res.match(RES_DONE)) {
		_req.reset();
		_res.reset();
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