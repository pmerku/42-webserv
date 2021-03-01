//
// Created by jelle on 3/1/2021.
//

#include "server/TCPServer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>

#include <sys/types.h>

TCPServer::TCPServer() {}

void TCPServer::listen(int port) {
	int one = 1, client_fd;
	struct sockaddr_in svr_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		logItem(log::ERROR, "Socket opening failed!");
		return;
	}

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
	while (1) {
		client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
		logItem(log::DEBUG, "Client connected");

		if (client_fd == -1) {
			perror("Can't accept");
			logItem(log::ERROR, "Client not accepted");
			continue;
		}

		ssize_t oof = write(client_fd, "Hello world!\n", 13);
		if (oof == -1)
			logItem(log::ERROR, "Failed to write to client");
		close(client_fd);
	}
}
