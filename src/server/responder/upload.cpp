//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include <cerrno>
#include <fcntl.h>

using namespace NotApache;

void HTTPResponder::uploadFile(HTTPClient &client) {
	struct ::stat buf = {};
	std::string message = "Successfully updated file!";

	// get file data
	if (::stat(client.file.path.c_str(), &buf) == -1) {
		if (errno != ENOENT) {
			if (errno == ENOTDIR)
				handleError(client, 404);
			else
				handleError(client, 500);
			return;
		}
		message = "Successfully created file!";
	}
	else {
		// 403 on anything that isnt a regular file
		if (!S_ISREG(buf.st_mode)) {
			handleError(client, 403);
			return;
		}
	}

	// create the file
	FD uploadFd = ::open(client.file.path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (uploadFd == -1) {
		if (errno == ENOENT)
			handleError(client, 404);
		else
			handleError(client, 500);
		return;
	}

	// setup client for uploading
	client.data.response.builder.setHeader("Content-Type", "text/html");
	client.data.response.builder.setBody(std::string("<h1>") + message + "</h1>");
	client.replyStatus = 200;
	client.addAssociatedFd(uploadFd, associatedFD::WRITE);
	client.responseState = NotApache::UPLOAD;
	client.connectionState = ASSOCIATED_FD;
}
