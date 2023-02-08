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

void Client::handleRequest(istream &stream) {
	req.consumeStream(stream);
}
