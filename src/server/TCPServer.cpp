//
// Created by jelle on 3/1/2021.
//

#include "server/TCPServer.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <algorithm>
#include <vector>
#include <map>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int	get_maxFD(std::vector<int> &set1, std::vector<int> &set2) {
	if (set1.empty() && set2.empty())
		return 0;
	if (!set1.empty()) std::sort(set1.begin(), set1.end());
	if (!set2.empty()) std::sort(set2.begin(), set2.end());
	if (set1.empty()) return set2.back();
	if (set2.empty()) return set1.back();
	return set1.back() > set2.back() ? set1.back() : set2.back();
}

TCPServer::TCPServer() {}

void TCPServer::listen(int port) {
	int one = 1;
	struct sockaddr_in svr_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);
	fd_set	read_fdset;
	fd_set	write_fdset;
	std::vector<int>	read_fd;
	std::vector<int>	write_fd;
	std::map<int, std::string>	connection_read;

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		logItem(log::ERROR, "Socket opening failed!");
		return;
	}
	read_fd.push_back(sock);

	// set socket options
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
		close(sock);
		logItem(log::ERROR, "Bind failed!");
		return;
	}

	::listen(sock, 5); // sockfd, max connections
	logItem(log::INFO, "Server is listening!");
	while (true) {
		// setup select
		FD_ZERO(&read_fdset);
		FD_ZERO(&write_fdset);
		for (std::vector<int>::iterator first = read_fd.begin(); first != read_fd.end(); ++first)
			FD_SET(*first, &read_fdset);
		for (std::vector<int>::iterator first = write_fd.begin(); first != write_fd.end(); ++first)
			FD_SET(*first, &write_fdset);
		int success = select(get_maxFD(read_fd, write_fd) + 1, &read_fdset, &write_fdset, NULL, NULL);
		(void)success;
		// accept new connections
		if (FD_ISSET(sock, &read_fdset)) {
			int client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
			logItem(log::DEBUG, "Client connected");

			if (client_fd == -1) {
				logItem(log::ERROR, "Client not accepted");
				continue;
			}

			fcntl(client_fd, F_SETFL, O_NONBLOCK);
			read_fd.push_back(client_fd);
			connection_read[client_fd] = "";
			continue;
		}

		// check readable fds
		for (std::vector<int>::iterator first = read_fd.begin(); first != read_fd.end(); ++first) {
			if (FD_ISSET(*first, &read_fdset)) {
				char	buf[1025];
				ssize_t	ret = recv(*first, buf, sizeof(buf)-1, 0);
				if (ret > 0) {
					// packet found, reading
					buf[ret] = 0;
					connection_read[*first] += buf;
				}
				else if (ret == -1) {
					// error reading
					logItem(log::WARNING, "Failed to read from client");
					logItem(log::ERROR, strerror(errno));
					logItem(log::DEBUG, connection_read[*first]);
				}
				else {
					// connection closed
					logItem(log::DEBUG, "Reached EOF of client");
					printf("Size: %li\n", connection_read[*first].length());
					logItem(log::DEBUG, "Client data:\n" + connection_read[*first]);
					close(*first);
					read_fd.erase(first);
					break;
				}

				if (connection_read[*first].rfind("Test v1.1") == 0) {
					// has read full data, start responding
					logItem(log::DEBUG, "Client has sent in format, adding to write queue");
					write_fd.push_back(*first);
					read_fd.erase(first);
					break;
				}
				break;
			}
		}

		// check writable fds
		for (std::vector<int>::iterator first = write_fd.begin(); first != write_fd.end(); ++first) {
			if (FD_ISSET(*first, &write_fdset)) {
				// send response
				send(*first, "Hello world!\n", 13, 0);

				// close connection
				close(*first);
				write_fd.erase(first);
				break;
			}
		}
	}
}
